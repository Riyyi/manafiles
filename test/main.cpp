#include "testsuite.h"

int main(int, const char*[])
{
	Test::TestSuite::the().run();

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

	return 0;
}
