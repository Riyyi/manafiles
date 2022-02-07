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
}

Config::~Config()
{
}
