/*
 * Copyright (C) 2021 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cctype>  // tolower
#include <cstddef> // size_t
#include <cstdio>  // fprintf, printf, stderr
#include <filesystem>
#include <pwd.h> // getpwnam
#include <string>
#include <system_error> // error_code
#include <unistd.h>     // geteuid, getlogin, setegid, seteuid
#include <vector>

#include "dotfile.h"

namespace Util {

std::vector<Dotfile::ExcludePath> Dotfile::s_excludePaths;
std::vector<std::filesystem::path> Dotfile::s_systemDirectories;
std::filesystem::path Dotfile::s_workingDirectory;

Dotfile::Dotfile()
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

	std::vector<size_t> noExistTargets;
	std::vector<size_t> homeTargets;
	std::vector<size_t> systemTargets;

	// Separate home and system targets
	bool isSystem = false;
	for (size_t i = 0; i < targets.size(); ++i) {
		if (!std::filesystem::is_regular_file(targets.at(i))
		    && !std::filesystem::is_directory(targets.at(i))
		    && !std::filesystem::is_symlink(targets.at(i))) {
			noExistTargets.push_back(i);
			continue;
		}

		isSystem = false;
		for (const auto& systemDirectory : s_systemDirectories) {
			if (targets.at(i).find(systemDirectory) == 0) {
				isSystem = true;
			}
		}

		if (isSystem) {
			systemTargets.push_back(i);
		}
		else {
			homeTargets.push_back(i);
		}
	}

	// Print non-existing targets and exit
	if (!noExistTargets.empty()) {
		for (size_t i : noExistTargets) {
			fprintf(stderr, "\033[31;1mDotfile:\033[0m '%s': no such file or directory\n", targets.at(i).c_str());
		}
		return;
	}

	// Print root-owned targets and exit
	bool root = !geteuid() ? true : false;
	if (!systemTargets.empty() && !root) {
		for (size_t i : systemTargets) {
			fprintf(stderr, "\033[31;1mDotfile:\033[0m you cannot copy system file '%s' unless you are root\n", targets.at(i).c_str());
		}
		return;
	}

	// Get the password database record (/etc/passwd) of the user logged in on
	// the controlling terminal of the process
	passwd* user = getpwnam(getlogin());

	const auto copyOptions = std::filesystem::copy_options::overwrite_existing
	                         | std::filesystem::copy_options::recursive
	                         | std::filesystem::copy_options::copy_symlinks;

	auto printError = [](const std::filesystem::path& path, const std::error_code& error) -> void {
		// std::filesystem::copy doesnt respect 'overwrite_existing' for symlinks
		if (error.value() && error.message() != "File exists") {
			fprintf(stderr, "\033[31;1mDotfile:\033[0m '%s': %c%s\n",
			        path.c_str(),
			        tolower(error.message().c_str()[0]),
			        error.message().c_str() + 1);
		}
	};

	auto copyTarget = [&root, &user, &printError](const std::filesystem::path& from,
	                                              const std::filesystem::path& to, bool homePath) -> void {
		if (homePath && root) {
			seteuid(user->pw_uid);
			setegid(user->pw_gid);
		}

		// Create directory for the file
		std::error_code error;
		if (std::filesystem::is_regular_file(from)) {
			auto directory = to.relative_path().parent_path();
			if (!directory.empty() && !std::filesystem::exists(directory) ) {
				printf("created directory '%s'\n", directory.c_str());
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
	std::string home = "/home/" + std::string(user->pw_name);
	for (size_t index : homeTargets) {
		copyTarget(std::filesystem::path(targets.at(index)),
		           std::filesystem::path(targets.at(index).substr(home.size() + 1)),
		           true);
	}
	// /
	for (size_t index : systemTargets) {
		auto path = std::filesystem::path(targets.at(index));
		copyTarget(path,
		           path.relative_path(),
		           false);
	}
}

void Dotfile::list(const std::vector<std::string>& targets)
{
	if (s_workingDirectory.empty()) {
		fprintf(stderr, "\033[31;1mDotfile:\033[0m working directory is unset\n");
		return;
	}

	if (!std::filesystem::is_directory(s_workingDirectory)) {
		fprintf(stderr, "\033[31;1mDotfile:\033[0m working directory is not a directory\n");
		return;
	}

	size_t workingDirectory = s_workingDirectory.string().size() + 1;
	for (const auto& path : std::filesystem::recursive_directory_iterator { s_workingDirectory }) {
		if (path.is_directory() || filter(path)) {
			continue;
		}
		if (!targets.empty() && !include(path.path().string(), targets)) {
			continue;
		}
		printf("%s\n", path.path().c_str() + workingDirectory);
	}
}

// -----------------------------------------

bool Dotfile::filter(const std::filesystem::path& path)
{
	for (auto& excludePath : s_excludePaths) {
		if (excludePath.type == ExcludeType::File) {
			if (path.string() == s_workingDirectory / excludePath.path) {
				return true;
			}
		}
		else if (excludePath.type == ExcludeType::Directory) {
			if (path.string().find(s_workingDirectory / excludePath.path) == 0) {
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
		if (path.string().find(s_workingDirectory / target) == 0) {
			return true;
		}
	}

	return false;
}

} // namespace Util
