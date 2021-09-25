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

	void aurInstall(const std::vector<std::string>& targets = {});
	void install(const std::vector<std::string>& targets = {});
	void list(const std::vector<std::string>& targets = {});
	void store(const std::vector<std::string>& targets = {});

private:
	bool distroDetect();
	bool distroDependencies();

	Distro m_distro { Distro::Unsupported };
};

#endif // PACKAGE_H
