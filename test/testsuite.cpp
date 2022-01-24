#include <cstddef> // size_t
#include <cstdio>  // fclose, fopen, printf, stdout
#include <string>
#include <vector>

#include "testsuite.h"
#include "util/timer.h"

namespace Test {

TestSuite::TestSuite(s)
{
	m_outputStd = stdout;
	m_outputErr = stderr;
	m_outputNull = fopen("/dev/null", "w"); // Windows: nul
}

TestSuite::~TestSuite()
{
	fclose(m_outputNull);
}

void TestSuite::run()
{
	const char* escapePass = "\033[42;30;1m";
	const char* escapeFail = "\033[41;1m";
	const char* escapeGreen = "\033[32m";
	const char* escapeGrey = "\033[37m";
	const char* escapeRed = "\033[31m";
	const char* escapeReset = "\033[0m";

	printf("\n");
	printf("---- Running %zu Test Cases ----\n", m_cases.size());

	Util::Timer totalTimer;

	size_t caseFailedCount = 0;
	for (size_t i = 0; i < m_cases.size(); ++i) {

		printf(" START  %s (%zu/%zu)\n", m_cases.at(i).name(), i + 1, m_cases.size());
		m_currentTestCasePassed = true;

		Util::Timer caseTimer;
		m_cases.at(i).function()();
		double elapsed = caseTimer.elapsedNanoseconds();

		std::string state;
		if (m_currentTestCasePassed) {
			state.append(escapePass);
			state.append(" PASS  ");
			state.append(escapeReset);
		}
		else {
			caseFailedCount++;
			state.append(escapeFail);
			state.append(" FAIL  ");
			state.append(escapeReset);
		}

		printf("%s %s %s(", state.c_str(), m_cases.at(i).name(), escapeGrey);
		Util::Timer::fancyPrint(elapsed);
		printf(")%s\n", escapeReset);
	}

	printf("\n");
	printf("Tests:  %s%zu failed%s, %s%zu passed%s, %zu total\n",
	       escapeRed, caseFailedCount, escapeReset,
	       escapeGreen, m_cases.size() - caseFailedCount, escapeReset,
	       m_cases.size());

	printf("Time:   ");
	Util::Timer::fancyPrint(totalTimer.elapsedNanoseconds());
	printf("\n");
}

} // namespace Test
