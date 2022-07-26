/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // replace
#include <cstddef>   // size_t
#include <string>
#include <string_view>

#include "util/format/builder.h"
#include "util/format/parser.h"
#include "util/meta/assert.h"

namespace Util::Format {

Parser::Parser(std::string_view format, size_t parameterCount)
	: GenericLexer(format)
	, m_parameterCount(parameterCount)
{
	checkFormatParameterConsistency();
}

Parser::~Parser()
{
}

// -----------------------------------------

void Parser::checkFormatParameterConsistency()
{
	size_t length = m_input.length();

	VERIFY(length >= m_parameterCount * 2, "format string does not reference all parameters");

	size_t braceOpen = 0;
	size_t braceClose = 0;
	while (!isEOF()) {
		char peek0 = peek();
		char peek1 = peek(1);

		if (peek0 == '{' && peek1 == '{') {
			ignore(2);
			continue;
		}

		if (peek0 == '}' && peek1 == '}') {
			ignore(2);
			continue;
		}

		if (peek0 == '{') {
			braceOpen++;

			VERIFY(peek1 == '}' || peek1 == ':', "invalid parameter reference");
		}
		if (peek0 == '}') {
			braceClose++;
		}

		ignore();
	}
	m_index = 0;

	VERIFY(!(braceOpen < braceClose), "extra open braces in format string");

	VERIFY(!(braceOpen > braceClose), "extra closing braces in format string");

	VERIFY(!(braceOpen < m_parameterCount), "format string does not reference all passed parameters");

	VERIFY(!(braceOpen > m_parameterCount), "format string references nonexistent parameter");
}

std::string_view Parser::consumeLiteral()
{
	const auto begin = tell();

	while (!isEOF()) {
		char peek0 = peek();
		char peek1 = peek(1);

		if (peek0 == '{' && peek1 == '{') {
			ignore(2);
			continue;
		}

		if (peek0 == '}' && peek1 == '}') {
			ignore(2);
			continue;
		}

		// Get literal before the specifier {}
		if (peek0 == '{' || peek0 == '}') {
			return m_input.substr(begin, tell() - begin);
		}

		ignore();
	}

	return m_input.substr(begin);
}

bool Parser::consumeSpecifier(std::string_view& specifier)
{
	if (!consumeSpecific('{')) {
		return false;
	}

	if (!consumeSpecific(':')) {
		VERIFY(consumeSpecific('}'), "invalid parameter reference");
		specifier = "";
	}
	else {
		const auto begin = tell();

		// Go until AFTER the closing brace
		while (peek(-1) != '}') {
			consume();
		}

		specifier = m_input.substr(begin, tell() - begin - 1);
	}

	return true;
}

void Parser::applySpecifier(Builder& builder, std::string_view specifier)
{
	if (specifier[0] == '.') {
		size_t value = 0;

		for (size_t i = 1; i < specifier.length(); ++i) {
			if (specifier[i] < '0' || specifier[i] > '9') {
				return;
			}
			value *= 10;
			value += specifier[i] - '0'; // Subtract ASCII 48 to get the number
		}

		builder.setPrecision(value);
	}
}

} // namespace Util::Format
