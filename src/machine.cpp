/*
 * Copyright (C) 2022,2025 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdint>    // int8_t
#include <filesystem> // std::filesystem::path
#include <pwd.h>      // getpwnam
#include <sstream>    // istringstream
#include <unistd.h>   // gethostname, getlogin

#include "ruc/file.h"

#include "machine.h"

Machine::Machine(s)
{
	fetchDistro();
	fetchHostname();
	fetchUsername();
	fetchSession();
}

Machine::~Machine()
{
}

// -----------------------------------------

void Machine::fetchDistro()
{
	ruc::File osRelease("/etc/os-release");
	std::istringstream stream(osRelease.data());
	for (std::string line; std::getline(stream, line);) {
		if (line.find("ID=") == 0) {
			m_distroId = line.substr(3);
		}
		if (line.find("ID_LIKE=") == 0) {
			m_distroIdLike = line.substr(8);
		}
	}
}

void Machine::fetchHostname()
{
	char hostname[64] { 0 };
	if (gethostname(hostname, 64) < 0) {
		perror("\033[31;1mError:\033[0m gethostname");
	}
	m_hostname = hostname;
}

void Machine::fetchUsername()
{
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

void Machine::fetchSession()
{
	// Determine if this is an Xorg or Wayland session
	int8_t likelyWayland = 0;

	// Detect via environment variable
	const char* env;
	env = std::getenv("XDG_SESSION_TYPE");
	if (env != nullptr) {
		auto session = std::string(env);
		if (session == "wayland") {
			likelyWayland++;
		}
		else if (session == "x11") {
			likelyWayland--;
		}
	}
	env = std::getenv("WAYLAND_DISPLAY");
	if (env != nullptr) {
		auto display = std::string(env);
		if (display.find("wayland-", 0) == 0) {
			likelyWayland++;
		}
	}

	// Detect via Wayland socket
	auto socket = std::filesystem::path("/run/user") / std::to_string(uid());
	if (std::filesystem::exists(socket) && std::filesystem::is_directory(socket)) {
		for (const auto& entry : std::filesystem::directory_iterator(socket)) {
			if (entry.path().filename().string().find("wayland-", 0) == 0) {
				likelyWayland++;
				break;
			}
		}
	}

	// Detect via Xorg socket
	if (std::filesystem::exists("/tmp/.X11-unix")) {
		likelyWayland--;
	}

	// Detect via running processes, /proc/<id>/comm
	std::filesystem::path processes = "/proc";
	if (std::filesystem::exists(processes) && std::filesystem::is_directory(processes)) {
		for (const auto& entry : std::filesystem::directory_iterator(processes)) {
			if (std::filesystem::is_directory(entry)) {

				std::filesystem::path comm = entry.path() / "comm";
				if (!std::filesystem::exists(comm)) {
					continue;
				}

				// Read the contents of the "comm" file
				std::ifstream stream(comm);
				std::string command;
				std::getline(stream, command);

				if (command == "Xwayland" || command == "sway" || command == "hyprland") {
					likelyWayland++;
					break;
				}

				if (command == "Xorg" || command == "xinit" || command == "i3" || command == "bspwm") {
					likelyWayland--;
					break;
				}
			}
		}
	}

	// If we detected at least 2 ways, we can be fairly certain
	m_session = "";
	if (likelyWayland <= -2) {
		m_session = "xorg";
	}
	else if (likelyWayland >= 2) {
		m_session = "wayland";
	}
}
