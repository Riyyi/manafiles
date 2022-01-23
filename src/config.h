/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "util/singleton.h"

class Config : public Util::Singleton<Config> {
public:
	Config(s) {}
	virtual ~Config() {}

	void setVerbose(bool verbose) { m_verbose = verbose; }

	bool verbose() const { return m_verbose; }

private:
	bool m_verbose { false };
};

#endif // CONFIG_H
