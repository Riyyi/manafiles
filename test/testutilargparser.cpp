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

TEST_CASE(NoArguments)
{
	auto result = runParser({});
	EXPECT_EQ(result, true);
}

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

TEST_CASE(MultipleOptions)
{
	// Both short options, second is required, with a non-option argument in-between
	bool boolOpt1 = false;
	std::string stringOpt1 = "";
	auto result = runParser({ "-b", "something", "-s", "a-string-value" }, [&](auto& parser) {
		parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
		parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
	});
	EXPECT_EQ(result, true);
	EXPECT_EQ(boolOpt1, true);
	EXPECT_EQ(stringOpt1, "a-string-value");
}
