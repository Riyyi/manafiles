/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_FORMAT_PARSER_H
#define UTIL_FORMAT_PARSER_H

#include <cstddef>   // size_t
#include <string_view>

#include "util/genericlexer.h"

namespace Util::Format {

class Builder;

class Parser final : public GenericLexer {
public:
	Parser(std::string_view format, size_t parameterCount);
	virtual ~Parser();

	void checkFormatParameterConsistency();

	std::string_view consumeLiteral();
	bool consumeSpecifier(std::string_view& specifier);

	void applySpecifier(Builder& builder, std::string_view specifier);

private:
	size_t m_parameterCount { 0 };
};

} // namespace Util::Format

#endif // UTIL_FORMAT_PARSER_H
