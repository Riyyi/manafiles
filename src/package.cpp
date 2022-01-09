/*
 * Copyright (C) 2021-2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <array>
#include <cstdio>  // fprintf, printf, stderr
#include <sstream> // istringstream
#include <string>  // getline
#include <vector>

#include "machine.h"
#include "package.h"
#include "util/file.h"
#include "util/system.h"

Package::Package()
{
}

Package::~Package()
{
}

// -----------------------------------------

void Package::aurInstall()
{
}

void Package::install()
{
}

void Package::list(const std::vector<std::string>& targets)
{
	std::string packages = getPackageList();

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
			if (line.find(target) != std::string::npos) {
				packages.append(line + '\n');
				break;
			}
		}
	}

	printf("%s", packages.c_str());
}

void Package::store()
{
	std::string packages = getPackageList();

	auto packageFile = Util::File("./packages");
	packageFile.clear();
	packageFile.append(packages);
	packageFile.flush();
}

// -----------------------------------------

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

	// FIXME: Conglomerate which calls to save multiple external process creation.
	Util::System $;
	for (const auto& dependency : dependencies) {
		if ($("which " + dependency.at(0))().status() > 0) {
			fprintf(stderr, "\033[31;1mPackage:\033[0m required dependency '%s' is missing\n", dependency.at(1).c_str());
			return false;
		}
	}

	return true;
}

std::string Package::getPackageList()
{
	distroDetect();
	distroDependencies();

	std::string packages;

	Util::System $;
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
