#ifndef TEST_H
#define TEST_H

#include <cstdio>   // fprintf
#include <iostream> // cerr

#define EXPECT(x)                                                              \
	if (!(x)) {                                                                \
		fprintf(stderr, " \033[31;1mFAIL:\033[0m  %s:%d: EXPECT(%s) failed\n", \
		        __FILE__, __LINE__, #x);                                       \
		Test::TestSuite::the().currentTestCaseFailed();                        \
	}

#define EXPECT_EQ(a, b)                                                         \
	if (a != b) {                                                               \
		std::cerr << " \033[31;1mFAIL:\033[0m  " << __FILE__ << ":" << __LINE__ \
				  << ": EXPECT_EQ(" << #a << ", " << #b ") failed with"         \
				  << " lhs='" << a << "' and rhs='" << b << "'" << std::endl;   \
		Test::TestSuite::the().currentTestCaseFailed();                         \
	}

#define EXPECT_NE(a, b)                                                         \
	if (a == b) {                                                               \
		std::cerr << " \033[31;1mFAIL:\033[0m  " << __FILE__ << ":" << __LINE__ \
				  << ": EXPECT_NE(" << #a << ", " << #b ") failed with"         \
				  << " lhs='" << a << "' and rhs='" << b << "'" << std::endl;   \
		Test::TestSuite::the().currentTestCaseFailed();                         \
	}

#endif // TEST_H
