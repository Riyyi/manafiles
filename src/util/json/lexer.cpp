/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstddef>
#include <string>

#include "util/json/job.h"
#include "util/json/lexer.h"

namespace Json {

Lexer::Lexer(Job* job)
	: m_job(job)
	, m_tokens(job->tokens())
{
}

Lexer::~Lexer()
{
}

// -----------------------------------------

void Lexer::analyze()
{
	printf("---------\n");
	printf("Input JSON:\n%s\n", m_job->input().c_str());
	printf("---------\n");
	printf("Lexing:\n");

	while (m_index < m_job->input().length()) {
		switch (peek()) {
		case '{':
			printf("Pushing ->    BraceOpen:  \"{\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens->push_back({ Token::Type::BraceOpen, m_line, m_column, "{" });
			break;
		case '}':
			printf("Pushing ->   BraceClose:  \"}\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens->push_back({ Token::Type::BraceClose, m_line, m_column, "}" });
			break;
		case '[':
			printf("Pushing ->  BracketOpen:  \"[\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens->push_back({ Token::Type::BracketOpen, m_line, m_column, "[" });
			break;
		case ']':
			printf("Pushing -> BracketClose:  \"]\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens->push_back({ Token::Type::BracketClose, m_line, m_column, "]" });
			break;
		case ':':
			printf("Pushing ->        Colon:  \":\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens->push_back({ Token::Type::Colon, m_line, m_column, ":" });
			break;
		case ',':
			printf("Pushing ->        Comma:  \",\"\t%zu[%zu]\n", m_line, m_column);
			m_tokens->push_back({ Token::Type::Comma, m_line, m_column, "," });
			break;
		case '"':
			if (!getString()) {
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
			if (!getLiteral()) {
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
			m_tokens->push_back({ Token::Type::None, m_line, m_column, std::string(1, peek()) });
			m_job->printErrorLine(m_tokens->back(),
			                      (std::string() + "unexpected character '" + peek() + "'").c_str());
			return;
			break;
		}

		increment();
	}
}

// -----------------------------------------

char Lexer::peek()
{
	return m_job->input()[m_index];
}

char Lexer::peekNext()
{
	return m_job->input()[m_index + 1];
}

void Lexer::increment()
{
	m_index++;
	m_column++;
}

void Lexer::decrement()
{
	m_index--;
	m_column--;
}

char Lexer::consume()
{
	char character = peek();
	increment();
	return character;
}

bool Lexer::consumeSpecific(char character)
{
	if (peek() != character) {
		return false;
	}

	increment();
	return true;
}

bool Lexer::getString()
{
	size_t column = m_column;
	std::string symbol = "";

	bool escape = false;
	char character = consume();
	for (;;) {
		character = peek();

		if (!escape && character == '\\') {
			symbol += '\\';
			increment();
			escape = true;
			continue;
		}

		if (!escape && character == '"') {
			break;
		}

		symbol += character;
		increment();

		if (escape) {
			escape = false;
		}
	}

	printf("Pushing ->       String:  \"%s\"\t%zu[%zu]\n", symbol.c_str(), m_line, column);
	m_tokens->push_back({ Token::Type::String, m_line, column, symbol });

	if (character != '"') {
		m_job->printErrorLine(m_job->tokens()->back(), "strings should be wrapped in double quotes");
		return false;
	}

	return true;
}

bool Lexer::getNumberOrLiteral(Token::Type type)
{
	size_t column = m_column;
	std::string symbol = "";
	std::string breakOnGrammar = std::string() + "{}[]:,\" " + '\t' + '\r' + '\n';

	for (char character;;) {
		character = peek();

		// Break on all valid JSON grammar thats not a number or literal
		if (breakOnGrammar.find(character) != std::string::npos) {
			break;
		}

		symbol += character;
		increment();
	}
	decrement();

	m_tokens->push_back({ type, m_line, column, symbol });

	return true;
}

bool Lexer::getNumber()
{
	return getNumberOrLiteral(Token::Type::Number);
}

bool Lexer::getLiteral()
{
	return getNumberOrLiteral(Token::Type::Literal);
}

} // namespace Json
