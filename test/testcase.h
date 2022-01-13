#ifndef TEST_CASE_H
#define TEST_CASE_H

#include <functional>
#include <string>

#define __TEST_CASE_FUNCTION(x) __test##x
#define __TEST_CASE_STRUCT(x) __testStruct##x

#define TEST_CASE(x)                                           \
	static void __TEST_CASE_FUNCTION(x)();                     \
	struct __TEST_CASE_STRUCT(x) {                             \
		__TEST_CASE_STRUCT(x)                                  \
		()                                                     \
		{                                                      \
			Test::TestSuite::the().addCase(                    \
				{ #x, __TEST_CASE_FUNCTION(x) });              \
		}                                                      \
	};                                                         \
	static struct __TEST_CASE_STRUCT(x) __TEST_CASE_STRUCT(x); \
	static void __TEST_CASE_FUNCTION(x)()

namespace Test {

using TestFunction = std::function<void()>;

class TestCase {
public:
	TestCase(const char* name, TestFunction&& function)
		: m_name(name)
		, m_function(function)
	{
	}

	const char* name() const { return m_name; }
	const TestFunction& function() const { return m_function; }

private:
	const char* m_name { nullptr };
	TestFunction m_function;
};

} // namespace Test

#endif // TEST_CASE_H
