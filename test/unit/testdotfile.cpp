/*
 * Copyright (C) 2022,2025 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstddef>    // size_t
#include <cstdio>     // stderr
#include <filesystem> // path
#include <string>
#include <unistd.h> // geteuid, setegid, seteuid
#include <unordered_map>
#include <vector>

#include "ruc/file.h"

#include "config.h"
#include "dotfile.h"
#include "machine.h"
#include "macro.h"
#include "testcase.h"
#include "testsuite.h"

const bool root = !geteuid() ? true : false;
const std::filesystem::path homeDirectory = "/home/" + Machine::the().username();
const size_t homeDirectorySize = homeDirectory.string().size();

void createTestDotfiles(const std::vector<std::string>& fileNames, const std::vector<std::string>& fileContents, bool asRoot = false)
{
	EXPECT(fileNames.size() == fileContents.size(), return);

	if (root && !asRoot) {
		setegid(Machine::the().gid());
		seteuid(Machine::the().uid());
	}

	for (size_t i = 0; i < fileNames.size(); ++i) {
		auto fileName = fileNames.at(i);

		auto directory = std::filesystem::path { fileName }.parent_path();
		if (!directory.empty() && !std::filesystem::exists(directory)) {
			std::filesystem::create_directories(directory);
		}

		auto file = ruc::File::create(fileName);
		if (file.data().size() == 0) {
			file.append(fileContents.at(i).c_str()).flush();
		}
	}

	if (root && !asRoot) {
		seteuid(0);
		setegid(0);
	}
}

void removeTestDotfiles(const std::vector<std::string>& files, bool deleteInHome = true)
{
	for (auto file : files) {
		// Get the top-level directory
		if (file.find(homeDirectory) == 0) {
			file = file.substr(homeDirectorySize + 1);
		}
		file = file.substr(0, file.find_first_of('/'));

		// Delete recursively in working directory
		if (std::filesystem::exists(file) || std::filesystem::is_symlink(file)) {
			std::filesystem::remove_all(file);
		}

		if (!deleteInHome) {
			continue;
		}

		// Delete recursively in home directory
		file = homeDirectory / file;
		if (std::filesystem::exists(file) || std::filesystem::is_symlink(file)) {
			std::filesystem::remove_all(file);
		}
	}
}

void testDotfileFilters(const std::unordered_map<std::string, bool>& tests,
                        const std::vector<std::string>& testIgnorePatterns)
{
	std::vector<std::string> fileNames;
	std::vector<std::string> fileContents;
	for (const auto& test : tests) {
		fileNames.push_back(test.first);
		fileContents.push_back("");
	}

	createTestDotfiles(fileNames, fileContents);

	for (const auto& path : fileNames) {
		bool result = Dotfile::the().match("/" + path, testIgnorePatterns);
		EXPECT_EQ(result, tests.at(path), printf("        path = '%s'\n", path.c_str()));
	}

	removeTestDotfiles(fileNames, false);
}

// -----------------------------------------

TEST_CASE(DotfilesLiteralIgnoreAllFiles)
{
	std::unordered_map<std::string, bool> tests = {
		{ "access.log", true },
		{ "logs/access.log", true },
		{ "var/logs/access.log", true },
		{ "error.log", false },
	};

	std::vector<std::string> testFilters = {
		"access.log",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesLiteralIgnoreFileInRoot)
{
	std::unordered_map<std::string, bool> tests = {
		{ "access.log", true },
		{ "logs/access.log", false },
		{ "var/logs/access.log", false },
		{ "error.log", false },
	};

	std::vector<std::string> testFilters = {
		"/access.log",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesLiteralIgnoreDirectories)
{
	std::unordered_map<std::string, bool> tests = {
		{ "build/executable", true },
		{ "doc/build", false },
	};

	std::vector<std::string> testFilters = {
		"build/",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreAllFilesWithExtension)
{
	std::unordered_map<std::string, bool> tests = {
		{ "error.log", true },
		{ "logs/debug.log", true },
		{ "var/logs/error.log", true },
		{ ".log/output.txt", true },
		{ "program.log/output.txt", true },
		{ "log.txt", false },
	};

	std::vector<std::string> testFilters = {
		"*.log",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreAllFilesInRootWithExtension)
{
	std::unordered_map<std::string, bool> tests = {
		{ "error.log", true },
		{ "logs/debug.log", false },
		{ "var/logs/error.log", false },
		{ ".log/output.txt", true },
		{ "program.log/output.txt", true },
		{ "log.txt", false },
	};

	std::vector<std::string> testFilters = {
		"/*.log",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreFileWithAllExtensions)
{
	std::unordered_map<std::string, bool> tests = {
		{ "README.md", true },
		{ "doc/README.org", true },
		{ "doc/compressed/README.tar.gz", true },
		{ "doc/compressed/BIG_README.tar.gz", false }, // FIXME
		{ "Documentation.README", false },
		{ "Config.org", false },
	};

	std::vector<std::string> testFilters = {
		"README.*",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreAllWithStartingPattern)
{
	std::unordered_map<std::string, bool> tests = {
		{ "cmake/uninstall.cmake.in", true },
		{ "cmake/templates/template.cmake.in", true },
		{ "build/cmake/executable", true },
		{ "build/cmakefiles/makefile", true },
		{ "CMakeLists.txt", false },
	};

	std::vector<std::string> testFilters = {
		"cmake*",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreAllInRootWithStartingPattern)
{
	std::unordered_map<std::string, bool> tests = {
		{ "project-directory/README.org", true },
		{ "project-directory/build/executable", true },
		{ "project-file", true },
		{ "doc/project-instructions.txt", false },
	};

	std::vector<std::string> testFilters = {
		"/project*",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreAllInDirectory)
{
	std::unordered_map<std::string, bool> tests = {
		{ "build/x32/executable", true },
		{ "build/x64/executable", true },
		{ "build/executable", true },
	};

	std::vector<std::string> testFilters = {
		"build/*",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreFileInSubDirectory)
{
	std::unordered_map<std::string, bool> tests = {
		{ "build/x32/executable", true },
		{ "build/x64/executable", true },
		{ "build/x64/config.json", false },
		{ "project/build/x64/config.json", false },
		{ "build/executable", false },
	};

	std::vector<std::string> testFilters = {
		"build/*/executable",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreAllInSubDirectory)
{
	std::unordered_map<std::string, bool> tests = {
		{ "build/x32/executable", true },
		{ "build/x64/executable", true },
		{ "build/x64/config.json", true },
		{ "project/build/x64/config.json", true },
		{ "build/executable", false },
	};

	std::vector<std::string> testFilters = {
		"build/*/*",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreAllDirectoriesWithStartingPattern)
{
	std::unordered_map<std::string, bool> tests = {
		{ "include/header.h", true },
		{ "include-dependency/header.h", true },
		{ "include-file", false },
		{ "src/include/header.h", true },
	};

	std::vector<std::string> testFilters = {
		"include*/",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreAllDirectoriesInRootWithStartingPattern)
{
	std::unordered_map<std::string, bool> tests = {
		{ "include/header.h", true },
		{ "include-dependency/header.h", true },
		{ "include-file", false },
		{ "src/include/header.h", false },
	};

	std::vector<std::string> testFilters = {
		"/include*/",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreAllDirectoriesWithEndingPattern)
{
	std::unordered_map<std::string, bool> tests = {
		{ "include/header.h", true },
		{ "dependency-include/header.h", true },
		{ "file-include", false },
		{ "src/include/header.h", true },
	};

	std::vector<std::string> testFilters = {
		"*include/",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(DotfilesWildcardIgnoreAllDirectoriesInRootWithEndingPattern)
{
	std::unordered_map<std::string, bool> tests = {
		{ "include/header.h", true },
		{ "dependency-include/header.h", true },
		{ "file-include", false },
		{ "src/include/header.h", false },
	};

	std::vector<std::string> testFilters = {
		"/*include/",
	};

	testDotfileFilters(tests, testFilters);
}

TEST_CASE(AddDotfiles)
{
	std::vector<std::string> fileNames = {
		homeDirectory / "__test-file-1",
		homeDirectory / "__subdir/__test-file-2",
		homeDirectory / "__subdir/__another-subdir/__test-file-3",
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
		EXPECT(std::filesystem::exists(file), continue);
		EXPECT(std::filesystem::exists(file.substr(homeDirectorySize + 1)), continue);

		ruc::File lhs(file);
		ruc::File rhs(file.substr(homeDirectorySize + 1));
		EXPECT_EQ(lhs.data(), rhs.data());
	}

	removeTestDotfiles(fileNames);
}

TEST_CASE(AddNonExistentDotfiles)
{
	stderr = test::TestSuite::the().outputNull();
	Dotfile::the().add({ homeDirectory / "__non-existent-test-file" });
	stderr = test::TestSuite::the().outputErr();

	EXPECT(!std::filesystem::exists("__non-existent-test-file"));
	removeTestDotfiles({ "__non-existent-test-file" });
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
		homeFileNames.push_back(homeDirectory / file);
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

	Dotfile::the().pull(fileNames);

	for (size_t i = 0; i < fileNames.size(); ++i) {
		EXPECT(std::filesystem::exists(homeFileNames.at(i)), continue);
		EXPECT(std::filesystem::exists(fileNames.at(i)), continue);

		ruc::File lhs(homeFileNames.at(i));
		ruc::File rhs(fileNames.at(i));
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
		EXPECT(std::filesystem::exists(file), continue);
		EXPECT(std::filesystem::exists(homeDirectory / file), continue);

		ruc::File lhs(file);
		ruc::File rhs((homeDirectory / file).string());
		EXPECT_EQ(lhs.data(), rhs.data());
	}

	removeTestDotfiles(fileNames);
}

TEST_CASE(PushDotfilesWithIgnorePattern)
{
	std::vector<std::string> fileNames = {
		"__test-file-1",
		"__subdir/__test-file-2",
		"__subdir/__test-file-3",
		"__another-subdir/__test-file-4.test",
	};

	createTestDotfiles(fileNames, { "", "", "", "" });

	auto ignorePatterns = Config::the().ignorePatterns();
	Config::the().setIgnorePatterns({
		"__test-file-1",
		"__subdir/",
		"*.test",
	});

	Dotfile::the().push(fileNames);

	Config::the().setIgnorePatterns(ignorePatterns);

	for (const auto& file : fileNames) {
		EXPECT(!std::filesystem::exists(homeDirectory / file));
	}

	removeTestDotfiles(fileNames);
}

TEST_CASE(PushDotfilesSelectivelyComment)
{
	std::vector<std::string> fileNames;
	for (size_t i = 0; i < 44; ++i) {
		fileNames.push_back("__test-file-" + std::to_string(i + 1));
	}

	auto distro = Machine::the().distroId();
	auto hostname = Machine::the().hostname();
	auto username = Machine::the().username();
	std::string placeholder = "@@@@";

	std::vector<std::string> fileContents = {
		// Untouched #
		"# >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
test data # untouched
# <<<
)",
		"  # >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
  test data # untouched
  # <<<
)",
		"	# >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	test data # untouched
	# <<<
)",
		"	  # >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	  test data # untouched
	  # <<<
)",

		// Comment #
		"# >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + R"(
test data # comment
# <<<
)",
		"  # >>> distro=" + distro + " hostname=" + placeholder + " user=" + username + R"(
  test data # comment
  # <<<
)",
		"	# >>> distro=" + distro + " hostname=" + hostname + " user=" + placeholder + R"(
	test data # comment
	# <<<
)",
		"	  # >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + R"(
	  test data # comment
	  # <<<
)",

		// Uncomment #
		"# >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
# test data # uncomment
# <<<
)",
		"  # >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
  #  test data # uncomment
  # <<<
)",
		"	# >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	#	test data # uncomment
	# <<<
)",
		"	  # >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	  #	  test data # uncomment
	  # <<<
)",

		// -----------------------------------------

		// Untouched //
		"// >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
test data // untouched
// <<<
)",
		"  // >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
  test data // untouched
  // <<<
)",
		"	// >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	test data // untouched
	// <<<
)",
		"	  // >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	  test data // untouched
	  // <<<
)",

		// Comment //
		"// >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + R"(
test data // comment
// <<<
)",
		"  // >>> distro=" + distro + " hostname=" + placeholder + " user=" + username + R"(
  test data // comment
  // <<<
)",
		"	// >>> distro=" + distro + " hostname=" + hostname + " user=" + placeholder + R"(
	test data // comment
	// <<<
)",
		"	  // >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + R"(
	  test data // comment
	  // <<<
)",

		// Uncomment //
		"// >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
// test data // uncomment
// <<<
)",
		"  // >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
  //  test data // uncomment
  // <<<
)",
		"	// >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	//	test data // uncomment
	// <<<
)",
		"	  // >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	  //	  test data // uncomment
	  // <<<
)",

		// -----------------------------------------

		// Untouched /**/
		"/* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
test data /**/ untouched
/* <<< */
)",
		"  /* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
  test data /**/ untouched
  /* <<< */
)",
		"	/* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
	test data /**/ untouched
	/* <<< */
)",
		"	  /* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
	  test data /**/ untouched
	  /* <<< */
)",

		// Comment /**/
		"/* >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + " */" + R"(
test data /**/ comment
/* <<< */
)",
		"  /* >>> distro=" + distro + " hostname=" + placeholder + " user=" + username + " */" + R"(
  test data /**/ comment
  /* <<< */
)",
		"	/* >>> distro=" + distro + " hostname=" + hostname + " user=" + placeholder + " */" + R"(
	test data /**/ comment
	/* <<< */
)",
		"	  /* >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + " */" + R"(
	  test data /**/ comment
	  /* <<< */
)",

		// Uncomment /**/
		"/* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
/* test data /**/ uncomment */
/* <<< */
)",
		"  /* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
  /*  test data /**/ uncomment  */
  /* <<< */
)",
		"	/* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
	/*	test data /**/ uncomment	*/
	/* <<< */
)",
		"	  /* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
	  /*	  test data /**/ uncomment	  */
	  /* <<< */
)",

		// Comment <!---->
		"<!-- >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + " -->" + R"(
test data <!----> comment
<!-- <<< -->
)",
		"  <!-- >>> distro=" + distro + " hostname=" + placeholder + " user=" + username + " -->" + R"(
  test data <!----> comment
  <!-- <<< -->
)",
		"	<!-- >>> distro=" + distro + " hostname=" + hostname + " user=" + placeholder + " -->" + R"(
	test data <!----> comment
	<!-- <<< -->
)",
		"	  <!-- >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + " -->" + R"(
	  test data <!----> comment
	  <!-- <<< -->
)",

		// Uncomment <!---->
		"<!-- >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " -->" + R"(
<!-- test data <!----> uncomment -->
<!-- <<< -->
)",
		"  <!-- >>> distro="
			+ distro + " hostname=" + hostname + " user=" + username + " -->" + R"(
  <!--  test data <!----> uncomment  -->
  <!-- <<< -->
)",
		"	<!-- >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " -->" + R"(
	<!--	test data <!----> uncomment	-->
	<!-- <<< -->
)",
		"	  <!-- >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " -->" + R"(
	  <!--	  test data <!----> uncomment	  -->
	  <!-- <<< -->
)",
	};

	std::vector<std::string> pushedFileContents = {
		// Untouched #
		"# >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
test data # untouched
# <<<
)",
		"  # >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
  test data # untouched
  # <<<
)",
		"	# >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	test data # untouched
	# <<<
)",
		"	  # >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	  test data # untouched
	  # <<<
)",

		// Comment #
		"# >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + R"(
# test data # comment
# <<<
)",
		"  # >>> distro=" + distro + " hostname=" + placeholder + " user=" + username + R"(
  # test data # comment
  # <<<
)",
		"	# >>> distro=" + distro + " hostname=" + hostname + " user=" + placeholder + R"(
	# test data # comment
	# <<<
)",
		"	  # >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + R"(
	  # test data # comment
	  # <<<
)",

		// Uncomment #
		"# >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
test data # uncomment
# <<<
)",
		"  # >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
  test data # uncomment
  # <<<
)",
		"	# >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	test data # uncomment
	# <<<
)",
		"	  # >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	  test data # uncomment
	  # <<<
)",

		// -----------------------------------------

		// Untouched //
		"// >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
test data // untouched
// <<<
)",
		"  // >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
  test data // untouched
  // <<<
)",
		"	// >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	test data // untouched
	// <<<
)",
		"	  // >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	  test data // untouched
	  // <<<
)",

		// Comment //
		"// >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + R"(
// test data // comment
// <<<
)",
		"  // >>> distro=" + distro + " hostname=" + placeholder + " user=" + username + R"(
  // test data // comment
  // <<<
)",
		"	// >>> distro=" + distro + " hostname=" + hostname + " user=" + placeholder + R"(
	// test data // comment
	// <<<
)",
		"	  // >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + R"(
	  // test data // comment
	  // <<<
)",

		// Uncomment //
		"// >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
test data // uncomment
// <<<
)",
		"  // >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
  test data // uncomment
  // <<<
)",
		"	// >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	test data // uncomment
	// <<<
)",
		"	  // >>> distro=" + distro + " hostname=" + hostname + " user=" + username + R"(
	  test data // uncomment
	  // <<<
)",

		// -----------------------------------------

		// Untouched /**/
		"/* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
test data /**/ untouched
/* <<< */
)",
		"  /* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
  test data /**/ untouched
  /* <<< */
)",
		"	/* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
	test data /**/ untouched
	/* <<< */
)",
		"	  /* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
	  test data /**/ untouched
	  /* <<< */
)",

		// Comment /**/
		"/* >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + " */" + R"(
/* test data /**/ comment */
/* <<< */
)",
		"  /* >>> distro=" + distro + " hostname=" + placeholder + " user=" + username + " */" + R"(
  /* test data /**/ comment */
  /* <<< */
)",
		"	/* >>> distro=" + distro + " hostname=" + hostname + " user=" + placeholder + " */" + R"(
	/* test data /**/ comment */
	/* <<< */
)",
		"	  /* >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + " */" + R"(
	  /* test data /**/ comment */
	  /* <<< */
)",

		// Uncomment /**/
		"/* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
test data /**/ uncomment
/* <<< */
)",
		"  /* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
  test data /**/ uncomment
  /* <<< */
)",
		"	/* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
	test data /**/ uncomment
	/* <<< */
)",
		"	  /* >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " */" + R"(
	  test data /**/ uncomment
	  /* <<< */
)",

		// Comment <!---->
		"<!-- >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + " -->" + R"(
<!-- test data <!----> comment -->
<!-- <<< -->
)",
		"  <!-- >>> distro=" + distro + " hostname=" + placeholder + " user=" + username + " -->" + R"(
  <!-- test data <!----> comment -->
  <!-- <<< -->
)",
		"	<!-- >>> distro=" + distro + " hostname=" + hostname + " user=" + placeholder + " -->" + R"(
	<!-- test data <!----> comment -->
	<!-- <<< -->
)",
		"	  <!-- >>> distro=" + placeholder + " hostname=" + hostname + " user=" + username + " -->" + R"(
	  <!-- test data <!----> comment -->
	  <!-- <<< -->
)",

		// Uncomment <!---->
		"<!-- >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " -->" + R"(
test data <!----> uncomment
<!-- <<< -->
)",
		"  <!-- >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " -->" + R"(
  test data <!----> uncomment
  <!-- <<< -->
)",
		"	<!-- >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " -->" + R"(
	test data <!----> uncomment
	<!-- <<< -->
)",
		"	  <!-- >>> distro=" + distro + " hostname=" + hostname + " user=" + username + " -->" + R"(
	  test data <!----> uncomment
	  <!-- <<< -->
)",
	};

	createTestDotfiles(fileNames, fileContents);

	Dotfile::the().push(fileNames);

	for (size_t i = 0; i < fileNames.size(); ++i) {
		const auto& file = fileNames.at(i);
		EXPECT(std::filesystem::exists(file), continue);
		EXPECT(std::filesystem::exists(homeDirectory / file), continue);

		ruc::File lhs((homeDirectory / file).string());
		EXPECT_EQ(lhs.data(), pushedFileContents.at(i));
	}

	removeTestDotfiles(fileNames);
}

TEST_CASE(AddSystemDotfiles)
{
	EXPECT(geteuid() == 0, return);

	Config::the().setSystemPatterns({ "/etc/", "/usr/lib/" });
	Dotfile::the().add({ "/etc/group", "/usr/lib/os-release" });
	Config::the().setSystemPatterns({});

	EXPECT(std::filesystem::exists("etc/group"));
	EXPECT(std::filesystem::exists("usr/lib/os-release"));

	std::filesystem::remove_all(Config::the().workingDirectory() / "etc");
	std::filesystem::remove_all(Config::the().workingDirectory() / "usr");
}

TEST_CASE(PullSystemDotfiles)
{
	EXPECT(geteuid() == 0, return);

	createTestDotfiles({ "etc/group" }, { "" }, true);

	Config::the().setSystemPatterns({ "/etc/" });
	Dotfile::the().pull({ "etc/group" });
	Config::the().setSystemPatterns({});

	ruc::File lhs("/etc/group");
	ruc::File rhs((Config::the().workingDirectory() / "etc/group").string());
	EXPECT_EQ(lhs.data(), rhs.data());

	std::filesystem::remove_all(Config::the().workingDirectory() / "etc");
}

TEST_CASE(AddSymlinkDotfiles)
{
	std::filesystem::path fileInHome = homeDirectory / "__the-add-file";
	std::filesystem::path symlinkFileName = "__the-add-symlink";
	std::filesystem::path symlinkInHome = homeDirectory / symlinkFileName;

	createTestDotfiles({ fileInHome }, { "the file contents" });
	std::filesystem::create_symlink(fileInHome, symlinkInHome);

	Dotfile::the().add({ symlinkInHome });

	EXPECT(std::filesystem::is_symlink(symlinkFileName));
	EXPECT_EQ(std::filesystem::read_symlink(symlinkFileName).string(), fileInHome);
	EXPECT_EQ(ruc::File(symlinkFileName.string()).data(), "the file contents");

	removeTestDotfiles({ symlinkInHome, fileInHome });
}

TEST_CASE(PullSymlinkDotfiles)
{
	std::filesystem::path fileInHome = homeDirectory / "__the-pull-file";
	std::filesystem::path symlinkFileName = "__the-pull-symlink";
	std::filesystem::path symlinkInHome = homeDirectory / symlinkFileName;

	createTestDotfiles({ fileInHome }, { "the file contents" });
	std::filesystem::create_symlink(fileInHome, symlinkInHome);
	std::filesystem::create_symlink("doesnt-exist", symlinkFileName);

	Dotfile::the().pull({ symlinkFileName });

	EXPECT(std::filesystem::is_symlink(symlinkFileName));
	EXPECT_EQ(std::filesystem::read_symlink(symlinkFileName).string(), fileInHome);
	EXPECT_EQ(ruc::File(symlinkFileName.string()).data(), "the file contents");

	removeTestDotfiles({ symlinkInHome, fileInHome });
}

TEST_CASE(PushSymlinkDotfiles)
{
	std::filesystem::path fileInHome = homeDirectory / "__the-push-file";
	std::filesystem::path symlinkFileName = "__the-push-symlink";
	std::filesystem::path symlinkInHome = homeDirectory / symlinkFileName;

	createTestDotfiles({ fileInHome }, { "the file contents" });
	std::filesystem::create_symlink(fileInHome, symlinkFileName);

	Dotfile::the().push({ symlinkFileName });

	EXPECT(std::filesystem::is_symlink(symlinkInHome));
	EXPECT_EQ(std::filesystem::read_symlink(symlinkInHome).string(), fileInHome);
	EXPECT_EQ(ruc::File(symlinkInHome.string()).data(), "the file contents");

	removeTestDotfiles({ symlinkInHome, fileInHome });
}
