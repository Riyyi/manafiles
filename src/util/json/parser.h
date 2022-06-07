/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <cstddef> // size_t
#include <map>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "util/json/lexer.h"
#include "util/json/value.h"

namespace Json {

class Parser {
public:
	Parser(const std::string& input);
	virtual ~Parser();

	void parse();

private:
	Token peek();

	Token consume();
	bool consumeSpecific(Token::Type type);

	Value getArray();
	Value getObject();

	std::string m_input;

	std::vector<Token> m_tokens;
	size_t m_index { 0 };
};

} // namespace Json

#endif // JSON_PARSER_H
