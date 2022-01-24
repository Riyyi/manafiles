/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio> // stderr
#include <filesystem>
#include <string>
#include <unistd.h> // geteuid, setegid, seteuid
#include <vector>

#include "dotfile.h"
#include "machine.h"
#include "macro.h"
#include "testcase.h"
#include "testsuite.h"
#include "util/file.h"
#include "util/system.h"

const std::string homeDirectory = "/home/" + Machine::the().username();
const size_t homeDirectorySize = homeDirectory.size();

void createTestDotfiles(const std::vector<std::string>& fileNames, const std::vector<std::string>& fileContents)
{
	VERIFY(fileNames.size() == fileContents.size());

	bool root = !geteuid() ? true : false;
	if (root) {
		setegid(Machine::the().gid());
		seteuid(Machine::the().uid());
	}

	for (size_t i = 0; i < fileNames.size(); ++i) {
		auto fileName = fileNames.at(i);

		auto directory = std::filesystem::path { fileName }.parent_path();
		if (!directory.empty() && !std::filesystem::exists(directory)) {
			std::filesystem::create_directories(directory);
		}

		auto file = Util::File::create(fileName);
		if (file.data().size() == 0) {
			file.append(fileContents.at(i).c_str()).flush();
		}
	}

	if (root) {
		seteuid(0);
		setegid(0);
	}
}

void removeTestDotfiles(const std::vector<std::string>& files)
{
	for (auto file : files) {
		// Get the top-level directory
		if (file.find(homeDirectory) == 0) {
			file = file.substr(homeDirectorySize + 1);
		}
		file = file.substr(0, file.find_first_of('/'));

		// Delete recursively in working directory
		if (std::filesystem::exists(file)) {
			std::filesystem::remove_all(file);
		}

		// Delete recursively in home directory
		file = homeDirectory + '/' + file;
		if (std::filesystem::exists(file)) {
			std::filesystem::remove_all(file);
		}
	}
}

// -----------------------------------------

TEST_CASE(AddDotfiles)
{
	std::vector<std::string> fileNames = {
		homeDirectory + "/__test-file-1",
		homeDirectory + "/__subdir/__test-file-2",
		homeDirectory + "/__subdir/__another-subdir/__test-file-3",
	};

	std::vector<std::string> fileContents = {
		R"(home directory file 1
)",
		R"(home directory file 2
)",
		R"(home directory file 3
)",
	};

	createTestDotfiles(fileNames, fileContents);

	Dotfile::the().add(fileNames);

	for (const auto& file : fileNames) {
		EXPECT(std::filesystem::exists(file));
		EXPECT(std::filesystem::exists(file.substr(homeDirectorySize + 1)));

		Util::File lhs(file);
		Util::File rhs(file.substr(homeDirectorySize + 1));
		EXPECT_EQ(lhs.data(), rhs.data());
	}

	removeTestDotfiles(fileNames);
}

TEST_CASE(AddNonExistentDotfiles)
{
	stderr = Test::TestSuite::the().outputNull();
	Dotfile::the().add({ "/tmp/__non-existent-test-file" });
	stderr = Test::TestSuite::the().outputErr();

	EXPECT(!std::filesystem::exists("__non-existent-test-file"));
}

TEST_CASE(PullDotfiles)
{
	std::vector<std::string> fileNames = {
		"__test-file-1",
		"__subdir/__test-file-2",
		"__subdir/__another-subdir/__test-file-3",
	};
	std::vector<std::string> homeFileNames;
	for (const auto& file : fileNames) {
		homeFileNames.push_back(homeDirectory + '/' + file);
	}

	std::vector<std::string> homeFileContents = {
		R"(file 1 after pulling
)",
		R"(file 2 after pulling
)",
		R"(file 3 after pulling
)",
	};
	std::vector<std::string> workingDirectoryFileContents = {
		R"(file 1
)",
		R"(file 2
)",
		R"(file 3
)",
	};

	createTestDotfiles(homeFileNames, homeFileContents);
	createTestDotfiles(fileNames, workingDirectoryFileContents);

	Dotfile::the().setSystemDirectories({ "/etc" });
	Dotfile::the().pull(fileNames);

	for (size_t i = 0; i < fileNames.size(); ++i) {
		EXPECT(std::filesystem::exists(homeFileNames.at(i)));
		EXPECT(std::filesystem::exists(fileNames.at(i)));

		Util::File lhs(homeFileNames.at(i));
		Util::File rhs(fileNames.at(i));
		EXPECT_EQ(lhs.data(), rhs.data());
	}

	removeTestDotfiles(fileNames);
}

TEST_CASE(PushDotfiles)
{
	std::vector<std::string> fileNames = {
		"__test-file-1",
		"__subdir/__test-file-2",
		"__subdir/__another-subdir/__test-file-3",
	};

	std::vector<std::string> fileContents = {
		R"(working directory file 1
)",
		R"(working directory file 2
)",
		R"(working directory file 3
)",
	};

	createTestDotfiles(fileNames, fileContents);

	Dotfile::the().push(fileNames);

	for (const auto& file : fileNames) {
		EXPECT(std::filesystem::exists(file));
		EXPECT(std::filesystem::exists(homeDirectory + '/' + file));

		Util::File lhs(file);
		Util::File rhs(homeDirectory + '/' + file);
		EXPECT_EQ(lhs.data(), rhs.data());
	}

	removeTestDotfiles(fileNames);
}
