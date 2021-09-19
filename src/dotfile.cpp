/*
 * Copyright (C) 2021 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio> // fprintf, printf
#include <filesystem>
#include <string>
#include <vector>

#include "dotfile.h"

namespace Util {

std::vector<Dotfile::ExcludePath> Dotfile::s_excludePaths;
std::filesystem::path Dotfile::s_workingDirectory;

Dotfile::Dotfile()
{
}

Dotfile::~Dotfile()
{
}

// -----------------------------------------

void Dotfile::list()
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

} // namespace Util
