/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstddef>
#include <string>

#include "util/json/job.h"
#include "util/json/lexer.h"

namespace Util::JSON {

Lexer::Lexer(Job* job)
	: GenericLexer(job->input())
	, m_job(job)
	, m_tokens(job->tokens())
{
}

Lexer::~Lexer()
{
}

// -----------------------------------------

void Lexer::analyze()
{
	while (m_index < m_input.length()) {
		switch (peek()) {
		case '{':
			m_tokens->push_back({ Token::Type::BraceOpen, m_line, m_column, "{" });
			break;
		case '}':
			m_tokens->push_back({ Token::Type::BraceClose, m_line, m_column, "}" });
			break;
		case '[':
			m_tokens->push_back({ Token::Type::BracketOpen, m_line, m_column, "[" });
			break;
		case ']':
			m_tokens->push_back({ Token::Type::BracketClose, m_line, m_column, "]" });
			break;
		case ':':
			m_tokens->push_back({ Token::Type::Colon, m_line, m_column, ":" });
			break;
		case ',':
			m_tokens->push_back({ Token::Type::Comma, m_line, m_column, "," });
			break;
		case '"':
			if (!consumeString()) {
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
			if (!consumeNumber()) {
				return;
			}
			break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
			if (!consumeLiteral()) {
				return;
			}
			break;
		case ' ':
		case '\t':
			break;
		case '\r':
			if (peek(1) == '\n') { // CRLF \r\n
				break;
			}
			m_column = -1;
			m_line++;
			break;
		case '\n':
			m_column = -1;
			m_line++;
			break;
		default:
			// Error!
			m_tokens->push_back({ Token::Type::None, m_line, m_column, std::string(1, peek()) });
			m_job->printErrorLine(m_tokens->back(),
			                      (std::string() + "unexpected character '" + peek() + "'").c_str());
			return;
			break;
		}

		ignore();
		m_column++;
	}
}

// -----------------------------------------

bool Lexer::consumeString()
{
	size_t column = m_column;
	std::string symbol = "";

	bool escape = false;
	char character = consume();
	for (;;) {
		character = peek();

		if (!escape && character == '\\') {
			symbol += '\\';
			ignore();
			escape = true;
			continue;
		}

		if (!escape
		    && (character == '"'
		        || character == '\r'
		        || character == '\n'
		        || character == '\0')) {
			break;
		}

		symbol += character;
		ignore();

		if (escape) {
			escape = false;
		}
	}

	m_tokens->push_back({ Token::Type::String, m_line, column, symbol });

	if (character != '"') {
		m_job->printErrorLine(m_job->tokens()->back(), "strings should be wrapped in double quotes");
		return false;
	}

	return true;
}

bool Lexer::consumeNumberOrLiteral(Token::Type type)
{
	size_t index = m_index;
	size_t column = m_column;

	for (char character;;) {
		character = peek();

		if (character == '{'
		    || character == '}'
		    || character == '['
		    || character == ']'
		    || character == ':'
		    || character == ','
		    || character == '"'
		    || character == ' '
		    || character == '\t'
		    || character == '\r'
		    || character == '\n'
		    || character == '\0') {
			break;
		}

		ignore();
	}

	m_tokens->push_back({ type, m_line, column,
	                      std::string(m_input.substr(index, m_index - index)) });

	retreat();

	return true;
}

bool Lexer::consumeNumber()
{
	return consumeNumberOrLiteral(Token::Type::Number);
}

bool Lexer::consumeLiteral()
{
	return consumeNumberOrLiteral(Token::Type::Literal);
}

} // namespace Util::JSON
