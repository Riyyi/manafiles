/*
 * Copyright (C) 2021 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef DOTFILE_H
#define DOTFILE_H

#include <filesystem>
#include <string>
#include <vector>

namespace Util {

class Dotfile {
public:
	Dotfile();
	virtual ~Dotfile();

	enum class ExcludeType {
		File,
		Directory,
		EndsWith,
	};

	struct ExcludePath {
		ExcludeType type;
		std::string path;
	};

	static void list();

	static void setWorkingDirectory(std::filesystem::path directory) { s_workingDirectory = directory; }
	static void setExcludePaths(const std::vector<ExcludePath>& excludePaths) { s_excludePaths = excludePaths; }

private:
	static bool filter(const std::filesystem::path& path);

	static std::vector<ExcludePath> s_excludePaths;
	static std::filesystem::path s_workingDirectory;
};

} // namespace Util

#endif // DOTFILE_H
