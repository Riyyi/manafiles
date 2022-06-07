/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstddef>
#include <string>

#include "util/json/lexer.h"

namespace Json {

Lexer::Lexer(const std::string& input)
	: m_input(input)
{
}

Lexer::~Lexer()
{
}

// -----------------------------------------

void Lexer::analyze()
{
	printf("---------\n");
	printf("Input JSON:\n%s\n", m_input.c_str());
	printf("---------\n");
	printf("Lexing:\n");

	while (m_index < m_input.length()) {
		switch (peek()) {
		case '{':
			printf("Pushing ->    BraceOpen:  \"{\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens.push_back({ Token::Type::BraceOpen, m_line, m_column, "" });
			break;
		case '}':
			printf("Pushing ->   BraceClose:  \"}\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens.push_back({ Token::Type::BraceClose, m_line, m_column, "" });
			break;
		case '[':
			printf("Pushing ->  BracketOpen:  \"[\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens.push_back({ Token::Type::BracketOpen, m_line, m_column, "" });
			break;
		case ']':
			printf("Pushing -> BracketClose:  \"]\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens.push_back({ Token::Type::BracketClose, m_line, m_column, "" });
			break;
		case ':':
			printf("Pushing ->        Colon:  \":\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens.push_back({ Token::Type::Colon, m_line, m_column, "" });
			break;
		case ',':
			printf("Pushing ->        Comma:  \",\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens.push_back({ Token::Type::Comma, m_line, m_column, "" });
			break;
		case '"':
			if (!getString()) {
				// Error!
				printf("Invalid JSON!\n");
				return;
			}
			break;
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (!getNumber()) {
				// Error!
				printf("Invalid JSON!\n");
				return;
			}
			break;
		case 'f':
		case 'n':
		case 't':
			if (!getLiteral()) {
				// Error!
				printf("Invalid JSON!\n");
				return;
			}
			break;
		case ' ':
		case '\t':
			break;
		case '\r':
			if (peekNext() == '\n') { // CRLF \r\n
				break;
			}
			m_column = 0;
			m_line++;
			break;
		case '\n':
			m_column = 0;
			m_line++;
			break;
		default:
			// Error!
			printf("Invalid JSON!\n");
			return;
			break;
		}

		m_index++;
		m_column++;
	}
}

// -----------------------------------------

char Lexer::peek()
{
	return m_input[m_index];
}

char Lexer::peekNext()
{
	return m_input[m_index + 1];
}

char Lexer::consume()
{
	char character = peek();
	m_index++;
	m_column++;
	return character;
}

bool Lexer::consumeSpecific(char character)
{
	if (peek() != character) {
		return false;
	}

	m_index++;
	m_column++;
	return true;
}

bool Lexer::getString()
{
	size_t column = m_column;
	std::string symbol = "";

	char character = consume();
	for (;;) {
		character = peek();

		// TODO: Escape logic goes here
		// ", \, /, b(ackspace), f(orm feed), l(ine feed), c(arriage return), t(ab), u(nicode) \u0021

		if (character == '"') {
			break;
		}

		m_index++;
		m_column++;
		symbol += character;
	}

	printf("Pushing ->       String:  \"%s\"\t%zu[%zu]\n", symbol.c_str(), m_line, column);
	m_tokens.push_back({ Token::Type::String, m_line, column, symbol });

	return true;
}

bool Lexer::getNumber()
{
	size_t index = m_index;
	size_t column = m_column;
	std::string symbol = "";

	bool seenDot = false;
	char character;
	for (;;) {
		character = peek();

		// FIXME: Break on separator }], rather than valid number symbols to
		//        get the entire thing, resulting in better error handling
		// FIXME: e/E and exponent are also valid characters (?)
		if (character != 45                          // -
		    && character != 46                       // .
		    && (character < 48 || character > 57)) { // 0-9
			break;
		}

		// Fail if '.' is used more than once
		if (seenDot == true && character == 46) { // .
			m_index = index;
			m_column = column;
			return false;
		}
		if (character == 46) { // .
			seenDot = true;
		}

		m_index++;
		m_column++;
		symbol += character;
	}
	m_index--;
	m_column--;

	printf("Pushing ->       Number:  \"%s\"\t%zu[%zu]\n", symbol.c_str(), m_line, column);
	m_tokens.push_back({ Token::Type::Number, m_line, column, symbol });

	return true;
}

bool Lexer::getLiteral()
{
	size_t index = m_index;
	size_t column = m_column;

	std::string symbol = "";

	char character;
	for (;;) {
		character = peek();

		// Literals can only contain lower-case letters
		if (character < 97 || character > 122) { // a-z
			break;
		}

		m_index++;
		m_column++;
		symbol += character;
	}
	m_index--;
	m_column--;

	// Literal name validation
	if (symbol != "false" && symbol != "null" && symbol != "true") {
		m_index = index;
		m_column = column;
		return false;
	}

	printf("Pushing ->      Literal:  \"%s\"\t%zu[%zu]\n", symbol.c_str(), m_line, column);
	m_tokens.push_back({ Token::Type::Literal, m_line, column, symbol });

	return true;
}

} // namespace Json
