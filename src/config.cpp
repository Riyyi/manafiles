/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <string>

#include "config.h"

Config::Config(s)
	: m_workingDirectory(std::filesystem::current_path())
	, m_workingDirectorySize(m_workingDirectory.string().size())
{
	findConfigFile();
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
