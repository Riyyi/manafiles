/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // replace
#include <cstddef>   // size_t
#include <limits>    // numeric_limits
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

			if (peek1 >= '0' && peek1 <= '9') {
				m_mode = ArgumentIndexingMode::Manual;
			}
		}
		if (peek0 == '}') {
			braceClose++;
		}

		ignore();
	}
	m_index = 0;

	// VERIFY(!(braceOpen < braceClose), "extra open braces in format string");

	// VERIFY(!(braceOpen > braceClose), "extra closing braces in format string");

	// VERIFY(!(braceOpen < m_parameterCount), "format string does not reference all passed parameters");

	// VERIFY(!(braceOpen > m_parameterCount), "format string references nonexistent parameter");
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

std::optional<size_t> Parser::consumeIndex()
{
	if (!consumeSpecific('{')) {
		VERIFY_NOT_REACHED();
		return {};
	}

	switch (m_mode) {
	case ArgumentIndexingMode::Automatic: {
		VERIFY(consumeSpecific(':') || peek() == '}', "expecting ':' or '}', not '%c'", peek());
		return {};
	}
	case ArgumentIndexingMode::Manual: {
		const auto begin = tell();

		while (!isEOF()) {
			char peek0 = peek();
			if (peek0 == '}' || peek0 == ':') {
				break;
			}

			VERIFY(peek0 >= '0' && peek0 <= '9', "expecting number, not '%c'", peek0);

			ignore();
		}

		size_t result = stringToNumber(m_input.substr(begin, tell() - begin));

		if (peek() == ':') {
			ignore();
		}

		return result;
	}
	};

	VERIFY_NOT_REACHED();
}

size_t Parser::stringToNumber(std::string_view value)
{
	size_t result = 0;

	for (size_t i = 1; i < value.length(); ++i) {
		if (value[i] < '0' || value[i] > '9') {
			VERIFY_NOT_REACHED();
		}
		result *= 10;
		result += value[i] - '0'; // Subtract ASCII 48 to get the number
	}

	return result;
}

			}
		}

	}
}

} // namespace Util::Format
