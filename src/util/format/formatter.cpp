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
	builder.putString(value, specifier.fill, specifier.align, specifier.width);
}

void Formatter<const char*>::parse(Parser& parser)
{
	parser.parseSpecifier(specifier, Parser::ParameterType::CString);
}

void Formatter<const char*>::format(Builder& builder, const char* value) const
{
	if (specifier.type == PresentationType::Pointer) {
		Formatter<uintptr_t> formatter { .specifier = specifier };
		formatter.specifier.alternativeForm = true;
		formatter.specifier.type = PresentationType::Hex;
		return formatter.format(builder, reinterpret_cast<uintptr_t>(value));
	}

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
