/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <cstdint> // int8_t, int32_t, int64_t, uint8_t, uint32_t
#include <map>
#include <string>
#include <string_view>
#include <type_traits> // is_integral_v, is_same
#include <unordered_map>
#include <vector>

#include "util/format/builder.h"
#include "util/format/parser.h"

namespace Util::Format {

enum class PresentationType : uint8_t {
	None, // Defaults are any of: 'dgcsp', depending on the type
	// Interger
	Binary = 98,          // 'b'
	BinaryUppercase = 66, // 'B'
	Decimal = 100,        // 'd'
	Octal = 111,          // 'o'
	Hex = 120,            // 'x'
	HexUppercase = 88,    // 'X'
	// Floating-point
	Hexfloat = 97,            // 'a'
	HexfloatUppercase = 65,   // 'A'
	Exponent = 101,           // 'e'
	ExponentUppercase = 69,   // 'E'
	FixedPoint = 102,         // 'f'
	FixedPointUppercase = 70, // 'F'
	General = 103,            // 'g'
	GeneralUppercase = 71,    // 'G'
	// Character
	Character = 99, // 'c'
	// String
	String = 115, // 's'
	// Pointer
	Pointer = 112, // 'p'
};

struct Specifier {
	char fill = ' ';
	Builder::Align align = Builder::Align::None;

	Builder::Sign sign = Builder::Sign::None;

	bool alternativeForm = false;
	bool zeroPadding = false;
	int width = 0;
	int8_t precision = -1;

	PresentationType type = PresentationType::None;
};

template<typename T>
struct Formatter {
	Specifier specifier;

	constexpr void parse(Parser& parser)
	{
		if (std::is_integral_v<T>) {
			parser.parseSpecifier(specifier, Parser::SpecifierType::Integral);
		}
		else if (std::is_floating_point_v<T>) {
			parser.parseSpecifier(specifier, Parser::SpecifierType::FloatingPoint);
		}
		else if (std::is_same_v<T, char>) {
			parser.parseSpecifier(specifier, Parser::SpecifierType::Char);
		}
		else if (std::is_same_v<T, std::string_view>) {
			parser.parseSpecifier(specifier, Parser::SpecifierType::String);
		}
	}

	void format(Builder& builder, T value) const { (void)builder, (void)value; }
};

// Integral

template<>
void Formatter<int32_t>::format(Builder& builder, int32_t value) const;

template<>
void Formatter<uint32_t>::format(Builder& builder, uint32_t value) const;

template<>
void Formatter<int64_t>::format(Builder& builder, int64_t value) const;

template<>
void Formatter<size_t>::format(Builder& builder, size_t value) const; // uint64_t

// Floating point

template<>
void Formatter<float>::format(Builder& builder, float value) const;

template<>
void Formatter<double>::format(Builder& builder, double value) const;

// Char

template<>
void Formatter<char>::format(Builder& builder, char value) const;

template<>
void Formatter<bool>::format(Builder& builder, bool value) const;

// String

template<>
void Formatter<std::string_view>::format(Builder& builder, std::string_view value) const;

template<>
struct Formatter<std::string> : Formatter<std::string_view> {
};

template<>
struct Formatter<const char*> : Formatter<std::string_view> {
	void format(Builder& builder, const char* value) const;
};

template<>
struct Formatter<char*> : Formatter<const char*> {
};

template<size_t N>
struct Formatter<char[N]> : Formatter<const char*> {
};

// Pointer

template<typename T>
struct Formatter<T*> {
	Specifier specifier;

	constexpr void parse(Parser& parser)
	{
		parser.parseSpecifier(specifier, Parser::SpecifierType::Pointer);
	}

	void format(Builder& builder, T* value) const
	{
		value == nullptr
			? builder.putString("0x0")
			: builder.putPointer(static_cast<const void*>(value));
	}
};

template<>
struct Formatter<std::nullptr_t> : Formatter<const void*> {
	void format(Builder& builder, std::nullptr_t) const;
};

// Container

template<typename T>
struct Formatter<std::vector<T>> : Formatter<T> {
	void format(Builder& builder, const std::vector<T>& value) const
	{
		builder.putString("{\n");
		for (auto it = value.cbegin(); it != value.cend(); ++it) {
			builder.putString("    ");
			Formatter<T>::format(builder, *it);

			// Add comma, except after the last element
			if (it != std::prev(value.end(), 1)) {
				builder.putCharacter(',');
			}
			builder.putCharacter('\n');
		}
		builder.putCharacter('}');
	}
};

#define UTIL_FORMAT_FORMAT_AS_MAP(type)                              \
	template<typename K, typename V>                                 \
	struct Formatter<type<K, V>>                                     \
		: Formatter<K>                                               \
		, Formatter<V> {                                             \
		void format(Builder& builder, const type<K, V>& value) const \
		{                                                            \
			builder.putString("{\n");                                \
			auto last = value.end();                                 \
			for (auto it = value.begin(); it != last; ++it) {        \
				builder.putString(R"(    ")");                       \
				Formatter<K>::format(builder, it->first);            \
				builder.putString(R"(": )");                         \
				Formatter<V>::format(builder, it->second);           \
                                                                     \
				/* Add comma, except after the last element */       \
				if (std::next(it) != last) {                         \
					builder.putCharacter(',');                       \
				}                                                    \
                                                                     \
				builder.putCharacter('\n');                          \
			}                                                        \
			builder.putCharacter('}');                               \
		}                                                            \
	}

UTIL_FORMAT_FORMAT_AS_MAP(std::map);
UTIL_FORMAT_FORMAT_AS_MAP(std::unordered_map);
} // namespace Util::Format
