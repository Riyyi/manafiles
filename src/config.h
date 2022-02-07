/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef> // size_t
#include <filesystem>

#include "util/singleton.h"

class Config : public Util::Singleton<Config> {
public:
	Config(s);
	virtual ~Config();

	void setVerbose(bool verbose) { m_verbose = verbose; }

	const std::filesystem::path& workingDirectory() const { return m_workingDirectory; }
	size_t workingDirectorySize() const { return m_workingDirectorySize; }
	bool verbose() const { return m_verbose; }

private:
	bool m_verbose { false };

	std::filesystem::path m_workingDirectory {};
	size_t m_workingDirectorySize { 0 };
};

#endif // CONFIG_H
