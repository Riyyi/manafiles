/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <optional>
#include <string_view>

#include "util/genericlexer.h"

namespace Util::Format {

class Builder;
struct Specifier;

class Parser final : public GenericLexer {
public:
	enum class ArgumentIndexingMode {
		Automatic, // {} ,{}
		Manual,    // {0},{1}
	};

	enum class ParameterType {
		Integral,
		FloatingPoint,
		Char,
		CString,
		String,
		Pointer,
		Container,
	};

	Parser(std::string_view format, size_t parameterCount);
	virtual ~Parser();

	void checkFormatParameterConsistency();
	size_t stringToNumber(std::string_view value);

	std::string_view consumeLiteral();
	std::optional<size_t> consumeIndex();

	void parseSpecifier(Specifier& specifier, ParameterType type);
	constexpr void checkSpecifierIntegralType(const Specifier& specifier);
	constexpr void checkSpecifierFloatingPointType(const Specifier& specifier);
	constexpr void checkSpecifierCharType(const Specifier& specifier);
	constexpr void checkSpecifierCStringType(const Specifier& specifier);
	constexpr void checkSpecifierStringType(const Specifier& specifier);
	constexpr void checkSpecifierPointerType(const Specifier& specifier);
	constexpr void checkSpecifierContainerType(const Specifier& specifier);
	constexpr void checkSpecifierType(const Specifier& specifier, ParameterType type);

private:
	ArgumentIndexingMode m_mode { ArgumentIndexingMode::Automatic };
	size_t m_parameterCount { 0 };
};

} // namespace Util::Format
