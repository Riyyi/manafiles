/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint> // uint32_t
#include <pwd.h>   // passwd
#include <string>

#include "ruc/singleton.h"

class Machine : public ruc::Singleton<Machine> {
public:
	Machine(s);
	virtual ~Machine();

	const std::string& distroId() { return m_distroId; }
	const std::string& distroIdLike() { return m_distroIdLike; }
	const std::string& hostname() { return m_hostname; }

	std::string username() { return m_passwd->pw_name; }
	uint32_t uid() { return m_passwd->pw_uid; }
	uint32_t gid() { return m_passwd->pw_gid; }

private:
	void fetchDistro();
	void fetchHostname();
	void fetchUsername();

	std::string m_distroId;
	std::string m_distroIdLike;
	std::string m_hostname;
	passwd* m_passwd { nullptr };
};
