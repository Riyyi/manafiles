/*
 * Copyright (C) 2021-2022,2025 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cctype>  // tolower
#include <cstddef> // size_t
#include <cstdio>  // fprintf, printf, stderr
#include <filesystem>
#include <functional> // function
#include <pwd.h>      // getpwnam
#include <string>
#include <system_error> // error_code
#include <unistd.h>     // geteuid, getlogin, setegid, seteuid
#include <vector>

#include "ruc/file.h"
#include "ruc/meta/assert.h"

#include "config.h"
#include "dotfile.h"
#include "machine.h"

Dotfile::Dotfile(s)
{
}

Dotfile::~Dotfile()
{
}

// -----------------------------------------

void Dotfile::add(const std::vector<std::string>& targets)
{
	if (targets.empty()) {
		fprintf(stderr, "\033[31;1mDotfile:\033[0m no files or directories selected\n");
		return;
	}

	std::vector<size_t> noExistIndices;
	std::vector<size_t> homeIndices;
	std::vector<size_t> systemIndices;

	// Separate home and system targets
	for (size_t i = 0; i < targets.size(); ++i) {
		if (!std::filesystem::is_regular_file(targets.at(i))
		    && !std::filesystem::is_directory(targets.at(i))
		    && !std::filesystem::is_symlink(targets.at(i))) {
			noExistIndices.push_back(i);
			continue;
		}

		if (match(targets.at(i), Config::the().systemPatterns())) {
			systemIndices.push_back(i);
		}
		else {
			homeIndices.push_back(i);
		}
	}

	// Print non-existing targets and exit
	if (!noExistIndices.empty()) {
		for (size_t i : noExistIndices) {
			fprintf(stderr, "\033[31;1mDotfile:\033[0m '%s': no such file or directory\n", targets.at(i).c_str());
		}
	}

	sync(
		SyncType::Add, targets, homeIndices, systemIndices,
		[](std::string* paths, const std::string& homePath, const std::string& homeDirectory) {
			paths[0] = homePath;
			paths[1] = homePath.substr(homeDirectory.size() + 1);
		},
		[](std::string* paths, const std::string& systemPath) {
			paths[0] = systemPath;
			paths[1] = systemPath.substr(1);
		});
}

void Dotfile::list(const std::vector<std::string>& targets)
{
	if (Config::the().workingDirectory().empty()) {
		fprintf(stderr, "\033[31;1mDotfile:\033[0m working directory is unset\n");
		return;
	}

	if (!std::filesystem::is_directory(Config::the().workingDirectory())) {
		fprintf(stderr, "\033[31;1mDotfile:\033[0m working directory is not a directory\n");
		return;
	}

	forEachDotfile(targets, [](std::filesystem::directory_entry path, size_t) {
		printf("%s\n", path.path().c_str() + Config::the().workingDirectorySize() + 1);
	});
}

void Dotfile::pull(const std::vector<std::string>& targets)
{
	pullOrPush(SyncType::Pull, targets);
}

void Dotfile::push(const std::vector<std::string>& targets)
{
	pullOrPush(SyncType::Push, targets);
}

bool Dotfile::match(const std::string& path, const std::vector<std::string>& patterns)
{
	VERIFY(path.front() == '/', "path is not absolute: '{}'", path);

	// Cut off working directory
	size_t cutFrom = path.find(Config::the().workingDirectory()) == 0 ? Config::the().workingDirectorySize() : 0;
	std::string pathString = path.substr(cutFrom);

	for (const auto& pattern : patterns) {
		// A dot matches everything in the current working directory
		if (pattern == ".") {
			return true;
		}

		// Exact match is obviously true
		if (pathString == pattern) {
			return true;
		}

		// If starts with '/', only match in the working directory root
		bool onlyMatchInRoot = false;
		if (pattern.front() == '/') {
			onlyMatchInRoot = true;
		}

		// If ends with '/', only match directories
		bool onlyMatchDirectories = false;
		if (pattern.back() == '/') {
			onlyMatchDirectories = true;
		}

		// Parsing

		bool tryPatternState = true;

		size_t pathIterator = 0;
		size_t patternIterator = 0;

		if (!onlyMatchInRoot) {
			pathIterator++;
		}

		// Current path charter 'x' == next ignore pattern characters '*x'
		// Example, iterator at []: [.]log/output.txt
		//                          [*].log
		if (pathIterator < pathString.length()
		    && patternIterator < pattern.length() - 1
		    && pattern.at(patternIterator) == '*'
		    && pathString.at(pathIterator) == pattern.at(patternIterator + 1)) {
			patternIterator++;
		}

		for (; pathIterator < pathString.length() && patternIterator < pattern.length();) {
			char character = pathString.at(pathIterator);
			pathIterator++;

			if (!tryPatternState && character == '/') {
				tryPatternState = true;
				continue;
			}

			if (!tryPatternState) {
				continue;
			}

			if (character == pattern.at(patternIterator)) {
				// Fail if the final match hasn't reached the end of the ignore pattern
				// Example, iterator at []: doc/buil[d]
				//                          buil[d]/
				if (pathIterator == pathString.length() && patternIterator < pattern.length() - 1) {
					break;
				}

				// Next path character 'x' == next ignore pattern characters '*x', skip the '*'
				// Example, iterator at []: /includ[e]/header.h
				//                          /includ[e]*/
				if (pathIterator < pathString.length()
				    && patternIterator < pattern.length() - 2
				    && pattern.at(patternIterator + 1) == '*'
				    && pathString.at(pathIterator) == pattern.at(patternIterator + 2)) {
					patternIterator++;
				}

				patternIterator++;
				continue;
			}

			if (pattern.at(patternIterator) == '*') {
				// Fail if we're entering a subdirectory and we should only match in the root
				// Example, iterator at []: /src[/]include/header.h
				//                          /[*]include/
				if (onlyMatchInRoot && character == '/') {
					break;
				}

				// Next path character == next ignore pattern character
				if (pathIterator < pathString.length()
				    && patternIterator + 1 < pattern.length()
				    && pathString.at(pathIterator) == pattern.at(patternIterator + 1)) {
					patternIterator++;
				}

				continue;
			}

			// Reset filter pattern if it hasnt been completed at this point
			// Example, iterator at []: /[s]rc/include/header.h
			//                          /[i]nclude*/
			if (patternIterator < pattern.length() - 1) {
				patternIterator = 0;
			}

			tryPatternState = false;
		}

		if (patternIterator == pattern.length()) {
			return true;
		}
		if (pattern.back() == '*' && patternIterator == pattern.length() - 1) {
			return true;
		}
		if (onlyMatchDirectories && patternIterator == pattern.length() - 1) {
			return true;
		}
	}

	return false;
}

// -----------------------------------------

void Dotfile::pullOrPush(SyncType type, const std::vector<std::string>& targets)
{
	std::vector<std::string> dotfiles;
	std::vector<size_t> homeIndices;
	std::vector<size_t> systemIndices;

	// Separate home and system targets
	forEachDotfile(targets, [&](const std::filesystem::directory_entry& path, size_t index) {
		dotfiles.push_back(path.path().string());
		if (match(path.path().string(), Config::the().systemPatterns())) {
			systemIndices.push_back(index);
		}
		else {
			homeIndices.push_back(index);
		}
	});

	if (type == SyncType::Pull) {
		sync(
			type, dotfiles, homeIndices, systemIndices,
			[](std::string* paths, const std::string& homeFile, const std::string& homeDirectory) {
				// homeFile = /home/<user>/dotfiles/<file>
			    // copy: /home/<user>/<file>  ->  /home/<user>/dotfiles/<file>
				paths[0] = homeDirectory + homeFile.substr(Config::the().workingDirectorySize());
				paths[1] = homeFile;
			},
			[](std::string* paths, const std::string& systemFile) {
				// systemFile = /home/<user>/dotfiles/<file>
			    // copy: <file>  ->  /home/<user>/dotfiles/<file>
				paths[0] = systemFile.substr(Config::the().workingDirectorySize());
				paths[1] = systemFile;
			});
	}
	else {
		sync(
			type, dotfiles, homeIndices, systemIndices,
			[](std::string* paths, const std::string& homeFile, const std::string& homeDirectory) {
				// homeFile = /home/<user>/dotfiles/<file>
			    // copy: /home/<user>/dotfiles/<file>  ->  /home/<user>/<file>
				paths[0] = homeFile;
				paths[1] = homeDirectory + homeFile.substr(Config::the().workingDirectorySize());
			},
			[](std::string* paths, const std::string& systemFile) {
				// systemFile = /home/<user>/dotfiles/<file>
			    // copy: /home/<user>/dotfiles/<file>  ->  <file>
				paths[0] = systemFile;
				paths[1] = systemFile.substr(Config::the().workingDirectorySize());
			});
	}
}

void Dotfile::sync(SyncType type,
                   const std::vector<std::string>& paths, const std::vector<size_t>& homeIndices, const std::vector<size_t>& systemIndices,
                   const std::function<void(std::string*, const std::string&, const std::string&)>& generateHomePaths,
                   const std::function<void(std::string*, const std::string&)>& generateSystemPaths)
{
	bool root = !geteuid() ? true : false;
	if (!systemIndices.empty() && !root) {
		for (size_t i : systemIndices) {
			fprintf(stderr, "\033[31;1mDotfile:\033[0m need root privileges to copy system file '%s'\n",
			        paths.at(i).c_str() + (type == SyncType::Add ? 0 : Config::the().workingDirectorySize()));
		}
	}

	auto printError = [](const std::filesystem::path& path, const std::error_code& error) -> void {
		if (error.value() && error.message() != "File exists") {
			fprintf(stderr, "\033[31;1mDotfile:\033[0m '%s': %c%s\n",
			        path.c_str(),
			        tolower(error.message().c_str()[0]),
			        error.message().c_str() + 1);
		}
	};

	const auto copyOptions = std::filesystem::copy_options::overwrite_existing
	                         | std::filesystem::copy_options::recursive
	                         | std::filesystem::copy_options::copy_symlinks;

	auto copy = [&root, &printError](const std::filesystem::path& from,
	                                 const std::filesystem::path& to, bool homePath) -> void {
		if (homePath && root) {
			setegid(Machine::the().gid());
			seteuid(Machine::the().uid());
		}

		// Create directory for the file
		std::error_code error;
		if (std::filesystem::is_regular_file(from) || std::filesystem::is_symlink(from)) {
			auto directory = to.parent_path();
			if (!directory.empty() && !std::filesystem::exists(directory)) {
				if (Config::the().verbose()) {
					printf("Created directory: '%s'\n", directory.c_str());
				}
				std::filesystem::create_directories(directory, error);
				printError(to.relative_path().parent_path(), error);
			}
		}

		// Copy the file or directory
		if (Config::the().verbose()) {
			printf("'%s' -> '%s'\n", from.c_str(), to.c_str());
		}
		if (std::filesystem::is_symlink(from)) {
			// NOTE: std::filesystem::copy doesnt respect 'overwrite_existing' for symlinks
			std::filesystem::remove(to, error);
			printError(to, error);
			std::filesystem::copy_symlink(from, to, error);
			printError(from, error);
		}
		else {
			std::filesystem::copy(from, to, copyOptions, error);
			printError(from, error);
		}

		if (homePath && root) {
			seteuid(0);
			setegid(0);
		}
	};

	// /home/<user>/
	std::string homeDirectory = "/home/" + Machine::the().username();
	for (size_t i : homeIndices) {
		std::string homePaths[2];
		generateHomePaths(homePaths, paths.at(i), homeDirectory);
		copy(homePaths[0], homePaths[1], true);
		if (type == SyncType::Push) {
			selectivelyCommentOrUncomment(homePaths[1]);
		}
	}
	// /
	for (size_t i : systemIndices) {
		std::string systemPaths[2];
		generateSystemPaths(systemPaths, paths.at(i));
		copy(systemPaths[0], systemPaths[1], false);
		if (type == SyncType::Push) {
			selectivelyCommentOrUncomment(systemPaths[1]);
		}
	}
}

void Dotfile::selectivelyCommentOrUncomment(const std::string& path)
{
	ruc::File dotfile(path);

	const std::string search[4] = {
		"distro=",
		"hostname=",
		"user=",
		"session=",
	};

	// State of the loop
	bool isFiltering = false;
	std::string filter[4];
	std::string commentCharacter;
	std::string commentTerminationCharacter;
	size_t positionInFile = 0;

	auto commentOrUncommentLine = [&](std::string& line, bool addComment) {
		size_t indentation = line.find_first_not_of(" \t");

		// Empty lines are skipped
		if (line.empty() || indentation == std::string::npos) {
			return;
		}

		size_t lineLength = line.size();
		size_t commentStart = line.find(commentCharacter, indentation);
		size_t commentEnd = line.rfind(commentTerminationCharacter);
		bool hasComment = commentStart != std::string::npos && (commentTerminationCharacter.empty() || commentEnd != std::string::npos);

		// Lines that have a comment at the *end* of the line aren't considered commented lines
		if (hasComment && line.find_first_not_of(" \t") < commentStart) {
			hasComment = false;
		}

		// Uncomment line
		if (hasComment && !addComment) {
			size_t contentStart = line.find_first_not_of(" \t", commentStart + commentCharacter.size());
			size_t contentLength = commentEnd - contentStart;

			// Trim trailing whitespace
			std::string content = line.substr(contentStart, contentLength);
			content = content.substr(0, content.find_last_not_of(" \t") + 1);

			line = line.substr(0, indentation)
			       + content;
		}
		// Comment line
		else if (!hasComment && addComment) {
			size_t contentStart = line.find_first_not_of(" \t");
			size_t contentLength = line.size() - contentStart;

			line = line.substr(0, indentation)
			       + commentCharacter + ' '
			       + line.substr(contentStart, contentLength)
			       + (!commentTerminationCharacter.empty() ? ' ' + commentTerminationCharacter : "");
		}

		dotfile.replace(positionInFile, lineLength, line);
	};

	std::istringstream stream(dotfile.data());
	for (std::string line; std::getline(stream, line); positionInFile += line.size() + 1) {

		if (line.find(">>>") != std::string::npos) {
			// Find machine info
			size_t find = 0;
			for (size_t i = 0; i < 4; ++i) {
				find = line.find(search[i]) + search[i].size();
				if (find < search[i].size()) {
					continue;
				}
				filter[i] = line.substr(find, line.find_first_of(' ', find) - find);
			}

			// Get the characters used for commenting in this file-type
			commentCharacter = line.substr(0, line.find_first_of(">>>"));
			for (size_t i = commentCharacter.size() - 1; i != std::string::npos; --i) {
				// Support for /* C-style comments */
				if (i > 0 && commentCharacter.at(i - 1) == '/' && commentCharacter.at(i) == '*') {
					commentTerminationCharacter = "*/";
				}
				// Support for <!-- XMl comments -->
				if (i > 0 && commentCharacter.at(i - 1) == '<' && commentCharacter.at(i) == '!' && commentCharacter.at(i + 1) == '-' && commentCharacter.at(i + 2) == '-') {
					commentTerminationCharacter = "-->";
				}
				// NOTE: Modification of the string should be at the end of the iteration to prevent 'out of range' errors
				if (commentCharacter.at(i) == ' ' || commentCharacter.at(i) == '\t') {
					commentCharacter.erase(i, 1);
				}
			}

			isFiltering = true;
			continue;
		}

		if (line.find("<<<") != std::string::npos) {
			isFiltering = false;
			filter[0] = "";
			filter[1] = "";
			filter[2] = "";
			filter[3] = "";
			commentCharacter.clear();
			commentTerminationCharacter.clear();
			continue;
		}

		if (!isFiltering) {
			continue;
		}

		if (filter[0] != Machine::the().distroId() && !filter[0].empty()) {
			commentOrUncommentLine(line, true);
			continue;
		}
		else if (filter[1] != Machine::the().hostname() && !filter[1].empty()) {
			commentOrUncommentLine(line, true);
			continue;
		}
		else if (filter[2] != Machine::the().username() && !filter[2].empty()) {
			commentOrUncommentLine(line, true);
			continue;
		}
		else if (filter[3] != Machine::the().session() && !filter[3].empty()) {
			commentOrUncommentLine(line, true);
			continue;
		}

		commentOrUncommentLine(line, false);
	}

	dotfile.flush();
}

void Dotfile::forEachDotfile(const std::vector<std::string>& targets, const std::function<void(const std::filesystem::directory_entry&, size_t)>& callback)
{
	size_t index = 0;
	for (const auto& path : std::filesystem::recursive_directory_iterator { Config::the().workingDirectory() }) {
		std::string pathString = path.path().string();

		if (path.is_directory()) {
			continue;
		}
		// Ignore pattern check
		if (match(pathString, Config::the().ignorePatterns())) {
			continue;
		}
		// Include check
		if (!targets.empty() && !match(pathString, targets)) {
			continue;
		}
		callback(path, index++);
	}
}
