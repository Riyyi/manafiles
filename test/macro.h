#ifndef TEST_H
#define TEST_H

#include <cstdio>   // fprintf
#include <iostream> // cerr

#define GET_2TH_ARG(arg1, arg2, ...) arg2
#define GET_3TH_ARG(arg1, arg2, arg3, ...) arg3
#define GET_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define MACRO_CHOOSER_1(macro, ...) \
	GET_2TH_ARG(__VA_ARGS__, macro##_1, )
#define MACRO_CHOOSER_2(macro, ...) \
	GET_3TH_ARG(__VA_ARGS__, macro##_2, macro##_1, )
#define MACRO_CHOOSER_3(macro, ...) \
	GET_4TH_ARG(__VA_ARGS__, macro##_3, macro##_2, macro##_1, )

// -----------------------------------------

#define EXPECT_IMPL(x, result)                                                 \
	if (!(x)) {                                                                \
		fprintf(stderr, " \033[31;1mFAIL:\033[0m  %s:%d: EXPECT(%s) failed\n", \
		        __FILE__, __LINE__, #x);                                       \
		Test::TestSuite::the().currentTestCaseFailed();                        \
		result;                                                                \
	}

#define EXPECT_1(x) \
	EXPECT_IMPL(x, (void)0)

#define EXPECT_2(x, result) \
	EXPECT_IMPL(x, result)

#define EXPECT(...)                      \
	MACRO_CHOOSER_2(EXPECT, __VA_ARGS__) \
	(__VA_ARGS__)

// -----------------------------------------

#define EXPECT_EQ_IMPL(a, b, result)                                            \
	if (a != b) {                                                               \
		std::cerr << " \033[31;1mFAIL:\033[0m  " << __FILE__ << ":" << __LINE__ \
				  << ": EXPECT_EQ(" << #a << ", " << #b ") failed with"         \
				  << " lhs='" << a << "' and rhs='" << b << "'" << std::endl;   \
		Test::TestSuite::the().currentTestCaseFailed();                         \
		result;                                                                 \
	}

#define EXPECT_EQ_2(a, b) \
	EXPECT_EQ_IMPL(a, b, (void)0)

#define EXPECT_EQ_3(a, b, result) \
	EXPECT_EQ_IMPL(a, b, result)

#define EXPECT_EQ(...)                      \
	MACRO_CHOOSER_3(EXPECT_EQ, __VA_ARGS__) \
	(__VA_ARGS__)

// -----------------------------------------

#define EXPECT_NE_IMPL(a, b, result)                                            \
	if (a == b) {                                                               \
		std::cerr << " \033[31;1mFAIL:\033[0m  " << __FILE__ << ":" << __LINE__ \
				  << ": EXPECT_NE(" << #a << ", " << #b ") failed with"         \
				  << " lhs='" << a << "' and rhs='" << b << "'" << std::endl;   \
		Test::TestSuite::the().currentTestCaseFailed();                         \
		result;                                                                 \
	}

#define EXPECT_NE_2(a, b) \
	EXPECT_NE_IMPL(a, b, (void)0)

#define EXPECT_NE_3(a, b, result) \
	EXPECT_NE_IMPL(a, b, result)

#define EXPECT_NE(...)                      \
	MACRO_CHOOSER_3(EXPECT_NE, __VA_ARGS__) \
	(__VA_ARGS__)

#endif // TEST_H
