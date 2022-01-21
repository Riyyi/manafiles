/*
 * Copyright (C) 2021-2022 Riyyi
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

#include "dotfile.h"
#include "machine.h"
#include "util/file.h"

Dotfile::Dotfile(s)
	: m_workingDirectory(std::filesystem::current_path())
	, m_workingDirectorySize(m_workingDirectory.string().size())
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

		if (isSystemTarget(targets.at(i))) {
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
		return;
	}

	sync(
		SyncType::Pull, targets, homeIndices, systemIndices,
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
	if (m_workingDirectory.empty()) {
		fprintf(stderr, "\033[31;1mDotfile:\033[0m working directory is unset\n");
		return;
	}

	if (!std::filesystem::is_directory(m_workingDirectory)) {
		fprintf(stderr, "\033[31;1mDotfile:\033[0m working directory is not a directory\n");
		return;
	}

	forEachDotfile(targets, [this](std::filesystem::directory_entry path, size_t) {
		printf("%s\n", path.path().c_str() + m_workingDirectorySize + 1);
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

// -----------------------------------------

void Dotfile::pullOrPush(SyncType type, const std::vector<std::string>& targets)
{
	std::vector<std::string> dotfiles;
	std::vector<size_t> homeIndices;
	std::vector<size_t> systemIndices;

	// Separate home and system targets
	forEachDotfile(targets, [&](const std::filesystem::directory_entry& path, size_t index) {
		dotfiles.push_back(path.path().string());
		if (isSystemTarget(path.path().string())) {
			systemIndices.push_back(index);
		}
		else {
			homeIndices.push_back(index);
		}
	});

	if (type == SyncType::Pull) {
		sync(
			type, dotfiles, homeIndices, systemIndices,
			[this](std::string* paths, const std::string& homeFile, const std::string& homeDirectory) {
				// homeFile = /home/<user>/dotfiles/<file>
			    // copy: /home/<user>/<file>  ->  /home/<user>/dotfiles/<file>
				paths[0] = homeDirectory + homeFile.substr(m_workingDirectorySize);
				paths[1] = homeFile;
			},
			[this](std::string* paths, const std::string& systemFile) {
				// systemFile = /home/<user>/dotfiles/<file>
			    // copy: <file>  ->  /home/<user>/dotfiles/<file>
				paths[0] = systemFile.substr(m_workingDirectorySize);
				paths[1] = systemFile;
			});
	}
	else {
		sync(
			type, dotfiles, homeIndices, systemIndices,
			[this](std::string* paths, const std::string& homeFile, const std::string& homeDirectory) {
				// homeFile = /home/<user>/dotfiles/<file>
			    // copy: /home/<user>/dotfiles/<file>  ->  /home/<user>/<file>
				paths[0] = homeFile;
				paths[1] = homeDirectory + homeFile.substr(m_workingDirectorySize);
			},
			[this](std::string* paths, const std::string& systemFile) {
				// systemFile = /home/<user>/dotfiles/<file>
			    // copy: /home/<user>/dotfiles/<file>  ->  <file>
				paths[0] = systemFile;
				paths[1] = systemFile.substr(m_workingDirectorySize);
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
			        paths.at(i).c_str() + m_workingDirectorySize);
		}
		return;
	}

	auto printError = [](const std::filesystem::path& path, const std::error_code& error) -> void {
		// std::filesystem::copy doesnt respect 'overwrite_existing' for symlinks
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
			seteuid(Machine::the().uid());
			setegid(Machine::the().gid());
		}

		// Create directory for the file
		std::error_code error;
		if (std::filesystem::is_regular_file(from)) {
			auto directory = to.parent_path();
			if (!directory.empty() && !std::filesystem::exists(directory)) {
				printf("Created directory: '%s'\n", directory.c_str());
				std::filesystem::create_directories(directory, error);
				printError(to.relative_path().parent_path(), error);
			}
		}

		// Copy the file or directory
		printf("'%s' -> '%s'\n", from.c_str(), to.c_str());
		std::filesystem::copy(from, to, copyOptions, error);
		printError(to, error);

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
	Util::File dotfile(path);

	const std::string search[3] = {
		"distro=",
		"hostname=",
		"user=",
	};

	// State of the loop
	bool isFiltering = false;
	std::string filter[3];
	std::string commentCharacter;
	size_t positionInFile = 0;

	auto commentOrUncommentLine = [&](std::string& line, bool addComment) {
		size_t lineLength = line.size();
		size_t whiteSpaceBeforeComment = line.find_first_of(commentCharacter);
		size_t contentAfterComment = line.find_first_not_of(commentCharacter + " \t");

		// If there was no comment, grab whitespace correctly
		if (whiteSpaceBeforeComment == std::string::npos) {
			whiteSpaceBeforeComment = contentAfterComment;
		}

		if (!addComment) {
			line = line.substr(0, whiteSpaceBeforeComment)
				+ line.substr(contentAfterComment);
		}
		else {
			line = line.substr(0, whiteSpaceBeforeComment)
				+ commentCharacter + ' '
				+ line.substr(contentAfterComment);
		}

		dotfile.replace(positionInFile, lineLength, line);
	};

	std::istringstream stream(dotfile.data());
	for (std::string line; std::getline(stream, line); positionInFile += line.size() + 1) {

		if (line.find(">>>") != std::string::npos) {
			// Find machine info
			size_t find = 0;
			for (size_t i = 0; i < 3; ++i) {
				find = line.find(search[i]) + search[i].size();
				if (find < search[i].size()) {
					continue;
				}
				filter[i] = line.substr(find, line.find_first_of(' ', find) - find);
			}

			// Get the characters used for commenting in this file-type
			commentCharacter = line.substr(0, line.find_first_of(">>>"));
			for (size_t i = commentCharacter.size() - 1; i != std::string::npos; --i) {
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
			commentCharacter.clear();
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

		commentOrUncommentLine(line, false);
	}

	dotfile.flush();
}

void Dotfile::forEachDotfile(const std::vector<std::string>& targets, const std::function<void(const std::filesystem::directory_entry&, size_t)>& callback)
{
	size_t index = 0;
	for (const auto& path : std::filesystem::recursive_directory_iterator { m_workingDirectory }) {
		if (path.is_directory() || filter(path)) {
			continue;
		}
		if (!targets.empty() && !include(path.path().string(), targets)) {
			continue;
		}
		callback(path, index++);
	}
}

bool Dotfile::filter(const std::filesystem::path& path)
{
	for (auto& excludePath : m_excludePaths) {
		if (excludePath.type == ExcludeType::File) {
			if (path.string() == m_workingDirectory / excludePath.path) {
				return true;
			}
		}
		else if (excludePath.type == ExcludeType::Directory) {
			if (path.string().find(m_workingDirectory / excludePath.path) == 0) {
				return true;
			}
		}
		else if (excludePath.type == ExcludeType::EndsWith) {
			if (path.string().find(excludePath.path) == path.string().size() - excludePath.path.size()) {
				return true;
			}
		}
	}

	return false;
}

bool Dotfile::include(const std::filesystem::path& path, const std::vector<std::string>& targets)
{
	for (const auto& target : targets) {
		if (path.string().find(m_workingDirectory / target) == 0) {
			return true;
		}
	}

	return false;
}

bool Dotfile::isSystemTarget(const std::string& target)
{
	for (const auto& systemDirectory : m_systemDirectories) {

		if (target.find(systemDirectory) == 0) {
			return true;
		}
		if (target.find(m_workingDirectory / systemDirectory) == 0) {
			return true;
		}
	}

	return false;
}
