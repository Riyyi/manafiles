/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>  // nullptr_t, size_t
#include <cstdint>  // int32_t, uint32_t, int64_t
#include <cstring>  // strlen
#include <iterator> // next
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility> // forward
#include <vector>

#include "util/meta/odr.h"

namespace Util::Format {

namespace Detail {

template<typename FormatBuilder>
void format(FormatBuilder& builder, std::nullptr_t)
{
	builder.putString("(nil)");
}

template<typename FormatBuilder, typename T>
void format(FormatBuilder& builder, T* pointer)
{
	builder.putPointer(static_cast<const void*>(pointer));
}

template<typename FormatBuilder>
void format(FormatBuilder& builder, bool boolean)
{
	builder.putString(boolean ? "true" : "false");
}

template<typename FormatBuilder>
void format(FormatBuilder& builder, int32_t number) // int
{
	builder.putI32(number);
}

template<typename FormatBuilder>
void format(FormatBuilder& builder, uint32_t number) // unsigned int
{
	builder.putU32(number);
}

template<typename FormatBuilder>
void format(FormatBuilder& builder, int64_t number) // long int
{
	builder.putI64(number);
}

template<typename FormatBuilder>                   // uint64_t
void format(FormatBuilder& builder, size_t number) // long unsigned int
{
	builder.putU64(number);
}

template<typename FormatBuilder>
void format(FormatBuilder& builder, float number)
{
	builder.putF32(number);
}

template<typename FormatBuilder>
void format(FormatBuilder& builder, double number)
{
	builder.putF64(number);
}

template<typename FormatBuilder>
void format(FormatBuilder& builder, char character)
{
	builder.putCharacter(character);
}

template<typename FormatBuilder>
void format(FormatBuilder& builder, const char* string)
{
	builder.putString({ string, strlen(string) });
}

template<typename FormatBuilder>
void format(FormatBuilder& builder, const std::string& string)
{
	builder.putString(string);
}

template<typename FormatBuilder>
void format(FormatBuilder& builder, std::string_view string)
{
	builder.putString(string);
}

template<typename FormatBuilder, typename T>
void format(FormatBuilder& builder, const std::vector<T>& array)
{
	builder.putString("{\n");
	for (auto it = array.cbegin(); it != array.cend(); ++it) {
		builder.putString("    ");
		format(builder, *it);

		// Add comma, except after the last element
		if (it != std::prev(array.end(), 1)) {
			builder.putCharacter(',');
		}
		builder.putCharacter('\n');
	}
	builder.putCharacter('}');
}

#define FORMAT_MAP                                     \
	builder.putString("{\n");                          \
	auto last = map.end();                             \
	for (auto it = map.begin(); it != last; ++it) {    \
		builder.putString(R"(    ")");                 \
		format(builder, it->first);                    \
		builder.putString(R"(": )");                   \
		format(builder, it->second);                   \
                                                       \
		/* Add comma, except after the last element */ \
		if (std::next(it) != last) {                   \
			builder.putCharacter(',');                 \
		}                                              \
                                                       \
		builder.putCharacter('\n');                    \
	}                                                  \
	builder.putCharacter('}');

template<typename FormatBuilder, typename K, typename V>
void format(FormatBuilder& builder, const std::map<K, V>& map)
{
	FORMAT_MAP;
}

template<typename FormatBuilder, typename K, typename V>
void format(FormatBuilder& builder, const std::unordered_map<K, V>& map)
{
	FORMAT_MAP;
}

struct formatFunction {
	template<typename FormatBuilder, typename T>
	auto operator()(FormatBuilder& builder, T&& value) const
	{
		return format(builder, std::forward<T>(value));
	}
};

} // namespace Detail

namespace {
constexpr const auto& format = Util::Detail::staticConst<Detail::formatFunction>; // NOLINT(misc-definitions-in-headers,clang-diagnostic-unused-variable)
} // namespace

} // namespace Util::Format
