/*
 * Copyright (C) 2021-2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <filesystem>
#include <functional> // function
#include <string>
#include <vector>

#include "ruc/singleton.h"

class Dotfile : public ruc::Singleton<Dotfile> {
public:
	Dotfile(s);
	virtual ~Dotfile();

	enum class SyncType : uint8_t {
		Add,
		Pull,
		Push,
	};

	void add(const std::vector<std::string>& targets = {});
	void list(const std::vector<std::string>& targets = {});
	void pull(const std::vector<std::string>& targets = {});
	void push(const std::vector<std::string>& targets = {});

	bool match(const std::string& path, const std::vector<std::string>& patterns);

private:
	void pullOrPush(SyncType type, const std::vector<std::string>& targets = {});
	void sync(SyncType type,
	          const std::vector<std::string>& paths, const std::vector<size_t>& homeIndices, const std::vector<size_t>& systemIndices,
	          const std::function<void(std::string*, const std::string&, const std::string&)>& generateHomePaths,
	          const std::function<void(std::string*, const std::string&)>& generateSystemPaths);
	void selectivelyCommentOrUncomment(const std::string& path);

	void forEachDotfile(const std::vector<std::string>& targets, const std::function<void(const std::filesystem::directory_entry&, size_t)>& callback);
};
