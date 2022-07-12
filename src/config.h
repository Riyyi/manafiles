/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef>    // size_t
#include <filesystem> // path
#include <string>
#include <vector>

#include "util/singleton.h"
#include "util/json/value.h"

struct Settings {
	std::vector<std::string> ignorePatterns {
		".git/",
		"*.md",
		"manafiles.json",
		"packages",
		"README.org",
		"screenshot.png",
	};
	std::vector<std::string> systemPatterns {
		"/boot/",
		"/etc/",
		"/usr/share/"
	};
};

class Config : public Util::Singleton<Config> {
public:
	Config(s);
	virtual ~Config();

	void setSystemPatterns(const std::vector<std::string>& systemPatterns) { m_settings.systemPatterns = systemPatterns; }
	void setIgnorePatterns(const std::vector<std::string>& ignorePatterns) { m_settings.ignorePatterns = ignorePatterns; }
	void setVerbose(bool verbose) { m_verbose = verbose; }

	const std::vector<std::string>& ignorePatterns() const { return m_settings.ignorePatterns; }
	const std::vector<std::string>& systemPatterns() const { return m_settings.systemPatterns; }

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

// Json arbitrary type conversion functions

void toJson(Json::Value& object, const Settings& settings);
void fromJson(const Json::Value& object, Settings& settings);

#endif // CONFIG_H
