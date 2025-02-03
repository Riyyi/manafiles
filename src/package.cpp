/*
 * Copyright (C) 2021-2022,2025 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // replace
#include <array>
#include <cstdio>     // fprintf, printf, stderr
#include <cstdlib>    // system
#include <filesystem> // exists
#include <optional>
#include <sstream> // istringstream
#include <string>  // getline
#include <vector>

#include "ruc/shell.h"
#include "ruc/system.h"

#include "machine.h"
#include "package.h"

Package::Package(s)
{
}

Package::~Package()
{
}

// -----------------------------------------

void Package::aurInstall(const std::vector<std::string>& targets)
{
	if (targets.size() > 1) {
		fprintf(stderr, "\033[31;1mPackage:\033[0m only 1 file can be read packages from at a time\n");
		return;
	}

	installOrAurInstall(InstallType::AurInstall, targets.size() != 0 ? targets.front() : PACKAGE_FILE);
}

void Package::install(const std::vector<std::string>& targets)
{
	if (targets.size() > 1) {
		fprintf(stderr, "\033[31;1mPackage:\033[0m only 1 file can be read packages from at a time\n");
		return;
	}

	installOrAurInstall(InstallType::Install, targets.size() != 0 ? targets.front() : PACKAGE_FILE);
}

void Package::list(const std::vector<std::string>& targets, bool partialMatch)
{
	auto packagesOrEmpty = getPackageList();

	if (!packagesOrEmpty.has_value()) {
		return;
	}

	std::string packages = packagesOrEmpty.value();

	if (targets.empty()) {
		printf("%s", packages.c_str());
		return;
	}

	auto stream = std::istringstream(packages);
	packages.clear();

	// FIXME: Decide on the type of match, currently 'or, any part of the string'.
	std::string line;
	while (std::getline(stream, line)) {
		for (const auto& target : targets) {
			if (line == target || (partialMatch && line.find(target) != std::string::npos)) {
				packages.append(line + '\n');
				break;
			}
		}
	}

	printf("%s", packages.c_str());
}

// -----------------------------------------

std::optional<std::string> Package::fetchAurHelper()
{
	const std::string helpers[] = {
		"yay",
		"paru",
		"trizen",
	};

	for (const auto& helper : helpers) {
		if (findDependency(helper)) {
			return { helper };
		}
	}

	return {};
}

void Package::installOrAurInstall(InstallType type, const std::string& file)
{
	distroDetect();
	distroDependencies();

	std::optional<std::string> aurHelper;
	if (type == InstallType::AurInstall) {
		if (m_distro != Distro::Arch) {
			fprintf(stderr, "\033[31;1mPackage:\033[0m AUR is not supported on this distribution\n");
			return;
		}

		aurHelper = fetchAurHelper();
		if (!aurHelper.has_value()) {
			fprintf(stderr, "\033[31;1mPackage:\033[0m no supported AUR helper found\n");
			return;
		}
	}

	std::string command = "";

	ruc::System $;
	if (m_distro == Distro::Arch) {
		// Grab everything off enabled official repositories that is in the list
		auto repoList = $("pacman -Ssq") | $("grep -xf " + file);

		if (type == InstallType::AurInstall) {
			// Determine which packages in the list are from the AUR
			// NOTE: ruc::System does not support commands with newlines
			auto aurList = ruc::Shell()("grep -vx '" + repoList.output() + "' " + file);
			command = aurHelper.value() + " -Sy --devel --needed --noconfirm " + aurList.output();
		}
		else {
			command = "pacman -Sy --needed " + repoList.output();
		}
	}
	else if (m_distro == Distro::Debian) {
		// Grab everything off enabled official repositories that is in the list
		auto repoList = $("apt-cache search .").cut(1, ' ') | $("grep -xf " + file);
		command = "apt install " + repoList.output();
	}

	std::replace(command.begin(), command.end(), '\n', ' ');

#ifndef NDEBUG
	printf("running: $ %s\n", command.c_str());
#endif
	system(command.c_str());
}

bool Package::findDependency(const std::string& search)
{
	return std::filesystem::exists("/bin/" + search)
	       || std::filesystem::exists("/usr/bin/" + search)
	       || std::filesystem::exists("/usr/local/bin/" + search);
}

bool Package::distroDetect()
{
	std::string id = Machine::the().distroId();
	std::string idLike = Machine::the().distroIdLike();

	if (id == "arch") {
		m_distro = Distro::Arch;
	}
	else if (id == "debian") {
		m_distro = Distro::Debian;
	}
	else if (id == "ubuntu") {
		m_distro = Distro::Debian;
	}
	else if (idLike.find("arch") != std::string::npos) {
		m_distro = Distro::Arch;
	}
	else if (idLike.find("debian") != std::string::npos) {
		m_distro = Distro::Debian;
	}
	else if (idLike.find("ubuntu") != std::string::npos) {
		m_distro = Distro::Debian;
	}
	else {
		fprintf(stderr, "\033[31;1mPackage:\033[0m unsupported distribution\n");
		return false;
	}

	return true;
}

bool Package::distroDependencies()
{
	std::vector<std::array<std::string, 2>> dependencies;
	if (m_distro == Distro::Arch) {
		dependencies.push_back({ "pacman", "pacman" });
		dependencies.push_back({ "pactree", "pacman-contrib" });
	}
	else if (m_distro == Distro::Debian) {
		dependencies.push_back({ "apt-cache", "apt" });
		dependencies.push_back({ "apt-mark", "apt" });
		dependencies.push_back({ "dpkg-query", "dpkg" });
	}

	for (const auto& dependency : dependencies) {
		if (!findDependency(dependency.at(0))) {
			fprintf(stderr, "\033[31;1mPackage:\033[0m required dependency '%s' is missing\n", dependency.at(1).c_str());
			return false;
		}
	}

	return true;
}

std::optional<std::string> Package::getPackageList()
{
	if (!distroDetect() || !distroDependencies()) {
		return {};
	}

	std::string packages;

	ruc::System $;
	if (m_distro == Distro::Arch) {
		auto basePackages = $("pactree -u base").tail(2, true);
		auto develPackages = $("pacman -Qqg base-devel");
		auto filterList = (basePackages + develPackages).sort(true);
		auto packageList = ($("pacman -Qqe") | $("grep -xv " + filterList.output())).sort();
		packages = packageList.output();
	}
	else if (m_distro == Distro::Debian) {
		auto installedList = $("dpkg-query --show --showformat=${Package}\\t${Priority}\\n");
		auto filterList = (installedList | $("grep -E required|important|standard")).cut(1);
		installedList = installedList.cut(1);
		auto installedManuallyList = $("awk '/Commandline:.* install / && !/APT::/ { print $NF }' /var/log/apt/history.log");
		installedManuallyList = (installedManuallyList + $("apt-mark showmanual")).sort(true);
		auto packageList = installedManuallyList | $("grep -x " + installedList.output()) | $("grep -xv " + filterList.output());
		packages = packageList.output();
	}

	return packages;
}
