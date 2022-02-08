/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef>    // size_t
#include <filesystem> // path
#include <map>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "util/singleton.h"

struct Settings {
	std::map<std::string, std::string> excludePaths {
		{ ".git", "directory" },
		{ ".md", "endsWith" },
		{ "packages", "file" },
		{ "README.org", "endsWith" },
		{ "screenshot.png", "file" },
	};
	std::vector<std::filesystem::path> systemDirectories {
		"/boot",
		"/etc",
		"/usr/share"
	};
};

class Config : public Util::Singleton<Config> {
public:
	Config(s);
	virtual ~Config();

	void setVerbose(bool verbose) { m_verbose = verbose; }

	const std::map<std::string, std::string>& excludePaths() const { return m_settings.excludePaths; }
	const std::vector<std::filesystem::path>& systemDirectories() const { return m_settings.systemDirectories; }

	const std::filesystem::path& workingDirectory() const { return m_workingDirectory; }
	size_t workingDirectorySize() const { return m_workingDirectorySize; }

	bool verbose() const { return m_verbose; }

private:
	void findConfigFile();
	void parseConfigFile();

	bool m_verbose { false };

	std::filesystem::path m_workingDirectory {};
	size_t m_workingDirectorySize { 0 };

	std::filesystem::path m_config;
	Settings m_settings;
};

// -----------------------------------------

// nlohmann::json arbitrary type conversion functions

void to_json(nlohmann::json& object, const Settings& settings);
void from_json(const nlohmann::json& object, Settings& settings);

#endif // CONFIG_H
