/*
 * Copyright (C) 2021 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef DOTFILE_H
#define DOTFILE_H

#include <cstddef> // size_t
#include <filesystem>
#include <functional> // function
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

	void add(const std::vector<std::string>& targets = {});
	void list(const std::vector<std::string>& targets = {});

	static void setWorkingDirectory(std::filesystem::path directory) { s_workingDirectory = directory; }
	static void setSystemDirectories(const std::vector<std::filesystem::path>& systemDirectories) { s_systemDirectories = systemDirectories; }
	static void setExcludePaths(const std::vector<ExcludePath>& excludePaths) { s_excludePaths = excludePaths; }

private:
	void forEachDotfile(const std::vector<std::string>& targets, const std::function<void(std::filesystem::directory_entry, size_t)>& callback);
	bool filter(const std::filesystem::path& path);
	bool include(const std::filesystem::path& path, const std::vector<std::string>& targets);

	static std::vector<ExcludePath> s_excludePaths;
	static std::vector<std::filesystem::path> s_systemDirectories;
	static std::filesystem::path s_workingDirectory;
};

} // namespace Util

#endif // DOTFILE_H
