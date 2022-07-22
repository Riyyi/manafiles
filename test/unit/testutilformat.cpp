/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstddef> // size_t
#include <cstdint> // int32_t, uint32_t, int64_t
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "macro.h"
#include "testcase.h"
#include "testsuite.h"
#include "util/format/format.h"

// -----------------------------------------

TEST_CASE(FormatBasicTypes)
{
	std::string result;

	strln(result, "");
	EXPECT_EQ(result, "");

	strln(result, "{}", nullptr);
	EXPECT_EQ(result, "(nil)");

	int* number = new int(3);
	strln(result, "{}", number);
	EXPECT_EQ(result.substr(0, 2), "0x");

	strln(result, "{}", true);
	EXPECT_EQ(result, "true");

	strln(result, "{}", false);
	EXPECT_EQ(result, "false");

	strln(result, "{}", 'c');
	EXPECT_EQ(result, "c");

	const char* cString = "C string";
	strln(result, "{}", cString);
	EXPECT_EQ(result, "C string");

	std::string string = "string";
	strln(result, "{}", string);
	EXPECT_EQ(result, "string");

	std::string_view stringView = "string_view";
	strln(result, "{}", stringView);
	EXPECT_EQ(result, "string_view");

	strln(result, "{} {}", "Hello", "World");
	EXPECT_EQ(result, "Hello World");

	strln(result, "{{escaped braces}}");
	EXPECT_EQ(result, "{escaped braces}");

	strln(result, "{{braces{}}}", "Something");
	EXPECT_EQ(result, "{bracesSomething}");
}

TEST_CASE(FormatNumbers)
{
	std::string result;

	int32_t i32 = 68766;
	strln(result, "{}", i32); // int
	EXPECT_EQ(result, "68766");

	uint32_t u32 = 123841; // unsigned int
	strln(result, "{}", u32);
	EXPECT_EQ(result, "123841");

	int64_t i64 = 237942768427; // long int
	strln(result, "{}", i64);
	EXPECT_EQ(result, "237942768427");

	size_t u64 = 1337; // long unsigned int
	strln(result, "{}", u64);
	EXPECT_EQ(result, "1337");

	float f32R = 245789.70000;
	strln(result, "{}", f32R);
	EXPECT_EQ(result, "245789.7");

	float f32 = 45645.3233;
	strln(result, "{}", f32);
	EXPECT_EQ(result, "45645.324219");

	double f64 = 87522.300000000;
	strln(result, "{}", f64);
	EXPECT_EQ(result, "87522.3");

	double pi = 3.14159265359;
	strln(result, "{:.15}", pi);
	EXPECT_EQ(result, "3.14159265359");
}

TEST_CASE(FormatContainers)
{
	std::string result;

	std::vector<std::string> vector { "thing1", "thing2", "thing3" };
	strln(result, "{}", vector);
	EXPECT_EQ(result, R"({
    thing1,
    thing2,
    thing3
})");

	std::map<std::string, int> map { { "thing3", 3 }, { "thing2", 2 }, { "thing1", 1 } };
	strln(result, "{}", map);
	EXPECT_EQ(result, R"({
    "thing1": 1,
    "thing2": 2,
    "thing3": 3
})");

	// Multidimensional containers arent supported,
	// the user should write a customization point
	std::vector<std::vector<std::string>> twoDimensionalVector {
		{ "thing1", "thing2", "thing3" },
		{ "thing1", "thing2", "thing3" }
	};
	strln(result, "{}", twoDimensionalVector);
	EXPECT_EQ(result, R"({
    {
    thing1,
    thing2,
    thing3
},
    {
    thing1,
    thing2,
    thing3
}
})");
}
