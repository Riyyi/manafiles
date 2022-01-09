/*
 * Copyright (C) 2021-2022 Riyyi
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

private:
	bool distroDetect();
	bool distroDependencies();
	std::string getPackageList();

	Distro m_distro { Distro::Unsupported };
};

#endif // PACKAGE_H
