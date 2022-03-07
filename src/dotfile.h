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

	void add(const std::vector<std::string>& targets = {});
	void list(const std::vector<std::string>& targets = {});
	void pull(const std::vector<std::string>& targets = {});
	void push(const std::vector<std::string>& targets = {});

	bool filter(const std::filesystem::directory_entry& path,
	            const std::vector<std::string>& patterns);

private:
	void pullOrPush(SyncType type, const std::vector<std::string>& targets = {});
	void sync(SyncType type,
	          const std::vector<std::string>& paths, const std::vector<size_t>& homeIndices, const std::vector<size_t>& systemIndices,
	          const std::function<void(std::string*, const std::string&, const std::string&)>& generateHomePaths,
	          const std::function<void(std::string*, const std::string&)>& generateSystemPaths);
	void selectivelyCommentOrUncomment(const std::string& path);

	void forEachDotfile(const std::vector<std::string>& targets, const std::function<void(const std::filesystem::directory_entry&, size_t)>& callback);
	bool isSystemTarget(const std::string& target);
};

#endif // DOTFILE_H
