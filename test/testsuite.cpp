#include <cstddef> // size_t
#include <cstdio> // fclose, fopen, printf, stdout

#include "testsuite.h"

namespace Test {

TestSuite::TestSuite(s)
{
	m_outputStd = stdout;
	m_outputNull = fopen("/dev/null", "w"); // Windows: nul
}

TestSuite::~TestSuite()
{
	fclose(m_outputNull);
}

void TestSuite::run()
{
	printf("TestSuite: %d cases have been added!\n", (int)m_cases.size());

	size_t caseFailedCount = 0;

	for(auto& testCase : m_cases) {
		printf("%s\n", testCase.name());
		m_currentTestCasePassed = true;

		testCase.function()();

		if (!m_currentTestCasePassed) {
			caseFailedCount++;
		}
	}

	int percentagePassed = (1 - caseFailedCount / (float)m_cases.size()) * 100;
	printf("Passed %d%% of tests\n", percentagePassed);
}

} // namespace Test
