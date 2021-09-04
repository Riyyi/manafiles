#include <cstdio> // fopen, printf, stdout
#include <string>
#include <vector>

#include "macro.h"
#include "util/argparser.h"

FILE* output;
FILE* null;

bool runParser(std::vector<const char*> arguments, std::function<void(Util::ArgParser&)> initializer = {})
{
	stdout = null;

	Util::ArgParser parser;
	if (initializer) {
		initializer(parser);
	}

	arguments.insert(arguments.begin(), "app");
	auto result = parser.parse(arguments.size(), arguments.data());

	stdout = output;
	return result;
}

int main(int, const char*[])
{
	output = stdout;
	null = fopen("/dev/null", "w"); // Windows: nul

	printf("Test project\n");

	// No arguments
	{
		auto result = runParser({});
		EXPECT_EQ(result, true);
	}

	// Bool options
	{
		// Short option
		bool boolOpt1 = false;
		auto result = runParser({ "-b" }, [&](auto& parser) {
			parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
		});
		EXPECT_EQ(result, true);
		EXPECT_EQ(boolOpt1, true);
	}
	{
		// Short option, not given
		bool boolOpt1 = false;
		auto result = runParser({}, [&](auto& parser) {
			parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
		});
		EXPECT_EQ(result, true);
		EXPECT_EQ(boolOpt1, false);
	}
	{
		// Long option
		bool boolOpt1 = false;
		auto result = runParser({ "--bool" }, [&](auto& parser) {
			parser.addOption(boolOpt1, '\0', "bool", nullptr, nullptr);
		});
		EXPECT_EQ(result, true);
		EXPECT_EQ(boolOpt1, true);
	}
	{
		// Long option, not given
		bool boolOpt1 = false;
		auto result = runParser({}, [&](auto& parser) {
			parser.addOption(boolOpt1, '\0', "bool", nullptr, nullptr);
		});
		EXPECT_EQ(result, true);
		EXPECT_EQ(boolOpt1, false);
	}
	{
		// Allow both short and long option, provide short
		bool boolOpt1 = false;
		auto result = runParser({ "-b" }, [&](auto& parser) {
			parser.addOption(boolOpt1, 'b', "bool", nullptr, nullptr);
		});
		EXPECT_EQ(result, true);
		EXPECT_EQ(boolOpt1, true);
	}
	{
		// Allow both short and long option, provide long
		bool boolOpt1 = false;
		auto result = runParser({ "--bool" }, [&](auto& parser) {
			parser.addOption(boolOpt1, 'b', "bool", nullptr, nullptr);
		});
		EXPECT_EQ(result, true);
		EXPECT_EQ(boolOpt1, true);
	}
	{
		// Allow both short and long option, provide both
		bool boolOpt1 = false;
		auto result = runParser({ "-b", "--bool" }, [&](auto& parser) {
			parser.addOption(boolOpt1, 'b', "bool", nullptr, nullptr);
		});
		EXPECT_EQ(result, true);
		EXPECT_EQ(boolOpt1, true);
	}

	// ..
	{
		//
		bool boolOpt1 = false;
		std::string stringOpt1 = "";
		auto result = runParser({ "-b", "something", "-s", "my-value" }, [&](auto& parser) {
			parser.addOption(boolOpt1, 'b', nullptr, nullptr, nullptr);
			parser.addOption(stringOpt1, 's', nullptr, nullptr, nullptr, nullptr, Util::ArgParser::Required::Yes);
		});
		EXPECT_EQ(result, true);
		EXPECT_EQ(boolOpt1, true);
		EXPECT_EQ(stringOpt1, "");
	}

	// // bool tests
	// test('o', "option", { "-o" },         true);
	// test('o', "option", { "-n" },         false);
	// test('o', "option", { "--option" },   true);
	// test('o', "option", { "--noexist" },  false);

	// // string tests
	// test('o', "option", Util::ArgParser::Required::Yes,       { "-o", "my-argument" },        "my-argument",  0);
	// test('o', "option", Util::ArgParser::Required::Optional,  { "-o", "my-argument" },        {},             0);
	// test('o', "option", Util::ArgParser::Required::No,        { "-o", "my-argument" },        {},             0);

	// test('o', "option", Util::ArgParser::Required::Yes,       { "-omy-argument" },            "my-argument",  0);
	// test('o', "option", Util::ArgParser::Required::Optional,  { "-omy-argument" },            "my-argument",  0);
	// test('o', "option", Util::ArgParser::Required::No,        { "-omy-argument" },            {},             0);

	// test('o', "option", Util::ArgParser::Required::Yes,       { "--option", "my-argument" },  "my-argument",  0);
	// test('o', "option", Util::ArgParser::Required::Optional,  { "--option", "my-argument" },  {},             0);
	// test('o', "option", Util::ArgParser::Required::No,        { "--option", "my-argument" },  {},             0);

	// test('o', "option", Util::ArgParser::Required::Yes,       { "--option=my-argument" },     "my-argument",  0);
	// test('o', "option", Util::ArgParser::Required::Optional,  { "--option=my-argument" },     "my-argument",  0);
	// test('o', "option", Util::ArgParser::Required::No ,       { "--option=my-argument" },     {},             0);

	// test('o', "option", Util::ArgParser::Required::Yes,       { "-o", "my-argument" },        "not-same",    -1);
	// test('o', "option", Util::ArgParser::Required::Yes,       { "-omy-argument" },            "not-same",    -1);
	// test('o', "option", Util::ArgParser::Required::Optional,  { "-omy-argument" },            "not-same",    -1);

	// test('o', "option", Util::ArgParser::Required::Yes,       { "--option", "my-argument" },  "not-same",    -1);
	// test('o', "option", Util::ArgParser::Required::Yes,       { "--option=my-argument" },     "not-same",    -1);
	// test('o', "option", Util::ArgParser::Required::Optional,  { "--option=my-argument" },     "not-same",    -1);

	// ./help -o something -a my-value
	// -a has required argument, but something should stop option parsing

	printf("Completed running tests\n");

	fclose(null);
	return 0;
}
