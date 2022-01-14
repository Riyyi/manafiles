/*
 * Copyright (C) 2021-2022 Riyyi
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

#include "util/singleton.h"

class Dotfile : public Util::Singleton<Dotfile> {
public:
	Dotfile(s);
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

	void setWorkingDirectory(std::filesystem::path directory)
	{
		m_workingDirectory = directory;
		m_workingDirectorySize = directory.string().size();
	}
	void setSystemDirectories(const std::vector<std::filesystem::path>& systemDirectories) { m_systemDirectories = systemDirectories; }
	void setExcludePaths(const std::vector<ExcludePath>& excludePaths) { m_excludePaths = excludePaths; }

private:
	void pullOrPush(SyncType type, const std::vector<std::string>& targets = {});
	void sync(SyncType type,
	          const std::vector<std::string>& paths, const std::vector<size_t>& homeIndices, const std::vector<size_t>& systemIndices,
	          const std::function<void(std::string*, const std::string&, const std::string&)>& generateHomePaths,
	          const std::function<void(std::string*, const std::string&)>& generateSystemPaths);
	void selectivelyCommentOrUncomment(const std::string& path);

	void forEachDotfile(const std::vector<std::string>& targets, const std::function<void(const std::filesystem::directory_entry&, size_t)>& callback);
	bool filter(const std::filesystem::path& path);
	bool include(const std::filesystem::path& path, const std::vector<std::string>& targets);
	bool isSystemTarget(const std::string& target);

	std::vector<ExcludePath> m_excludePaths;
	std::vector<std::filesystem::path> m_systemDirectories;
	std::filesystem::path m_workingDirectory;
	size_t m_workingDirectorySize { 0 };
};

#endif // DOTFILE_H
