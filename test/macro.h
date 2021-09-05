#ifndef TEST_H
#define TEST_H

#include <cstdio>   // printf
#include <iostream> // cout

#define EXPECT(x)                                                           \
	if (!(x)) {                                                             \
		printf("FAIL: %s:%d: EXPECT(%s) failed\n", __FILE__, __LINE__, #x); \
	}

#define EXPECT_EQ(a, b)                                                       \
	if (a != b) {                                                             \
		std::cout << "FAIL: " << __FILE__ << ":" << __LINE__                  \
				  << ": EXPECT_EQ(" << #a << ", " << #b ") failed with"       \
				  << " lhs='" << a << "' and rhs='" << b << "'" << std::endl; \
		Test::TestSuite::the().currentTestCaseFailed();                       \
	}

#endif // TEST_H
