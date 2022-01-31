#include <string>

#include "macro.h"
#include "testcase.h"
#include "testsuite.h"
#include "util/shell.h"

bool runShell(const char* command, std::string* output = nullptr)
{
	stdout = Test::TestSuite::the().outputNull();

	Util::Shell $;
	auto exec = $(command);
	if (output) {
		*output = exec.output();
	}

	stdout = Test::TestSuite::the().outputStd();
	return !exec.status() ? true : false;
}

// -----------------------------------------

TEST_CASE(ShellCommand)
{
	// Pipe test, grep section of the echo command, return true
	std::string output = "";
	auto result = runShell("echo 'Hello World!' | grep --only-matching 'Hello'", &output);
	EXPECT_EQ(result, true);
	EXPECT_EQ(output, "Hello\n");

	// Return false
	result = runShell("exit 1");
	EXPECT_EQ(result, false);

	// Return false
	result = runShell("failure() { return 1; }; failure");
	EXPECT_EQ(result, false);
}
