/*
 * Copyright (C) 2021-2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PACKAGE_H
#define PACKAGE_H

#include <optional>
#include <string>
#include <vector>

#include "util/singleton.h"

class Package : public Util::Singleton<Package> {
public:
	Package(s);
	virtual ~Package();

	enum class Distro {
		Unsupported,
		Arch,
		Debian,
	};

	enum class InstallType {
		Install,
		AurInstall,
	};

	void aurInstall();
	void install();
	void list(const std::vector<std::string>& targets = {});
	void store();

private:
	std::optional<std::string> fetchAurHelper();
	void installOrAurInstall(InstallType type);

	bool findDependency(const std::string& search);
	bool distroDetect();
	bool distroDependencies();
	std::optional<std::string> getPackageList();

	Distro m_distro { Distro::Unsupported };
};

#endif // PACKAGE_H
