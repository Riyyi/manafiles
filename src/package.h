/*
 * Copyright (C) 2021 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>
#include <vector>

class Package {
public:
	Package();
	virtual ~Package();

	enum class Distro {
		Unsupported,
		Arch,
		Debian,
	};

	void aurInstall();
	void install();
	void list(const std::vector<std::string>& targets = {});
	void store();

	static void setHostname(const std::string& hostname) { m_hostname = hostname; }

private:
	bool distroDetect();
	bool distroDependencies();
	std::string getPackageList();

	static std::string m_hostname;

	Distro m_distro { Distro::Unsupported };
};

#endif // PACKAGE_H
