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

	const std::string& distroId() const { return m_distroId; }
	const std::string& distroIdLike() const { return m_distroIdLike; }
	const std::string& hostname() const { return m_hostname; }

	std::string username() const { return std::string(m_passwd->pw_name); }
	uint32_t uid() const { return m_passwd->pw_uid; }
	uint32_t gid() const { return m_passwd->pw_gid; }

	const std::string& session() const { return m_session; }

private:
	void fetchDistro();
	void fetchHostname();
	void fetchUsername();
	void fetchSession();

	std::string m_distroId;
	std::string m_distroIdLike;
	std::string m_hostname;
	std::string m_session;
	passwd* m_passwd { nullptr };
};
