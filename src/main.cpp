// #include <cstddef> // size_t
// #include <cstdlib> // maloc, free
#include <string>

#include "util/argparser.h"

// void* operator new(size_t size)
// {
// 	std::cout << "Allocating '" << size << "' bytes" << std::endl;
// 	return std::malloc(size);
// }

// void operator delete(void* pointer, size_t size)
// {
// 	std::cout << "Freeing '" << size << "' bytes" << std::endl;
// 	free(pointer);
// }

int main(int argc, const char* argv[])
{
	bool pattern = false;
	std::string stringArg1 = "default value";
	std::string stringArg2 = "nothing";

	Util::ArgParser parser;
	// parser.setExitOnFirstError(false);
	// parser.setErrorMessages(false);

	parser.addOption(pattern, 'e', "regexp", "search pattern", "Use ${U}PATTERNS${N} as the patterns.");
	parser.addOption(stringArg1, 'a', "arg1", "test argument", "Test argument manpage description.", "TEST", Util::ArgParser::Required::Yes);
	parser.addOption(stringArg2, 'b', "arg2", "optional argument", "Option with optional argument", "TEST", Util::ArgParser::Required::Optional);
	parser.parse(argc, argv);

	printf("  Pattern:  {%d}\n", pattern);
	printf("  Arg1:     {%s}\n", stringArg1.data());
	printf("  Arg2:     {%s}\n", stringArg2.data());

	return 0;
}
