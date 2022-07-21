/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // replace
#include <cassert>   // assert
#include <cstddef>   // size_t
#include <string>
#include <string_view>

#include "util/format/builder.h"
#include "util/format/parser.h"

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

	// Format string does not reference all passed parameters
	assert(length >= m_parameterCount * 2);

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

			// Only empty references {} or specified references {:} are valid
			assert(peek1 == '}' || peek1 == ':');
		}
		if (peek0 == '}') {
			braceClose++;
		}

		ignore();
	}
	m_index = 0;

	// Extra open braces in format string
	assert(!(braceOpen < braceClose));

	// Extra closing braces in format string
	assert(!(braceOpen > braceClose));

	// Format string does not reference all passed parameters
	assert(!(braceOpen < m_parameterCount));

	// Format string references nonexistent parameter
	assert(!(braceOpen > m_parameterCount));
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
		assert(consumeSpecific('}'));
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
