/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // replace
#include <cstddef>   // size_t
#include <cstdint>   // int8_t
#include <limits>    // numeric_limits
#include <string>
#include <string_view>

#include "util/format/builder.h"
#include "util/format/formatter.h"
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

size_t Parser::stringToNumber(std::string_view value)
{
	size_t result = 0;

	for (size_t i = 0; i < value.length(); ++i) {
		VERIFY(value[i] >= '0' && value[i] <= '9', "unexpected '%c'", value[i]);
		result *= 10;
		result += value[i] - '0'; // Subtract ASCII 48 to get the number
	}

	return result;
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

void Parser::parseSpecifier(Specifier& specifier, SpecifierType type)
{
	if (consumeSpecific('}') || isEOF()) {
		return;
	}

	// Alignment
	char peek0 = peek();
	char peek1 = peek(1);
	if (peek1 == '<' || peek1 == '>' || peek1 == '^') {
		specifier.fill = peek0;
		specifier.align = static_cast<Builder::Align>(peek1);
		ignore(2);
	}

	enum State {
		AfterAlign,
		AfterSign,
		AfterAlternativeForm,
		AfterZeroPadding,
		AfterWidth,
		AfterDot,
		AfterPrecision,
		AfterType,
	} state { State::AfterAlign };

	size_t widthBegin = std::numeric_limits<size_t>::max();
	size_t precisionBegin = std::numeric_limits<size_t>::max();
	size_t widthEnd = 0;
	size_t precisionEnd = 0;
	std::string_view acceptedTypes = "bdoxaefgscpBXAFG";

	while (true) {
		char peek0 = peek();

		if (peek0 == '}') {
			ignore();
			break;
		}
		if (isEOF()) {
			break;
		}

		// Sign is only valid for numeric types
		if (peek0 == '+' || peek0 == '-' || peek0 == ' ') {
			VERIFY(state < State::AfterSign, "unexpected '%c' at this position", peek0);
			VERIFY(type == SpecifierType::Integral || type == SpecifierType::FloatingPoint,
			       "sign option is only valid for numeric types");
			state = State::AfterSign;
			specifier.sign = static_cast<Builder::Sign>(peek0);
		}

		// Alternative form is only valid for numeric types
		if (peek0 == '#') {
			VERIFY(state < State::AfterAlternativeForm, "unexpected '#' at this position");
			VERIFY(type == SpecifierType::Integral || type == SpecifierType::FloatingPoint,
			       "'#' option is only valid for numeric types");
			state = State::AfterAlternativeForm;
			specifier.alternativeForm = true;
		}

		// Sign aware zero padding is only valid for numeric types
		if (peek0 == '0') {
			if (state < State::AfterWidth) {
				VERIFY(state < State::AfterZeroPadding, "unexpected '0' at this position");
				VERIFY(type == SpecifierType::Integral || type == SpecifierType::FloatingPoint,
				       "zero padding option is only valid for numeric types");
				state = State::AfterZeroPadding;
				specifier.zeroPadding = true;
			}
		}

		if (peek0 >= '0' && peek0 <= '9') {
			if (widthBegin == std::numeric_limits<size_t>::max() && state < State::AfterDot) {
				VERIFY(state < State::AfterWidth, "unexpected '%c' at this position", peek0);
				state = State::AfterWidth;
				widthBegin = tell();
			}
			if (precisionBegin == std::numeric_limits<size_t>::max() && state == State::AfterDot) {
				state = State::AfterPrecision;
				precisionBegin = tell();
			}
		}

		if (peek0 == '.') {
			if (state == State::AfterWidth) {
				widthEnd = tell();
			}

			VERIFY(state < State::AfterDot, "unexpected '.' at this position");
			state = State::AfterDot;
		}

		if ((peek0 >= 'a' && peek0 <= 'z')
		    || (peek0 >= 'A' && peek0 <= 'Z')) {
			if (state == State::AfterWidth) {
				widthEnd = tell();
			}
			if (state == State::AfterPrecision) {
				precisionEnd = tell();
			}

			VERIFY(state < State::AfterType, "unexpected '%c' at this position", peek0);
			state = State::AfterType;
			VERIFY(acceptedTypes.find(peek0) != std::string_view::npos, "unexpected '%c' at this position", peek0);
			specifier.type = static_cast<PresentationType>(peek0);
		}

		ignore();
	}

	if (widthBegin != std::numeric_limits<size_t>::max()) {
		if (widthEnd == 0) {
			// We parse until after the closing '}', so take this into account
			widthEnd = tell() - 1;
		}
		specifier.width = static_cast<int>(stringToNumber(m_input.substr(widthBegin, widthEnd - widthBegin)));
	}

	if (precisionBegin != std::numeric_limits<size_t>::max()) {
		if (precisionEnd == 0) {
			// We parse until after the closing '}', so take this into account
			precisionEnd = tell() - 1;
		}
		specifier.precision = static_cast<int8_t>(stringToNumber(m_input.substr(precisionBegin, precisionEnd - precisionBegin)));
	}
}

} // namespace Util::Format
