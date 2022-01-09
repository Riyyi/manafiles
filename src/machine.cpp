/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <pwd.h>    // getpwnam
#include <sstream>  // istringstream
#include <unistd.h> // gethostname, getlogin

#include "machine.h"
#include "util/file.h"

Machine::Machine(s)
{
	auto osRelease = Util::File("/etc/os-release");
	auto stream = std::istringstream(osRelease.data());
	std::string line;
	while (std::getline(stream, line)) {
		if (line.find("ID=") == 0) {
			m_distroId = line.substr(3);
		}
		if (line.find("ID_LIKE=") == 0) {
			m_distroIdLike = line.substr(8);
		}
	}

	char hostname[64] { 0 };
	if (gethostname(hostname, 64) < 0) {
		perror("\033[31;1mError:\033[0m gethostname");
	}
	m_hostname = hostname;

	// Get the username logged in on the controlling terminal of the process
	char username[32] { 0 };
	if (getlogin_r(username, 32) != 0) {
		perror("\033[31;1mError:\033[0m getlogin_r");
	}

	// Get the password database record (/etc/passwd) of the user
	m_passwd = getpwnam(username);
	if (m_passwd == nullptr) {
		perror("\033[31;1mError:\033[0m getpwnam");
	}
}

Machine::~Machine()
{
}
