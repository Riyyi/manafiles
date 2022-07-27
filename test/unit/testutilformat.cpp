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

	result = Util::format("");
	EXPECT_EQ(result, "");

	result = Util::format("{}", nullptr);
	EXPECT_EQ(result, "(nil)");

	int* number = new int(3);
	result = Util::format("{}", number);
	EXPECT_EQ(result.substr(0, 2), "0x");

	result = Util::format("{}", true);
	EXPECT_EQ(result, "true");

	result = Util::format("{}", false);
	EXPECT_EQ(result, "false");

	result = Util::format("{}", 'c');
	EXPECT_EQ(result, "c");

	const char* cString = "C string";
	result = Util::format("{}", cString);
	EXPECT_EQ(result, "C string");

	std::string string = "string";
	result = Util::format("{}", string);
	EXPECT_EQ(result, "string");

	std::string_view stringView = "string_view";
	result = Util::format("{}", stringView);
	EXPECT_EQ(result, "string_view");

	result = Util::format("{} {}", "Hello", "World");
	EXPECT_EQ(result, "Hello World");

	result = Util::format("{{escaped braces}}");
	EXPECT_EQ(result, "{escaped braces}");

	result = Util::format("{{braces{}}}", "Something");
	EXPECT_EQ(result, "{bracesSomething}");
}

TEST_CASE(FormatNumbers)
{
	std::string result;

	int32_t i32 = 68766;
	result = Util::format("{}", i32); // int
	EXPECT_EQ(result, "68766");

	uint32_t u32 = 123841; // unsigned int
	result = Util::format("{}", u32);
	EXPECT_EQ(result, "123841");

	int64_t i64 = 237942768427; // long int
	result = Util::format("{}", i64);
	EXPECT_EQ(result, "237942768427");

	size_t u64 = 1337; // long unsigned int
	result = Util::format("{}", u64);
	EXPECT_EQ(result, "1337");

	float f32R = 245789.70000;
	result = Util::format("{}", f32R);
	EXPECT_EQ(result, "245789.703125");

	float f32 = 45645.3233;
	result = Util::format("{}", f32);
	EXPECT_EQ(result, "45645.324219");

	double f64 = 87522.300000000;
	result = Util::format("{}", f64);
	EXPECT_EQ(result, "87522.300000");

	double pi = 3.14159265359;
	result = Util::format("{:.15}", pi);
	EXPECT_EQ(result, "3.141592653590000");
}

TEST_CASE(FormatContainers)
{
	std::string result;

	std::vector<std::string> vector { "thing1", "thing2", "thing3" };
	result = Util::format("{}", vector);
	EXPECT_EQ(result, R"({
    thing1,
    thing2,
    thing3
})");

	std::map<std::string, int> map { { "thing3", 3 }, { "thing2", 2 }, { "thing1", 1 } };
	result = Util::format("{}", map);
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
	result = Util::format("{}", twoDimensionalVector);
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
