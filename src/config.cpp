/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cassert>    // assert
#include <csignal>    // raise
#include <cstdio>     // fprintf
#include <filesystem> // current_path, recursive_directory
#include <fstream>    // ifstream
#include <string>
#include <vector>

#include "config.h"
#include "util/json/value.h"

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

	Json::Value json;

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

void toJson(Json::Value& json, const Settings& settings)
{
	json = Json::Value {
		{ "ignorePatterns", settings.ignorePatterns },
		{ "systemPatterns", settings.systemPatterns }
	};
}

void fromJson(const Json::Value& json, Settings& settings)
{
	assert(json.type() == Json::Value::Type::Object);

	if (json.exists("ignorePatterns")) {
		json.at("ignorePatterns").getTo(settings.ignorePatterns);
	}

	if (json.exists("systemPatterns")) {
		json.at("systemPatterns").getTo(settings.systemPatterns);
	}
}
