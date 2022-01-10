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

class Dotfile {
public:
	Dotfile();
	virtual ~Dotfile();

	enum class SyncType {
		Pull,
		Push,
	};

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
	void pull(const std::vector<std::string>& targets = {});
	void push(const std::vector<std::string>& targets = {});

	static void setWorkingDirectory(std::filesystem::path directory)
	{
		s_workingDirectory = directory;
		s_workingDirectorySize = directory.string().size();
	}
	static void setSystemDirectories(const std::vector<std::filesystem::path>& systemDirectories) { s_systemDirectories = systemDirectories; }
	static void setExcludePaths(const std::vector<ExcludePath>& excludePaths) { s_excludePaths = excludePaths; }

private:
	void pullOrPush(SyncType type, const std::vector<std::string>& targets = {});
	void sync(const std::vector<std::string>& paths, const std::vector<size_t>& homeIndices, const std::vector<size_t>& systemIndices,
	          const std::function<void(std::string*, const std::string&, const std::string&)>& generateHomePaths,
	          const std::function<void(std::string*, const std::string&)>& generateSystemPaths);

	void forEachDotfile(const std::vector<std::string>& targets, const std::function<void(const std::filesystem::directory_entry&, size_t)>& callback);
	bool filter(const std::filesystem::path& path);
	bool include(const std::filesystem::path& path, const std::vector<std::string>& targets);
	bool isSystemTarget(const std::string& target);

	static std::vector<ExcludePath> s_excludePaths;
	static std::vector<std::filesystem::path> s_systemDirectories;
	static std::filesystem::path s_workingDirectory;
	static size_t s_workingDirectorySize;
};

#endif // DOTFILE_H
