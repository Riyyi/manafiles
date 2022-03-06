/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <csignal>    // raise
#include <cstdio>     // fprintf
#include <filesystem> // current_path, recursive_directory
#include <fstream>    // ifstream
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "config.h"

Config::Config(s)
	: m_workingDirectory(std::filesystem::current_path())
	, m_workingDirectorySize(m_workingDirectory.string().size())
{
	findConfigFile();
	parseConfigFile();
}

Config::~Config()
{
}

// -----------------------------------------

void Config::findConfigFile()
{
	std::string configFileName = "manafiles.json";

	for (const auto& path : std::filesystem::recursive_directory_iterator { m_workingDirectory }) {
		const auto& file = path.path().string();
		if (file.find(configFileName) + configFileName.size() == file.size()) {
			m_config = file;
		}
	}

#ifndef NDEBUG
	printf("Found config file @ %s\n", m_config.c_str() + m_workingDirectorySize + 1);
#endif
}

void Config::parseConfigFile()
{
	if (m_config.empty()) {
		return;
	}

	nlohmann::json json;

	std::ifstream file(m_config);
	if (!file.is_open()) {
		return;
	}

	try {
		file >> json;
	}
	catch (...) {
		fprintf(stderr, "\033[31;1mConfig:\033[0m json syntax error\n");
		raise(SIGABRT);
		return;
	}

	m_settings = json.get<Settings>();
}

// -----------------------------------------

void to_json(nlohmann::json& object, const Settings& settings)
{
	object = nlohmann::json {
		{ "ignorePatterns", settings.ignorePatterns },
		{ "systemDirectories", settings.systemDirectories }
	};
}

void from_json(const nlohmann::json& object, Settings& settings)
{
	if (object.find("ignorePatterns") != object.end()) {
		object.at("ignorePatterns").get_to(settings.ignorePatterns);
	}

	if (object.find("systemDirectories") != object.end()) {
		object.at("systemDirectories").get_to(settings.systemDirectories);
	}
}
