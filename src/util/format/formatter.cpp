/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstddef> // size_t
#include <cstdint> // int32_t, uint32_t, int64_t,
#include <cstring> // strlen
#include <string>
#include <string_view>

#include "util/format/builder.h"
#include "util/format/formatter.h"
#include "util/format/parser.h"

namespace Util::Format {

// Integral

template<>
void Formatter<int32_t>::format(Builder& builder, int32_t value) const
{
	builder.putI32(value);
}

template<>
void Formatter<uint32_t>::format(Builder& builder, uint32_t value) const
{
	builder.putU32(value);
}

template<>
void Formatter<int64_t>::format(Builder& builder, int64_t value) const
{
	builder.putI64(value);
}

template<>
void Formatter<size_t>::format(Builder& builder, size_t value) const
{
	builder.putU64(value);
}

// Floating point

template<>
void Formatter<float>::format(Builder& builder, float value) const
{
	if (specifier.precision < 0) {
		builder.putF32(value);
	}
	else {
		builder.putF32(value, specifier.precision);
	}
}

template<>
void Formatter<double>::format(Builder& builder, double value) const
{
	if (specifier.precision < 0) {
		builder.putF64(value);
		return;
	}

	builder.putF64(value, specifier.precision);
}

// Char

template<>
void Formatter<char>::format(Builder& builder, char value) const
{
	builder.putCharacter(value);
}

template<>
void Formatter<bool>::format(Builder& builder, bool value) const
{
	builder.putString(value ? "true" : "false");
}

// String

template<>
void Formatter<std::string_view>::format(Builder& builder, std::string_view value) const
{
	if (specifier.align == Builder::Align::None) {
		builder.putString(value, specifier.width);
		return;
	}

	builder.putString(value, specifier.width, specifier.align, specifier.fill);
}

void Formatter<const char*>::format(Builder& builder, const char* value) const
{
	Formatter<std::string_view>::format(
		builder,
		value != nullptr ? std::string_view { value, strlen(value) } : "nullptr");
}

// Pointer

void Formatter<std::nullptr_t>::format(Builder& builder, std::nullptr_t) const
{
	Formatter<const void*>::format(builder, 0);
}

} // namespace Util::Format
