/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstddef> // size_t
#include <cstdint> // int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t
#include <map>
#include <sstream> // stringstream
#include <string>
#include <unordered_map>
#include <vector>

#include "macro.h"
#include "testcase.h"
#include "testsuite.h"
#include "util/format/format.h"

// -----------------------------------------

TEST_CASE(FormatIntegral)
{
	std::string result;

	// Signed

	int8_t i8 = 127; // char
	result = Util::format("{}", i8);
	EXPECT_EQ(result, "127");

	int16_t i16 = 32767;
	result = Util::format("{}", i16);
	EXPECT_EQ(result, "32767");

	int32_t i32 = 68766; // int
	result = Util::format("{}", i32);
	EXPECT_EQ(result, "68766");

	int64_t i64 = 237942768427; // long int
	result = Util::format("{}", i64);
	EXPECT_EQ(result, "237942768427");

	// Unsigned

	uint8_t u8 = 255; // unsigned char
	result = Util::format("{}", u8);
	EXPECT_EQ(result, "255");

	uint16_t u16 = 65535;
	result = Util::format("{}", u16);
	EXPECT_EQ(result, "65535");

	uint32_t u32 = 4294967295; // unsigned int
	result = Util::format("{}", u32);
	EXPECT_EQ(result, "4294967295");

	size_t u64 = 18446744073709551615; // long unsigned int
	result = Util::format("{}", u64);
	EXPECT_EQ(result, "18446744073709551615");
}

TEST_CASE(FormatFloatingPoint)
{
	std::string result;

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
	result = Util::format("{}", pi);
	EXPECT_EQ(result, "3.141593");
}

TEST_CASE(FormatChar)
{
	std::string result;

	char character = 'A';
	result = Util::format("{}", character);
	EXPECT_EQ(result, "A");

	bool boolean = true;
	result = Util::format("{}", boolean);
	EXPECT_EQ(result, "true");

	boolean = false;
	result = Util::format("{}", boolean);
	EXPECT_EQ(result, "false");
}

TEST_CASE(FormatString)
{
	std::string result;

	result = Util::format("");
	EXPECT_EQ(result, "");

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

TEST_CASE(FormatPointer)
{
	std::string result;

	result = Util::format("{}", nullptr);
	EXPECT_EQ(result, "0x0");

	int integer = 42;
	std::stringstream stream;
	stream << &integer;
	std::string pointer = stream.str();

	result = Util::format("{}", &integer);
	EXPECT_EQ(result, pointer);
}

TEST_CASE(FormatSpecifierIntegral)
{
	std::string result;

	// Fill and Align
	result = Util::format("{:+<}", 12345);
	EXPECT_EQ(result, "12345");
	result = Util::format("{:+^}", 12345);
	EXPECT_EQ(result, "12345");
	result = Util::format("{:+>}", 12345);
	EXPECT_EQ(result, "12345");

	// Sign
	result = Util::format("{:+}", 12345);
	EXPECT_EQ(result, "+12345");
	result = Util::format("{:+}", -12345);
	EXPECT_EQ(result, "-12345");
	result = Util::format("{:-}", 12345);
	EXPECT_EQ(result, "12345");
	result = Util::format("{:-}", -12345);
	EXPECT_EQ(result, "-12345");
	result = Util::format("{: }", 12345);
	EXPECT_EQ(result, " 12345");
	result = Util::format("{: }", -12345);
	EXPECT_EQ(result, "-12345");

	// AlternativeForm
	result = Util::format("{:#}", 12345);
	EXPECT_EQ(result, "12345");

	// ZeroPadding
	result = Util::format("{:0}", 12345);
	EXPECT_EQ(result, "12345");

	// Width
	result = Util::format("{:10}", 12345);
	EXPECT_EQ(result, "     12345");

	// Width + Fill and Align
	result = Util::format("{:+<10}", 12345);
	EXPECT_EQ(result, "12345+++++");
	result = Util::format("{:+^10}", 12345);
	EXPECT_EQ(result, "++12345+++");
	result = Util::format("{:+>10}", 12345);
	EXPECT_EQ(result, "+++++12345");

	// Width + ZeroPadding
	result = Util::format("{:010}", 12345);
	EXPECT_EQ(result, "0000012345");

	// Precision
	// Not possible on integral types

	// Type
	result = Util::format("{:b}", 12345);
	EXPECT_EQ(result, "11000000111001");
	result = Util::format("{:B}", 12345);
	EXPECT_EQ(result, "11000000111001");
	result = Util::format("{:c}", 65);
	EXPECT_EQ(result, "A");
	result = Util::format("{:o}", 12345);
	EXPECT_EQ(result, "30071");
	result = Util::format("{:x}", 62432);
	EXPECT_EQ(result, "f3e0");
	result = Util::format("{:X}", 62432);
	EXPECT_EQ(result, "F3E0");

	// Type + AlternativeForm
	result = Util::format("{:#b}", 12345);
	EXPECT_EQ(result, "0b11000000111001");
	result = Util::format("{:#B}", 12345);
	EXPECT_EQ(result, "0B11000000111001");
	result = Util::format("{:#c}", 65);
	EXPECT_EQ(result, "A");
	result = Util::format("{:#o}", 12345);
	EXPECT_EQ(result, "030071");
	result = Util::format("{:#x}", 62432);
	EXPECT_EQ(result, "0xf3e0");
	result = Util::format("{:#X}", 62432);
	EXPECT_EQ(result, "0XF3E0");
}

TEST_CASE(FormatSpecifierIntegralCombination)
{
	std::string result;

	// AlternativeForm + ZeroPadding + Width + Type
	// ------------------------------

	result = Util::format("{:-#010d}", 402);
	EXPECT_EQ(result, "0000000402");

	// AlternativeForm + Width + Type
	// ------------------------------

	result = Util::format("{:#10x}", 402);
	EXPECT_EQ(result, "     0x192");

	// + Fill and Align

	result = Util::format("{:^<#10x}", 402);
	EXPECT_EQ(result, "0x192^^^^^");

	result = Util::format("{:^^#10x}", 402);
	EXPECT_EQ(result, "^^0x192^^^");

	result = Util::format("{:^>#10x}", 402);
	EXPECT_EQ(result, "^^^^^0x192");

	// ------------------------------

	// + Sign

	result = Util::format("{:+#10x}", 402);
	EXPECT_EQ(result, "    +0x192");

	// + Fill and Align + Sign

	result = Util::format("{:^<+#10x}", 402);
	EXPECT_EQ(result, "+0x192^^^^");

	result = Util::format("{:^^+#10x}", 402);
	EXPECT_EQ(result, "^^+0x192^^");

	result = Util::format("{:^>+#10x}", 402);
	EXPECT_EQ(result, "^^^^+0x192");

	// ------------------------------

	// + ZeroPadding

	result = Util::format("{:#010x}", 402);
	EXPECT_EQ(result, "0x00000192");

	// Fill and Align + ZeroPadding

	result = Util::format("{:^<#010x}", 402);
	EXPECT_EQ(result, "0x19200000");

	result = Util::format("{:^^#010x}", 402);
	EXPECT_EQ(result, "000x192000");

	result = Util::format("{:^>#010x}", 402);
	EXPECT_EQ(result, "000000x192");

	// ------------------------------

	// + Sign + ZeroPadding

	result = Util::format("{:+#010x}", 402);
	EXPECT_EQ(result, "+0x0000192");

	// + Fill and Align + Sign + ZeroPadding

	result = Util::format("{:^<+#010x}", 402);
	EXPECT_EQ(result, "+0x1920000");

	result = Util::format("{:^^+#010x}", 402);
	EXPECT_EQ(result, "00+0x19200");

	result = Util::format("{:^>+#010x}", 402);
	EXPECT_EQ(result, "0000+0x192");
}

TEST_CASE(FormatSpecifierFloatingPoint)
{
}

TEST_CASE(FormatSpecifierChar)
{
	std::string result;

	char character = 65;
	result = Util::format("{:b}", character);
	EXPECT_EQ(result, "1000001");
	result = Util::format("{:B}", character);
	EXPECT_EQ(result, "1000001");
	result = Util::format("{:d}", character);
	EXPECT_EQ(result, "65");
	result = Util::format("{:o}", character);
	EXPECT_EQ(result, "101");
	result = Util::format("{:x}", character);
	EXPECT_EQ(result, "41");
	result = Util::format("{:X}", character);
	EXPECT_EQ(result, "41");

	bool boolean = true;
	result = Util::format("{:b}", boolean);
	EXPECT_EQ(result, "1");
	result = Util::format("{:B}", boolean);
	EXPECT_EQ(result, "1");
	result = Util::format("{:d}", boolean);
	EXPECT_EQ(result, "1");
	result = Util::format("{:o}", boolean);
	EXPECT_EQ(result, "1");
	result = Util::format("{:x}", boolean);
	EXPECT_EQ(result, "1");
	result = Util::format("{:X}", boolean);
	EXPECT_EQ(result, "1");

	boolean = false;
	result = Util::format("{:b}", boolean);
	EXPECT_EQ(result, "0");
	result = Util::format("{:B}", boolean);
	EXPECT_EQ(result, "0");
	result = Util::format("{:d}", boolean);
	EXPECT_EQ(result, "0");
	result = Util::format("{:o}", boolean);
	EXPECT_EQ(result, "0");
	result = Util::format("{:x}", boolean);
	EXPECT_EQ(result, "0");
	result = Util::format("{:X}", boolean);
	EXPECT_EQ(result, "0");
}

TEST_CASE(FormatSpecifierString)
{
	std::string result;

	std::string string = "my string";

	// Fill and Align
	result = Util::format("{:+<}", string);
	EXPECT_EQ(result, "my string");
	result = Util::format("{:+^}", string);
	EXPECT_EQ(result, "my string");
	result = Util::format("{:+>}", string);
	EXPECT_EQ(result, "my string");

	// Sign
	// Not possible on string types

	// AlternativeForm
	// Not possible on string types

	// ZeroPadding
	// Not possible on string types

	// Width
	result = Util::format("{:15}", string);
	EXPECT_EQ(result, "my string      ");

	// Width + Fill and Align
	result = Util::format("{:+<15}", string);
	EXPECT_EQ(result, "my string++++++");
	result = Util::format("{:+^15}", string);
	EXPECT_EQ(result, "+++my string+++");
	result = Util::format("{:+>15}", string);
	EXPECT_EQ(result, "++++++my string");

	// Precision
	// Not possible on string types

	// Type
	result = Util::format("{:s}", string);
	EXPECT_EQ(result, "my string");
}

TEST_CASE(FormatSpecifierPointer)
{
	std::string result;

	int integer = 42;
	std::stringstream stream;
	stream << &integer;
	std::string pointer = stream.str();

	// Fill and Align
	result = Util::format("{:+<}", &integer);
	EXPECT_EQ(result, pointer);
	result = Util::format("{:+^}", &integer);
	EXPECT_EQ(result, pointer);
	result = Util::format("{:+>}", &integer);
	EXPECT_EQ(result, pointer);

	// Sign
	// Not possible on string types

	// AlternativeForm
	// Not possible on string types

	// ZeroPadding
	// Not possible on string types

	// Width
	result = Util::format("{:24}", &integer);
	EXPECT_EQ(result, std::string(24 - pointer.length(), ' ') + pointer);

	// Width + Fill and Align
	result = Util::format("{:+<24}", &integer);
	EXPECT_EQ(result, pointer + std::string(24 - pointer.length(), '+'));
	result = Util::format("{:+^24}", &integer);
	EXPECT_EQ(result, std::string((24 - pointer.length()) / 2, '+') + pointer + std::string((24 - pointer.length()) / 2, '+'));
	result = Util::format("{:+>24}", &integer);
	EXPECT_EQ(result, std::string(24 - pointer.length(), '+') + pointer);

	// Precision
	// Not possible on string types

	// Type
	result = Util::format("{:p}", &integer);
	EXPECT_EQ(result, pointer);
}

TEST_CASE(FormatContainers)
{
	std::string result;

	std::vector<std::string> vector { "thing1", "thing2", "thing3" };
	result = Util::format("{}", vector);
	EXPECT_EQ(result, "{thing1,thing2,thing3}");
	result = Util::format("{:1}", vector);
	EXPECT_EQ(result, "{ thing1, thing2, thing3 }");
	result = Util::format("{:#4}", vector);
	EXPECT_EQ(result, R"({
    thing1,
    thing2,
    thing3
})");
	result = Util::format("{:\t<#1}", vector);
	EXPECT_EQ(result, R"({
	thing1,
	thing2,
	thing3
})");

	std::map<std::string, int> map { { "thing3", 3 }, { "thing2", 2 }, { "thing1", 1 } };
	result = Util::format("{}", map);
	EXPECT_EQ(result, R"({"thing1":1,"thing2":2,"thing3":3})");
	result = Util::format("{:1}", map);
	EXPECT_EQ(result, R"({ "thing1": 1, "thing2": 2, "thing3": 3 })");
	result = Util::format("{:#4}", map);
	EXPECT_EQ(result, R"({
    "thing1": 1,
    "thing2": 2,
    "thing3": 3
})");
	result = Util::format("{:\t<#1}", map);
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
	result = Util::format("{:#4}", twoDimensionalVector);
	EXPECT_EQ(result, R"({
    {thing1,thing2,thing3},
    {thing1,thing2,thing3}
})");
}
