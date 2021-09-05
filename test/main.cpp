#include "testsuite.h"

int main(int, const char*[])
{
	Test::TestSuite::the().run();

	return 0;
}
