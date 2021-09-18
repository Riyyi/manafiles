#include <functional> // function
#include <string>
#include <vector>

#include "macro.h"
#include "testcase.h"
#include "testsuite.h"
#include "util/argparser.h"

bool runParser(std::vector<const char*> arguments, std::function<void(Util::ArgParser&)> initializer = {})
{
	stdout = Test::TestSuite::the().outputNull();

	Util::ArgParser parser;
	if (initializer) {
		initializer(parser);
	}

	arguments.insert(arguments.begin(), "app");
	auto result = parser.parse(arguments.size(), arguments.data());

	stdout = Test::TestSuite::the().outputStd();
	return result;
}

// -----------------------------------------

TEST_CASE(NoArguments)
{
	auto result = runParser({});
	EXPECT_EQ(result, true);
}

// -----------------------------------------

TEST_CASE(NonExistentArguments)
{
	auto result = runParser({ "-n", "-e" });
	EXPECT_EQ(result, false);

	result = runParser({ "--non", "--existent" });
	EXPECT_EQ(result, false);

	result = runParser({ "-n", "-e", "--non", "--existent" });
	EXPECT_EQ(result, false);

	result = runParser({ "no", "handling" });
	EXPECT_EQ(result, false);
}

// -----------------------------------------

TEST_CASE(RequiredStringArguments)
{
	// Single required string argument
	std::string stringArg1 = "";
	auto result = runParser({ "my-required-argument" }, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringArg1, "my-required-argument");

	// Single required string argument, not given
	stringArg1 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(stringArg1, "");

	// Multiple required string arguments
	stringArg1 = "";
	std::string stringArg2 = "";
	std::string stringArg3 = "";
	result = runParser({ "my", "required", "argument" }, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
		parser.addArgument(stringArg2, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
		parser.addArgument(stringArg3, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringArg1, "my");
	EXPECT_EQ(stringArg2, "required");
	EXPECT_EQ(stringArg3, "argument");

	// Multiple required string arguments, not given
	stringArg1 = "";
	stringArg2 = "";
	stringArg3 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
		parser.addArgument(stringArg2, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
		parser.addArgument(stringArg3, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(stringArg1, "");
	EXPECT_EQ(stringArg2, "");
	EXPECT_EQ(stringArg3, "");
}

// -----------------------------------------

TEST_CASE(OptionalStringArguments)
{
	// Single optional string argument
	std::string stringArg1 = "";
	auto result = runParser({ "my-optional-argument" }, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringArg1, "my-optional-argument");

	// Single optional string argument, not given
	stringArg1 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringArg1, "");

	// Multiple optional string arguments
	stringArg1 = "";
	std::string stringArg2 = "";
	std::string stringArg3 = "";
	result = runParser({ "my", "optional", "argument" }, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(stringArg2, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(stringArg3, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringArg1, "my");
	EXPECT_EQ(stringArg2, "optional");
	EXPECT_EQ(stringArg3, "argument");

	// Multiple optional string arguments, not given
	stringArg1 = "";
	stringArg2 = "";
	stringArg3 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(stringArg2, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(stringArg3, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringArg1, "");
	EXPECT_EQ(stringArg2, "");
	EXPECT_EQ(stringArg3, "");
}

// -----------------------------------------

TEST_CASE(NonRequiredStringArguments)
{
	// Single non-required string argument
	std::string stringArg1 = "";
	auto result = runParser({ "my-non-required-argument" }, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringArg1, "my-non-required-argument");

	// Single non-required string argument, not given
	stringArg1 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringArg1, "");

	// Multiple non-required string arguments
	stringArg1 = "";
	std::string stringArg2 = "";
	std::string stringArg3 = "";
	result = runParser({ "my", "non-required", "argument" }, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
		parser.addArgument(stringArg2, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
		parser.addArgument(stringArg3, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringArg1, "my");
	EXPECT_EQ(stringArg2, "non-required");
	EXPECT_EQ(stringArg3, "argument");

	// Multiple non-required string arguments, not given
	stringArg1 = "";
	stringArg2 = "";
	stringArg3 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
		parser.addArgument(stringArg2, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
		parser.addArgument(stringArg3, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringArg1, "");
	EXPECT_EQ(stringArg2, "");
	EXPECT_EQ(stringArg3, "");
}

// -----------------------------------------

TEST_CASE(VectorStringArguments)
{
	// Required vector string argument, not given
	std::vector<std::string> vectorArg1 = {};
	auto result = runParser({}, [&](auto& parser) {
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(vectorArg1.size(), 0);

	// Required vector string argument, one given
	vectorArg1 = {};
	result = runParser({ "foo" }, [&](auto& parser) {
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(vectorArg1.size(), 1);
	if (vectorArg1.size() == 1) {
		EXPECT_EQ(vectorArg1[0], "foo");
	}

	// Required vector string argument, two given
	vectorArg1 = {};
	result = runParser({ "hello", "world" }, [&](auto& parser) {
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(vectorArg1.size(), 2);
	if (vectorArg1.size() == 2) {
		EXPECT_EQ(vectorArg1[0], "hello");
		EXPECT_EQ(vectorArg1[1], "world");
	}
}

// -----------------------------------------

TEST_CASE(CombinationOfNonRequiredArguments)
{
	// Optional arguments, one given
	int intArg1 = 0;
	double doubleArg1 = 0;
	std::string stringArg1 = "";
	auto result = runParser({ "optional argument" }, [&](auto& parser) {
		parser.addArgument(intArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(doubleArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(intArg1, 0);
	EXPECT_EQ(doubleArg1, 0);
	EXPECT_EQ(stringArg1, "optional argument");

	// Optional arguments, two given
	intArg1 = 0;
	doubleArg1 = 0;
	stringArg1 = "";
	result = runParser({ "999.999", "optional argument" }, [&](auto& parser) {
		parser.addArgument(intArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(doubleArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(intArg1, 0);
	EXPECT_EQ(doubleArg1, 999.999);
	EXPECT_EQ(stringArg1, "optional argument");

	// Optional arguments, two given, one valid
	intArg1 = 0;
	doubleArg1 = 0;
	stringArg1 = "";
	result = runParser({ "999,999", "optional argument" }, [&](auto& parser) {
		parser.addArgument(intArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(doubleArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(intArg1, 0);
	EXPECT_EQ(doubleArg1, 0);
	EXPECT_EQ(stringArg1, "999,999");

	// Optional arguments, two given, both valid but wrong order
	stringArg1 = "";
	intArg1 = 0;
	doubleArg1 = 0;
	result = runParser({ "999.999", "optional argument" }, [&](auto& parser) {
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(intArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(doubleArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(stringArg1, "999.999");
	EXPECT_EQ(intArg1, 0);
	EXPECT_EQ(doubleArg1, 0);
}

// -----------------------------------------

TEST_CASE(BoolOptions)
{
	// Short option
	bool boolOpt1 = false;
	auto result = runParser({ "-b" }, [&](auto& parser) {
		parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);

	// Short option, not given
	boolOpt1 = false;
	result = runParser({}, [&](auto& parser) {
		parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, false);

	// Long option
	boolOpt1 = false;
	result = runParser({ "--bool" }, [&](auto& parser) {
		parser.addOption(boolOpt1, '\0', "bool", nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);

	// Long option, not given
	boolOpt1 = false;
	result = runParser({}, [&](auto& parser) {
		parser.addOption(boolOpt1, '\0', "bool", nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, false);

	// Allow both short and long option, provide short
	boolOpt1 = false;
	result = runParser({ "-b" }, [&](auto& parser) {
		parser.addOption(boolOpt1, 'b', "bool", nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);

	// Allow both short and long option, provide long
	boolOpt1 = false;
	result = runParser({ "--bool" }, [&](auto& parser) {
		parser.addOption(boolOpt1, 'b', "bool", nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);

	// Allow both short and long option, provide both
	boolOpt1 = false;
	result = runParser({ "-b", "--bool" }, [&](auto& parser) {
		parser.addOption(boolOpt1, 'b', "bool", nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);
}

// -----------------------------------------

TEST_CASE(SingleRequiredStringOptions)
{
	// Single required string short option
	std::string stringOpt1 = "";
	auto result = runParser({ "-s", "my-required-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "my-required-argument");

	// Single required string short option, given directly after
	stringOpt1 = "";
	result = runParser({ "-smy-required-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "my-required-argument");

	// Single required string short option, empty given
	stringOpt1 = "";
	result = runParser({ "-s" }, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(stringOpt1, "");

	// Single required string short option, not given
	stringOpt1 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");

	// Single required string long option
	stringOpt1 = "";
	result = runParser({ "--string", "my-required-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "my-required-argument");

	// Single required string long option, given directly after
	stringOpt1 = "";
	result = runParser({ "--string=my-required-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "my-required-argument");

	// Single required string long option, empty given
	stringOpt1 = "";
	result = runParser({ "--string" }, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(stringOpt1, "");

	// Single required string long option, not given
	stringOpt1 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");
}

// -----------------------------------------

TEST_CASE(SingleOptionalStringOptions)
{
	// Single optional string short option
	std::string stringOpt1 = "";
	std::string stringArg1 = "";
	auto result = runParser({ "-s", "my-optional-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");
	EXPECT_EQ(stringArg1, "my-optional-argument");

	// Single optional string short option, given directly after
	stringOpt1 = "";
	result = runParser({ "-smy-optional-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "my-optional-argument");

	// Single optional string short option, empty given
	stringOpt1 = "";
	result = runParser({ "-s" }, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");

	// Single optional string short option, not given
	stringOpt1 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");

	// Single optional string long option
	stringOpt1 = "";
	stringArg1 = "";
	result = runParser({ "--string", "my-optional-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");
	EXPECT_EQ(stringArg1, "my-optional-argument");

	// Single optional string long option, given directly after
	stringOpt1 = "";
	result = runParser({ "--string=my-optional-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "my-optional-argument");

	// Single optional string long option, empty given
	stringOpt1 = "";
	result = runParser({ "--string" }, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");

	// Single optional string long option, not given
	stringOpt1 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::Optional);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");
}

// -----------------------------------------

TEST_CASE(SingleNonRequiredStringOptions)
{
	// Single non-required string short option
	std::string stringOpt1 = "";
	std::string stringArg1 = "";
	auto result = runParser({ "-s", "my-non-required-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");
	EXPECT_EQ(stringArg1, "my-non-required-argument");

	// Single non-required string short option, given directly after
	stringOpt1 = "";
	result = runParser({ "-smy-non-required-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(stringOpt1, "");

	// Single non-required string short option, empty given
	stringOpt1 = "";
	result = runParser({ "-s" }, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");

	// Single non-required string short option, not given
	stringOpt1 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");

	// Single non-required string long option
	stringOpt1 = "";
	stringArg1 = "";
	result = runParser({ "--string", "my-non-required-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
		parser.addArgument(stringArg1, nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");
	EXPECT_EQ(stringArg1, "my-non-required-argument");

	// Single non-required string long option, given directly after
	stringOpt1 = "";
	result = runParser({ "--string=my-non-required-argument" }, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(stringOpt1, "");

	// Single non-required string long option, empty given
	stringOpt1 = "";
	result = runParser({ "--string" }, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");

	// Single non-required string long option, not given
	stringOpt1 = "";
	result = runParser({}, [&](auto& parser) {
		parser.addOption(stringOpt1, '\0', "string", nullptr, nullptr, nullptr, Util::ArgParser::Required::No);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(stringOpt1, "");
}

// -----------------------------------------

TEST_CASE(NumberOptions)
{
	// Required int short option
	int intOpt1 = 0;
	auto result = runParser({ "-i", "2147483647" }, [&](auto& parser) {
		parser.addOption(intOpt1, 'i', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(intOpt1, 2147483647);

	// Required int short option, overflown value given
	intOpt1 = 0;
	result = runParser({ "-i", "2147483648" }, [&](auto& parser) {
		parser.addOption(intOpt1, 'i', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(intOpt1, 0);

	// Required int short option, empty given
	intOpt1 = 0;
	result = runParser({ "-i" }, [&](auto& parser) {
		parser.addOption(intOpt1, 'i', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(intOpt1, 0);

	// Required unsigned int short option
	unsigned int unsignedIntOpt1 = 0;
	result = runParser({ "-u", "4294967295" }, [&](auto& parser) {
		parser.addOption(unsignedIntOpt1, 'u', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(unsignedIntOpt1, 4294967295);

	// Required unsigned int short option, overflown value given
	unsignedIntOpt1 = 0;
	result = runParser({ "-u", "4294967296" }, [&](auto& parser) {
		parser.addOption(unsignedIntOpt1, 'u', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(unsignedIntOpt1, 0);

	// Required unsigned int short option, empty given
	unsignedIntOpt1 = 0;
	result = runParser({ "-u" }, [&](auto& parser) {
		parser.addOption(unsignedIntOpt1, 'u', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(unsignedIntOpt1, 0);

	// Required double short option
	double doubleOpt1 = 0;
	result = runParser({ "-d", "999.999" }, [&](auto& parser) {
		parser.addOption(doubleOpt1, 'd', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(doubleOpt1, 999.999);

	// Required double short option, empty given
	doubleOpt1 = 0;
	result = runParser({ "-d" }, [&](auto& parser) {
		parser.addOption(doubleOpt1, 'd', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(doubleOpt1, 0);
}

// -----------------------------------------

TEST_CASE(VectorStringOptions)
{
	// Required vector string short option, not given
	std::vector<std::string> vectorOpt1 = {};
	auto result = runParser({}, [&](auto& parser) {
		parser.addOption(vectorOpt1, 'v', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(vectorOpt1.size(), 0);

	// Required vector string short option, empty given
	vectorOpt1 = {};
	result = runParser({ "-v" }, [&](auto& parser) {
		parser.addOption(vectorOpt1, 'v', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(vectorOpt1.size(), 0);

	// Required vector string short option, one given
	vectorOpt1 = {};
	result = runParser({ "-v", "a vector argument!" }, [&](auto& parser) {
		parser.addOption(vectorOpt1, 'v', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(vectorOpt1.size(), 1);
	if (vectorOpt1.size() == 1) {
		EXPECT_EQ(vectorOpt1[0], "a vector argument!");
	}

	// Required vector string short option, two given
	vectorOpt1 = {};
	result = runParser({ "-v", "hello", "-v", "world" }, [&](auto& parser) {
		parser.addOption(vectorOpt1, 'v', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(vectorOpt1.size(), 2);
	if (vectorOpt1.size() == 2) {
		EXPECT_EQ(vectorOpt1[0], "hello");
		EXPECT_EQ(vectorOpt1[1], "world");
	}

	// Required vector string short option, two given directly after
	vectorOpt1 = {};
	result = runParser({ "-vhello", "-vworld" }, [&](auto& parser) {
		parser.addOption(vectorOpt1, 'v', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(vectorOpt1.size(), 2);
	if (vectorOpt1.size() == 2) {
		EXPECT_EQ(vectorOpt1[0], "hello");
		EXPECT_EQ(vectorOpt1[1], "world");
	}

	// Required vector string long option, empty given
	vectorOpt1 = {};
	result = runParser({ "--vector" }, [&](auto& parser) {
		parser.addOption(vectorOpt1, '\0', "vector", nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(vectorOpt1.size(), 0);

	// Required vector string long option, one given
	vectorOpt1 = {};
	result = runParser({ "--vector", "a vector argument!" }, [&](auto& parser) {
		parser.addOption(vectorOpt1, '\0', "vector", nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(vectorOpt1.size(), 1);
	if (vectorOpt1.size() == 1) {
		EXPECT_EQ(vectorOpt1[0], "a vector argument!");
	}

	// Required vector string long option, two given
	vectorOpt1 = {};
	result = runParser({ "--vector", "hello", "--vector", "world" }, [&](auto& parser) {
		parser.addOption(vectorOpt1, '\0', "vector", nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(vectorOpt1.size(), 2);
	if (vectorOpt1.size() == 2) {
		EXPECT_EQ(vectorOpt1[0], "hello");
		EXPECT_EQ(vectorOpt1[1], "world");
	}

	// Required vector string long option, two given directly after
	vectorOpt1 = {};
	result = runParser({ "--vector=hello", "--vector=world" }, [&](auto& parser) {
		parser.addOption(vectorOpt1, '\0', "vector", nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(vectorOpt1.size(), 2);
	if (vectorOpt1.size() == 2) {
		EXPECT_EQ(vectorOpt1.at(0), "hello");
		EXPECT_EQ(vectorOpt1.at(1), "world");
	}
}

// -----------------------------------------

TEST_CASE(MultipleOptions)
{
	// Both short options, second is required, with a non-option parameter in-between
	bool boolOpt1 = false;
	std::string stringOpt1 = "";
	std::vector<std::string> vectorArg1 = {};
	auto result = runParser({ "-b", "something", "-s", "a-string-value" }, [&](auto& parser) {
		parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);
	EXPECT_EQ(stringOpt1, "a-string-value");
	EXPECT_EQ(vectorArg1.size(), 1);
	if (vectorArg1.size() == 1) {
		EXPECT_EQ(vectorArg1.at(0), "something");
	}
}

// -----------------------------------------

TEST_CASE(StopOnDoubleDashOption)
{
	// Bool short options, missing
	// Expected: The bool options are interpreted as non-option parameters
	bool boolOpt1 = false;
	bool boolOpt2 = false;
	std::vector<std::string> vectorArg1 = {};
	auto result = runParser({ "--", "-b", "-c" }, [&](auto& parser) {
		parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
		parser.addOption(boolOpt2, 'c', nullptr, nullptr, nullptr);
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, false);
	EXPECT_EQ(boolOpt2, false);
	EXPECT_EQ(vectorArg1.size(), 2);
	if (vectorArg1.size() == 2) {
		EXPECT_EQ(vectorArg1.at(0), "-b");
		EXPECT_EQ(vectorArg1.at(1), "-c");
	}

	// Bool short options, one given
	// Expected: boolOpt1 is set, one non-option parameter
	boolOpt1 = false;
	boolOpt2 = false;
	vectorArg1 = {};
	result = runParser({ "-b", "--", "-c" }, [&](auto& parser) {
		parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
		parser.addOption(boolOpt2, 'c', nullptr, nullptr, nullptr);
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);
	EXPECT_EQ(boolOpt2, false);
	EXPECT_EQ(vectorArg1.size(), 1);
	if (vectorArg1.size() == 1) {
		EXPECT_EQ(vectorArg1.at(0), "-c");
	}

	// Bool long options, missing
	// Expected: The bool options are interpreted as non-option parameters
	boolOpt1 = false;
	boolOpt2 = false;
	vectorArg1 = {};
	result = runParser({ "--", "--bool", "--cool" }, [&](auto& parser) {
		parser.addOption(boolOpt1, '\0', "bool", nullptr, nullptr);
		parser.addOption(boolOpt2, '\0', "cool", nullptr, nullptr);
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, false);
	EXPECT_EQ(boolOpt2, false);
	EXPECT_EQ(vectorArg1.size(), 2);
	if (vectorArg1.size() == 2) {
		EXPECT_EQ(vectorArg1.at(0), "--bool");
		EXPECT_EQ(vectorArg1.at(1), "--cool");
	}

	// Bool long options, one given
	// Expected: boolOpt1 is set, one non-option parameter
	boolOpt1 = false;
	boolOpt2 = false;
	vectorArg1 = {};
	result = runParser({ "--bool", "--", "--cool" }, [&](auto& parser) {
		parser.addOption(boolOpt1, '\0', "bool", nullptr, nullptr);
		parser.addOption(boolOpt2, '\0', "cool", nullptr, nullptr);
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);
	EXPECT_EQ(boolOpt2, false);
	EXPECT_EQ(vectorArg1.size(), 1);
	if (vectorArg1.size() == 1) {
		EXPECT_EQ(vectorArg1.at(0), "--cool");
	}
}

// -----------------------------------------

TEST_CASE(StopOnFirstNonOption)
{
	// Do not stop on first non-option; arguments are in correct order
	// Expected: The bool options are set and one non-option parameter
	bool boolOpt1 = false;
	bool boolOpt2 = false;
	std::vector<std::string> vectorArg1 = {};
	auto result = runParser({ "-b", "-c", "stopping" }, [&](auto& parser) {
		parser.setStopParsingOnFirstNonOption(false);
		parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
		parser.addOption(boolOpt2, 'c', nullptr, nullptr, nullptr);
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);
	EXPECT_EQ(boolOpt2, true);
	EXPECT_EQ(vectorArg1.size(), 1);
	if (vectorArg1.size() == 1) {
		EXPECT_EQ(vectorArg1.at(0), "stopping");
	}

	// Do not stop on first non-option; arguments are in wrong order
	// Expected: The bool options are set and one non-option parameter
	boolOpt1 = false;
	boolOpt2 = false;
	vectorArg1 = {};
	result = runParser({ "-b", "stopping", "-c" }, [&](auto& parser) {
		parser.setStopParsingOnFirstNonOption(false);
		parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
		parser.addOption(boolOpt2, 'c', nullptr, nullptr, nullptr);
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);
	EXPECT_EQ(boolOpt2, true);
	EXPECT_EQ(vectorArg1.size(), 1);
	if (vectorArg1.size() == 1) {
		EXPECT_EQ(vectorArg1.at(0), "stopping");
	}

	// Stop on first non option, arguments are in correct order
	// Expected: The bool options are set and one non-option parameter
	boolOpt1 = false;
	boolOpt2 = false;
	vectorArg1 = {};
	result = runParser({ "-b", "-c", "stopping" }, [&](auto& parser) {
		parser.setStopParsingOnFirstNonOption(true);
		parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
		parser.addOption(boolOpt2, 'c', nullptr, nullptr, nullptr);
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);
	EXPECT_EQ(boolOpt2, true);
	EXPECT_EQ(vectorArg1.size(), 1);
	if (vectorArg1.size() == 1) {
		EXPECT_EQ(vectorArg1.at(0), "stopping");
	}

	// Stop on first non option, arguments are in wrong order
	// Expected: boolOpt1 is set and the rest are non-option parameters
	boolOpt1 = false;
	boolOpt2 = false;
	vectorArg1 = {};
	result = runParser({ "-b", "stopping", "-c" }, [&](auto& parser) {
		parser.setStopParsingOnFirstNonOption(true);
		parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
		parser.addOption(boolOpt2, 'c', nullptr, nullptr, nullptr);
		parser.addArgument(vectorArg1, nullptr, nullptr, nullptr);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);
	EXPECT_EQ(boolOpt2, false);
	EXPECT_EQ(vectorArg1.size(), 2);
	if (vectorArg1.size() == 2) {
		EXPECT_EQ(vectorArg1.at(0), "stopping");
		EXPECT_EQ(vectorArg1.at(1), "-c");
	}
}

// -----------------------------------------

TEST_CASE(ExitOnFirstError)
{
	// Do not stop on first error, one non-existing given
	// Expected: parsing fails, boolOpt1 is set
	bool boolOpt1 = false;
	auto result = runParser({ "--this-doesnt-exist", "--this-exist" }, [&](auto& parser) {
		parser.setExitOnFirstError(false);
		parser.addOption(boolOpt1, '\0', "this-exist", nullptr, nullptr);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(boolOpt1, true);

	// Stop on first error, one non-existing given
	// Expected: parsing fails, boolOpt1 is not set
	boolOpt1 = false;
	result = runParser({ "--this-doesnt-exist", "--this-exist" }, [&](auto& parser) {
		parser.setExitOnFirstError(true);
		parser.addOption(boolOpt1, '\0', "this-exist", nullptr, nullptr);
	});
	EXPECT_EQ(result, false);
	EXPECT_EQ(boolOpt1, false);
}
