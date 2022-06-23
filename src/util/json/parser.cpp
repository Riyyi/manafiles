/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstddef> // size_t
#include <cstdint> // uint32_t
#include <cstdio>  // printf
#include <map>
#include <string> // stod

#include "util/json/array.h"
#include "util/json/lexer.h"
#include "util/json/object.h"
#include "util/json/parser.h"

namespace Json {

Parser::Parser(const std::string& input)
	: m_input(input)
{
}

Parser::~Parser()
{
}

// -----------------------------------------

Value Parser::parse()
{
	Lexer lexer(m_input);
	lexer.analyze();
	m_tokens = lexer.tokens();

	printf("---------\n");
	printf("Parsing:\n");

	Value result;

	Token token;
	while (m_index < m_tokens.size()) {
		token = peek();

		switch (token.type) {
		case Token::Type::Literal:
			if (token.symbol == "null") {
				result = Value {};
			}
			else if (token.symbol == "true") {
				result = Value { true };
			}
			else if (token.symbol == "false") {
				result = Value { false };
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
		case Token::Type::Comma:
			// Error!
			// Multiple JSON root elements
			break;
		default:
			// Error!
			break;
		}

		break;
	}

	return result;
}

// -----------------------------------------

Token Parser::peek()
{
	return m_tokens[m_index];
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
	size_t index = m_index;
	m_index++;

	Value array;

	Token token;
	for (;;) {
		token = consume();

		switch (token.type) {
		case Token::Type::Literal:
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
			break;
		case Token::Type::Number:
			printf("Adding number to array.. v:{%s}, t:{%d} -> %f\n", token.symbol.c_str(), (int)token.type, std::stod(token.symbol));
			array.emplace_back(std::stod(token.symbol));
			break;
		case Token::Type::String:
			printf("Adding string to array.. v:{%s}, t:{%d}\n", token.symbol.c_str(), (int)token.type);
			array.emplace_back(token.symbol);
			break;
		case Token::Type::BracketOpen:
			m_index--;
			array.emplace_back(getArray());
			break;
		case Token::Type::BraceOpen:
			m_index--;
			array.emplace_back(getObject());
			break;
		default:
			// Error!
			printf("Invalid JSON! array:1\n");
			break;
		}

		// Find , or }
		token = consume();
		if (token.type == Token::Type::Comma) {
			continue;
		}
		else if (token.type == Token::Type::BracketClose) {
			break;
		}
		else {
			// Error!
			printf("Invalid JSON! array:2\n");
			break;
		}
	}

	return array;
}

Value Parser::getObject()
{
	size_t index = m_index;
	m_index++;

	Value object;

	Token token;
	std::string key;
	std::map<std::string, uint32_t> unique;
	for (;;) {
		// Find string key
		token = consume();
		if (token.type != Token::Type::String) {
			// Error!
			printf("Invalid JSON! 1\n");
			break;
		}

		// Check if key exists in hashmap
		key = token.symbol;
		if (unique.find(key) != unique.end()) {
			// If exists, unique key fail!
			// Error!
			printf("Invalid JSON! 2\n");
			break;
		}
		// Add key to hashmap
		unique.insert({ key, 0 });

		// Find :
		if (!consumeSpecific(Token::Type::Colon)) {
			// Error!
			printf("Invalid JSON! 3\n");
			break;
		}

		// Find string/number/literal value
		token = consume();
		if (token.type != Token::Type::String
		    && token.type != Token::Type::Number
		    && token.type != Token::Type::Literal
		    && token.type != Token::Type::BracketOpen
		    && token.type != Token::Type::BraceOpen) {
			// Error!
			printf("Invalid JSON! 4\n");
			break;
		}

		// Add member to object
		switch (token.type) {
		case Token::Type::Literal:
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
			break;
		case Token::Type::Number:
			printf("Adding number to object.. k:{%s}, v:{%s}, t:{%d} -> %f\n", key.c_str(), token.symbol.c_str(), (int)token.type, std::stod(token.symbol));
			object[key] = std::stod(token.symbol);
			break;
		case Token::Type::String:
			printf("Adding string to object.. k:{%s}, v:{%s}, t:{%d}\n", key.c_str(), token.symbol.c_str(), (int)token.type);
			object[key] = token.symbol;
			break;
		case Token::Type::BracketOpen:
			m_index--;
			object[key] = getArray();
			break;
		case Token::Type::BraceOpen:
			m_index--;
			object[key] = getObject();
			break;
		default:
			// Error!
			printf("Invalid JSON! 5\n");
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
			// Error!
			printf("Invalid JSON! 6\n");
			break;
		}
	}

	return object;
}

} // namespace Json
