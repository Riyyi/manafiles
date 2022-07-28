/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // count
#include <cstddef>   // size_t
#include <cstdint>   // uint8_t
#include <cstdio>    // printf
#include <map>
#include <string> // stod

#include "util/json/array.h"
#include "util/json/job.h"
#include "util/json/lexer.h"
#include "util/json/object.h"
#include "util/json/parser.h"
#include "util/json/value.h"
#include "util/meta/assert.h"

namespace Util::JSON {

Parser::Parser(Job* job)
	: m_job(job)
	, m_tokens(m_job->tokens())
{
}

Parser::~Parser()
{
}

// -----------------------------------------

Value Parser::parse()
{
	Value result;

	if (m_tokens->size() == 0) {
		m_job->printErrorLine({}, "expecting token, not 'EOF'");
		return result;
	}

	Token token = peek();
	switch (token.type) {
	case Token::Type::Literal:
		result = consumeLiteral();
		break;
	case Token::Type::Number:
		result = consumeNumber();
		break;
	case Token::Type::String:
		result = consumeString();
		break;
	case Token::Type::BracketOpen:
		result = consumeArray();
		break;
	case Token::Type::BraceOpen:
		result = consumeObject();
		break;
	case Token::Type::BracketClose:
		m_job->printErrorLine(token, "expecting value, not ']'");
		m_index++;
		break;
	case Token::Type::BraceClose:
		m_job->printErrorLine(token, "expecting string, not '}'");
		m_index++;
		break;
	default:
		m_job->printErrorLine(token, "multiple root elements");
		m_index++;
		break;
	}

	if (!isEOF()) {
		m_job->printErrorLine(peek(), "multiple root elements");
	}

	return result;
}

// -----------------------------------------

bool Parser::isEOF()
{
	return m_index >= m_tokens->size();
}

Token Parser::peek()
{
	VERIFY(!isEOF());
	return (*m_tokens)[m_index];
}

Token Parser::consume()
{
	VERIFY(!isEOF());
	return (*m_tokens)[m_index++];
}

void Parser::ignoreUntil(Token::Type type)
{
	while (!isEOF() && peek().type != type) {
		++m_index;
	}
}

Value Parser::consumeLiteral()
{
	Token token = consume();

	if (token.symbol == "null") {
		return nullptr;
	}
	else if (token.symbol == "true") {
		return true;
	}
	else if (token.symbol == "false") {
		return false;
	}

	m_job->printErrorLine(token, "invalid literal");
	return nullptr;
}

Value Parser::consumeNumber()
{
	Token token = consume();

	auto reportError = [this](Token token, const std::string& message) -> void {
		m_job->printErrorLine(token, message.c_str());
	};

	// Validation
	// number = [ minus ] int [ frac ] [ exp ]

	size_t minusPrefix = token.symbol[0] == '-' ? 1 : 0;

	// Leading 0s
	if (token.symbol[minusPrefix] == '0'
	    && token.symbol[minusPrefix + 1] > '0' && token.symbol[minusPrefix + 1] < '9') {
		reportError(token, "invalid leading zero");
		return nullptr;
	}

	enum class State : uint8_t {
		Int,
		Fraction,
		Exponent
	};

	State state = State::Int;

#define CHECK_IF_VALID_NUMBER                                                                  \
	if (character < 48 || character > 57) {                                                    \
		reportError(token, std::string() + "invalid number, unexpected '" + character + '\''); \
		return nullptr;                                                                        \
	}

	size_t fractionPosition = 0;
	size_t exponentPosition = 0;
	size_t length = token.symbol.length();
	for (size_t i = 0; i < length; ++i) {
		char character = token.symbol[i];

		// Int -> Fraction
		if (character == '.' && state == State::Int) {
			state = State::Fraction;
			fractionPosition = i;
			continue;
		}
		// Int/Fraction -> Exponent
		else if ((character == 'e' || character == 'E') && state != State::Exponent) {
			state = State::Exponent;
			exponentPosition = i;
			continue;
		}

		if (state == State::Int) {
			if (character == '-') {
				if (i == length - 1) {
					reportError(token, "expected number after minus");
					return nullptr;
				}
				if (i != 0) {
					reportError(token, "invalid minus");
					return nullptr;
				}
			}
			else {
				CHECK_IF_VALID_NUMBER;
			}
		}
		else if (state == State::Fraction) {
			CHECK_IF_VALID_NUMBER;
		}
		else if (state == State::Exponent) {
			if (character == '-' || character == '+') {
				if (i == length - 1) {
					reportError(token, "expected number after plus/minus");
					return nullptr;
				}
				if (i > exponentPosition + 1) {
					reportError(token, "invalid plus/minus");
					return nullptr;
				}
			}
			else {
				CHECK_IF_VALID_NUMBER;
			}
		}
	}

	if (fractionPosition != 0 || exponentPosition != 0) {
		if (fractionPosition == exponentPosition - 1) {
			reportError(token, "invalid exponent sign, expected number");
			return nullptr;
		}

		if (fractionPosition == length - 1 || exponentPosition == length - 1) {
			reportError(token, "invalid number");
			return nullptr;
		}
	}

	return std::stod(token.symbol);
}

Value Parser::consumeString()
{
	Token token = consume();

	auto reportError = [this](Token token, const std::string& message) -> void {
		m_job->printErrorLine(token, message.c_str());
	};

	// FIXME: support \u Unicode character escape sequence
	auto getPrintableString = [](char character) -> std::string {
		if (character == '"' || character == '\\' || character == '/'
		    || (character >= 0 && character <= 31)) {
			switch (character) {
			case '"':
				return "\\\"";
				break;
			case '\\':
				return "\\\\";
				break;
			case '/':
				return "/";
				break;
			case '\b':
				return "\\b";
				break;
			case '\f':
				return "\\f";
				break;
			case '\n':
				return "\\n";
				break;
			case '\r':
				return "\\r";
				break;
			case '\t':
				return "\\t";
				break;
			default:
				char buffer[7];
				sprintf(buffer, "\\u%0.4X", character);
				return std::string(buffer);
				break;
			}
		}

		return std::string() + character;
	};

	std::string string;

	bool escape = false;
	for (char character : token.symbol) {
		if (!escape) {
			if (character == '\\') {
				escape = true;
				continue;
			}

			if (character == '"' || (character >= 0 && character <= 31)) {
				reportError(token, "invalid string, unescaped character found");
				return nullptr;
			}
		}

		string += getPrintableString(character);

		if (escape) {
			escape = false;
		}
	}

	return string;
}

Value Parser::consumeArray()
{
	m_index++;

	auto reportError = [this](Token token, const std::string& message) -> void {
		m_job->printErrorLine(token, message.c_str());

		// After an error, try to find the closing bracket
		ignoreUntil(Token::Type::BracketClose);
		m_index++;
	};

	Value array = Value::Type::Array;
	Token token;
	for (;;) {
		// EOF
		if (isEOF()) {
			reportError(m_tokens->at(m_index - 1), "expecting closing ']' at end");
			break;
		}

		token = peek();
		if (token.type == Token::Type::Literal) {
			array.emplace_back(consumeLiteral());
		}
		else if (token.type == Token::Type::Number) {
			array.emplace_back(consumeNumber());
		}
		else if (token.type == Token::Type::String) {
			array.emplace_back(consumeString());
		}
		else if (token.type == Token::Type::BracketOpen) {
			array.emplace_back(consumeArray());
		}
		else if (token.type == Token::Type::BraceOpen) {
			array.emplace_back(consumeObject());
		}
		else if (token.type == Token::Type::BracketClose) {
			// Trailing comma
			if (array.m_value.array->size() > 0) {
				reportError(m_tokens->at(m_index - 1), "invalid comma, expecting ']'");
				break;
			}
		}
		else {
			reportError(token, "expecting value or ']', not '" + token.symbol + "'");
			break;
		}

		// EOF
		if (isEOF()) {
			reportError(token, "expecting closing ']' at end");
			break;
		}

		// Find , or ]
		token = consume();
		if (token.type == Token::Type::Comma) {
			continue;
		}
		else if (token.type == Token::Type::BracketClose) {
			break;
		}
		else {
			reportError(m_tokens->at(m_index - 1), "expecting comma or ']', not '" + token.symbol + "'");
			break;
		}
	}

	return array;
}

Value Parser::consumeObject()
{
	m_index++;

	auto reportError = [this](Token token, const std::string& message) -> void {
		m_job->printErrorLine(token, message.c_str());

		// After an error, try to find the closing brace
		ignoreUntil(Token::Type::BraceClose);
		m_index++;
	};

	Value object = Value::Type::Object;
	Token token;
	std::string name;
	std::map<std::string, uint8_t> unique;
	for (;;) {
		// EOF
		if (isEOF()) {
			reportError(m_tokens->at(m_index - 1), "expecting closing '}' at end");
			break;
		}

		token = consume();
		if (token.type == Token::Type::BraceClose) {
			// Trailing comma
			if (object.m_value.object->size() > 0) {
				reportError(m_tokens->at(m_index - 1), "invalid comma, expecting '}'");
			}
			// Empty object
			break;
		}
		if (token.type != Token::Type::String) {
			reportError(token, "expecting string or '}', not '" + token.symbol + "'");
			break;
		}

		// Find member name
		m_index--;
		Value tmpName = consumeString();
		if (tmpName.m_type != Value::Type::String) {
			ignoreUntil(Token::Type::BraceClose);
			m_index++;
			break;
		}

		// Check if name exists in hashmap
		name = *tmpName.m_value.string;
		if (unique.find(name) != unique.end()) {
			reportError(token, "duplicate name '" + token.symbol + "', names should be unique");
			break;
		}
		// Add name to hashmap
		unique.insert({ name, 0 });

		// EOF
		if (isEOF()) {
			reportError(token, "expecting colon, not 'EOF'");
			reportError(token, "expecting closing '}' at end");
			break;
		}

		// Find :
		token = consume();
		if (token.type != Token::Type::Colon) {
			reportError(token, "expecting colon, not '" + token.symbol + "'");
			break;
		}

		// EOF
		if (isEOF()) {
			reportError(token, "expecting value, not 'EOF'");
			reportError(token, "expecting closing '}' at end");
			break;
		}

		// Add member (name:value pair) to object
		token = peek();
		if (token.type == Token::Type::Literal) {
			object.emplace(name, consumeLiteral());
		}
		else if (token.type == Token::Type::Number) {
			object.emplace(name, consumeNumber());
		}
		else if (token.type == Token::Type::String) {
			object.emplace(name, consumeString());
		}
		else if (token.type == Token::Type::BracketOpen) {
			object.emplace(name, consumeArray());
		}
		else if (token.type == Token::Type::BraceOpen) {
			object.emplace(name, consumeObject());
		}
		else {
			reportError(token, "expecting value, not '" + token.symbol + "'");
			break;
		}

		// EOF
		if (isEOF()) {
			reportError(token, "expecting closing '}' at end");
			break;
		}

		// Find , or }
		token = consume();
		if (token.type == Token::Type::Comma) {
			continue;
		}
		else if (token.type == Token::Type::BraceClose) {
			break;
		}
		else {
			reportError(token, "expecting comma or '}', not '" + token.symbol + "'");
			break;
		}
	}

	return object;
}

} // namespace Util::JSON
