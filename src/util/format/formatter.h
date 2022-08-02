/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <cstdint> // int32_t, uint8_t, uint32_t, int64_t,
#include <string>
#include <string_view>
#include <vector>

#include "util/format/builder.h"
#include "util/format/parser.h"

namespace Util::Format {


template<typename T>
struct Formatter {

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
