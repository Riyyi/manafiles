#include <cstddef> // size_t
#include <cstdint> // uint32_t
#include <cstdio> // fclose, fopen, printf, stdout

#include "testsuite.h"
#include "util/timer.h"

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

	Util::Timer totalTimer;

	for (auto& testCase : m_cases) {
		printf("Start %s\n", testCase.name());
		m_currentTestCasePassed = true;

		Util::Timer caseTimer;
		testCase.function()();
		printf("      %s, %luns\n", testCase.name(), caseTimer.elapsedNanoseconds());

		if (!m_currentTestCasePassed) {
			caseFailedCount++;
		}
	}

	uint32_t percentagePassed = (1 - caseFailedCount / (float)m_cases.size()) * 100;
	printf("Passed %d%% of tests\n", percentagePassed);

	float elapsed = totalTimer.elapsedNanoseconds() / 1000000.0;
	printf("Elapsed: %.3f milliseconds\n", elapsed);
}

} // namespace Test
