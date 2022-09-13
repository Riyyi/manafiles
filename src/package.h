/*
 * Copyright (C) 2021-2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint> // uint8_t
#include <optional>
#include <string>
#include <vector>

#include "ruc/singleton.h"

class Package : public ruc::Singleton<Package> {
public:
	Package(s);
	virtual ~Package();

	enum class Distro : uint8_t {
		Unsupported,
		Arch,
		Debian,
	};

	enum class InstallType : uint8_t {
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
