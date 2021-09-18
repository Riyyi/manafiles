#include <functional> // function
#include <string>
#include <vector>

#include "macro.h"
#include "testcase.h"
#include "testsuite.h"
#include "util/system.h"

bool runSystem(std::function<Util::System(Util::System&)> commands, std::string* output = nullptr, std::string* error = nullptr)
{
	Util::System system;
	auto exec = commands(system);
	if (output) {
		*output = exec.output();
	}
	if (error) {
		*error = exec.error();
	}

	return !exec.status() ? true : false;
}

// -----------------------------------------

TEST_CASE(SystemCommand)
{
	// Regular echo command, return true
	std::string output = "";
	std::string error = "";
	auto result = runSystem([&](auto& $) {
		return $("echo Hello World!")();
	}, &output, &error);
	EXPECT_EQ(result, true);
	EXPECT_EQ(output, "Hello World!\n");
	EXPECT_EQ(error, "");

	// Apend output of two echo commands, return true
	output = "";
	error = "";
	result = runSystem([&](auto& $) {
		return $("echo -n Hello ") + $("echo -n World!");
	}, &output, &error);
	EXPECT_EQ(result, true);
	EXPECT_EQ(output, "Hello World!");
	EXPECT_EQ(error, "");

	// Pipe test, grep section of the echo command, return true
	output = "";
	error = "";
	result = runSystem([&](auto& $) {
		return $("echo Hello World!") | $("grep --only-matching Hello");
	}, &output, &error);
	EXPECT_EQ(result, true);
	EXPECT_EQ(output, "Hello\n");
	EXPECT_EQ(error, "");

	// Run all commands until first false, return true
	output = "";
	error = "";
	result = runSystem([&](auto& $) {
		return $("echo Hello") && $("echo World!");
	}, &output, &error);
	EXPECT_EQ(result, true);
	EXPECT_EQ(output, "Hello\nWorld!\n");
	EXPECT_EQ(error, "");

	// Run all commands until first true, return true
	output = "";
	error = "";
	result = runSystem([&](auto& $) {
		return $("echo Hello") || $("echo World!");
	}, &output, &error);
	EXPECT_EQ(result, true);
	EXPECT_EQ(output, "Hello\n");
	EXPECT_EQ(error, "");

	// And plus pipe test, grep the middle word, return true
	output = "";
	error = "";
	result = runSystem([&](auto& $) {
		return ($("echo -n one ") && $("echo -n two ") && $("echo -n three")) | $("grep --only-matching two");
	}, &output, &error);
	EXPECT_EQ(result, true);
	EXPECT_EQ(output, "two\n");
	EXPECT_EQ(error, "");

	// FIXME waitpid does not seem to get the right exit status here
	// Return false
	output = "";
	error = "";
	result = runSystem([&](auto& $) {
		return $("exit 1")();
	}, &output, &error);
	EXPECT_EQ(result, false);
	EXPECT_EQ(output, "");
	EXPECT_EQ(error, "");
}
