/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cassert>
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
#include "util/meta/concepts.h"

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
	size_t width = 0;
	int8_t precision = -1;

	PresentationType type = PresentationType::None;
};

template<typename T>
struct Formatter {
	Specifier specifier;

	constexpr void parse(Parser& parser)
	{
		if (std::is_same_v<T, char>) {
			parser.parseSpecifier(specifier, Parser::ParameterType::Char);
		}
		else if (std::is_same_v<T, std::string_view>) {
			parser.parseSpecifier(specifier, Parser::ParameterType::String);
		}
	}

	void format(Builder&, T) const {}
};

// Integral

template<Integral T>
struct Formatter<T> {
	Specifier specifier;

	void parse(Parser& parser)
	{
		parser.parseSpecifier(specifier, Parser::ParameterType::Integral);
	}

	void format(Builder& builder, T value) const
	{
		if (specifier.type == PresentationType::Character) {
			assert(value >= 0 && value <= 127);

			Formatter<std::string_view> formatter { specifier };
			formatter.specifier.type = PresentationType::String;
			return formatter.format(builder, { reinterpret_cast<const char*>(&value), 1 });
		}

		uint8_t base = 0;
		bool uppercase = false;
		switch (specifier.type) {
		case PresentationType::BinaryUppercase:
			uppercase = true;
		case PresentationType::Binary:
			base = 2;
			break;
		case PresentationType::Octal:
			base = 8;
			break;
		case PresentationType::None:
		case PresentationType::Decimal:
			base = 10;
			break;
		case PresentationType::HexUppercase:
			uppercase = true;
		case PresentationType::Hex:
			base = 16;
			break;
		default:
			assert(false);
		};

		if constexpr (std::is_unsigned_v<T>) {
			builder.putU64(
				value, base, uppercase, specifier.fill, specifier.align, specifier.sign,
				specifier.alternativeForm, specifier.zeroPadding, specifier.width);
		}
		else {
			builder.putI64(
				value, base, uppercase, specifier.fill, specifier.align, specifier.sign,
				specifier.alternativeForm, specifier.zeroPadding, specifier.width);
		}
	}
};

// Floating point

template<FloatingPoint T>
struct Formatter<T> {
	Specifier specifier;

	void parse(Parser& parser)
	{
		parser.parseSpecifier(specifier, Parser::ParameterType::FloatingPoint);
	}

	void format(Builder& builder, T value) const
	{
		if (specifier.precision < 0) {
			builder.putF64(value);
			return;
		}

		builder.putF64(value, specifier.precision);
	}
};

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
	void parse(Parser& parser);
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
		parser.parseSpecifier(specifier, Parser::ParameterType::Pointer);
	}

	void format(Builder& builder, T* value) const
	{
		value == nullptr
			? builder.putString("nullptr")
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
