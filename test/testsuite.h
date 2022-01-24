#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <cstdio> // FILE
#include <vector>

#include "testcase.h"
#include "util/singleton.h"

namespace Test {

class TestSuite final : public Util::Singleton<TestSuite> {
public:
	TestSuite(s);
	virtual ~TestSuite();

	void run();
	void addCase(const TestCase& testCase) { m_cases.push_back(testCase); }
	void currentTestCaseFailed() { m_currentTestCasePassed = false; }

	FILE* outputStd() const { return m_outputStd; }
	FILE* outputErr() const { return m_outputErr; }
	FILE* outputNull() const { return m_outputNull; }

private:
	bool m_currentTestCasePassed { true };
	FILE* m_outputStd { nullptr };
	FILE* m_outputErr { nullptr };
	FILE* m_outputNull { nullptr };

	std::vector<TestCase> m_cases;
};

} // namespace Test

#endif // TEST_SUITE_H
