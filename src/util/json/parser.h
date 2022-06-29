/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <cstddef> // size_t
#include <vector>

#include "util/json/lexer.h"
#include "util/json/value.h"

namespace Json {

class Job;

class Parser {
public:
	Parser(Job* job);
	virtual ~Parser();

	Value parse();

private:
	Token peek();
	bool seekForward(Token::Type type);

	Token consume();
	bool consumeSpecific(Token::Type type);

	Value getLiteral();
	Value getNumber();
	Value getArray();
	Value getObject();

	Job* m_job { nullptr };

	size_t m_index { 0 };

	std::vector<Token>* m_tokens { nullptr };
};

} // namespace Json

#endif // JSON_PARSER_H
