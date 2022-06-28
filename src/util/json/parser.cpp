/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstddef> // size_t
#include <cstdint> // uint32_t
#include <cstdio>  // printf, sprintf
#include <map>
#include <memory> // shared_ptr
#include <string> // stod

#include "util/json/array.h"
#include "util/json/job.h"
#include "util/json/lexer.h"
#include "util/json/object.h"
#include "util/json/parser.h"

namespace Json {

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
	printf("---------\n");
	printf("Parsing:\n");

	Value result;

	Token token;
	while (m_index < m_tokens->size()) {
		token = peek();

		switch (token.type) {
		case Token::Type::Literal:
			if (token.symbol == "null") {
				result = nullptr;
			}
			else if (token.symbol == "true") {
				result = true;
			}
			else if (token.symbol == "false") {
				result = false;
			}
			m_index++;
			break;
		case Token::Type::Number:
			result = Value { std::stod(token.symbol) };
			m_index++;
			break;
		case Token::Type::String:
			result = Value { token.symbol };
			m_index++;
			break;
		case Token::Type::BracketOpen:
			result = getArray();
			break;
		case Token::Type::BraceOpen:
			result = getObject();
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
	}

	return result;
}

// -----------------------------------------

Token Parser::peek()
{
	return (*m_tokens)[m_index];
}

bool Parser::seekForward(Token::Type type)
{
	for (size_t index = m_index; index < m_tokens->size(); ++index) {
		if ((*m_tokens)[index].type == type) {
			m_index = index;
			return true;
		}
	}

	return false;
}

Token Parser::consume()
{
	Token token = peek();
	m_index++;
	return token;
}

bool Parser::consumeSpecific(Token::Type type)
{
	if (peek().type != type) {
		return false;
	}

	m_index++;
	return true;
}

Value Parser::getArray()
{
	m_index++;

	auto reportError = [this](Token token, const std::string& message) -> void {
		m_job->printErrorLine(token, message.c_str());

		// After an error, try to find the closing bracket
		seekForward(Token::Type::BracketClose);
		consumeSpecific(Token::Type::BracketClose);
	};

	Value array = Value::Type::Array;
	Token token;
	for (;;) {
		token = consume();

		if (token.type == Token::Type::Literal) {
			printf("Adding literal to array.. v:{%s}, t:{%d}\n", token.symbol.c_str(), (int)token.type);
			if (token.symbol == "null") {
				array.emplace_back(nullptr);
			}
			else if (token.symbol == "true") {
				array.emplace_back(true);
			}
			else if (token.symbol == "false") {
				array.emplace_back(false);
			}
		}
		else if (token.type == Token::Type::Number) {
			printf("Adding number to array.. v:{%s}, t:{%d} -> %f\n", token.symbol.c_str(), (int)token.type, std::stod(token.symbol));
			array.emplace_back(std::stod(token.symbol));
		}
		else if (token.type == Token::Type::String) {
			printf("Adding string to array.. v:{%s}, t:{%d}\n", token.symbol.c_str(), (int)token.type);
			array.emplace_back(token.symbol);
		}
		else if (token.type == Token::Type::BracketOpen) {
			m_index--;
			array.emplace_back(getArray());
		}
		else if (token.type == Token::Type::BraceOpen) {
			m_index--;
			array.emplace_back(getObject());
		}
		else if (token.type == Token::Type::BracketClose) {
			// Trailing comma
			if (array.asArray().size() > 0) {
				reportError((*m_tokens)[m_index - 2], "invalid comma, expecting ']'");
			}
			break;
		}
		else {
			reportError(token, "expecting value or ']', not '" + token.symbol + "'");
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
			reportError(token, "expecting comma or ']', not '" + token.symbol + "'");
			break;
		}
	}

	return array;
}

Value Parser::getObject()
{
	m_index++;

	auto reportError = [this](Token token, const std::string& message) -> void {
		m_job->printErrorLine(token, message.c_str());

		// After an error, try to find the closing bracket
		seekForward(Token::Type::BraceClose);
		consumeSpecific(Token::Type::BraceClose);
	};

	Value object = Value::Type::Object;
	Token token;
	std::string key;
	std::map<std::string, uint32_t> unique;
	for (;;) {
		token = consume();
		// Empty object
		if (token.type == Token::Type::BraceClose) {
			// Trailing comma
			if (object.asObject().size() > 0) {
				reportError((*m_tokens)[m_index - 2], "invalid comma, expecting '}'");
			}
			break;
		}
		// Find string key
		if (token.type != Token::Type::String) {
			reportError(token, "expecting string, or '}' not '" + token.symbol + "'");
			break;
		}

		// Check if key exists in hashmap
		key = token.symbol;
		if (unique.find(key) != unique.end()) {
			reportError(token, "duplicate key '" + token.symbol + "', names should be unique");
			break;
		}
		// Add key to hashmap
		unique.insert({ key, 0 });

		// Find :
		token = consume();
		if (token.type != Token::Type::Colon) {
			reportError(token, "expecting colon, not '" + token.symbol + "'");
			break;
		}

		// Add member (name:value pair) to object
		token = consume();
		if (token.type == Token::Type::Literal) {
			printf("Adding literal to object.. k:{%s}, v:{%s}, t:{%d}\n", key.c_str(), token.symbol.c_str(), (int)token.type);
			if (token.symbol == "null") {
				object[key] = nullptr;
			}
			else if (token.symbol == "true") {
				object[key] = true;
			}
			else if (token.symbol == "false") {
				object[key] = false;
			}
		}
		else if (token.type == Token::Type::Number) {
			printf("Adding number to object.. k:{%s}, v:{%s}, t:{%d} -> %f\n", key.c_str(), token.symbol.c_str(), (int)token.type, std::stod(token.symbol));
			object[key] = std::stod(token.symbol);
		}
		else if (token.type == Token::Type::String) {
			printf("Adding string to object.. k:{%s}, v:{%s}, t:{%d}\n", key.c_str(), token.symbol.c_str(), (int)token.type);
			object[key] = token.symbol;
		}
		else if (token.type == Token::Type::BracketOpen) {
			m_index--;
			object[key] = getArray();
		}
		else if (token.type == Token::Type::BraceOpen) {
			m_index--;
			object[key] = getObject();
		}
		else {
			reportError(token, "expecting value, not '" + token.symbol + "'");
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

} // namespace Json
