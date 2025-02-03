/*
 * Copyright (C) 2021-2022,2025 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint> // uint8_t
#include <optional>
#include <string>
#include <vector>

#include "ruc/singleton.h"

#define PACKAGE_FILE "./packages"

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

	void aurInstall(const std::vector<std::string>& targets = {});
	void install(const std::vector<std::string>& targets = {});
	void list(const std::vector<std::string>& targets = {}, bool partialMatch = false);

private:
	std::optional<std::string> fetchAurHelper();
	void installOrAurInstall(InstallType type, const std::string& file);

	bool findDependency(const std::string& search);
	bool distroDetect();
	bool distroDependencies();
	std::optional<std::string> getPackageList();

	Distro m_distro { Distro::Unsupported };
};
