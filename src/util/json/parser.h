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

namespace Json {

class Job;
class Value;

class Parser {
public:
	Parser(Job* job);
	virtual ~Parser();

	Value parse();

private:
	bool reachedEnd();
	bool seekForward(Token::Type type);

	Token peek();
	Token consume();

	Value consumeLiteral();
	Value consumeNumber();
	Value consumeString();
	Value consumeArray();
	Value consumeObject();

	Job* m_job { nullptr };

	size_t m_index { 0 };

	std::vector<Token>* m_tokens { nullptr };
};

} // namespace Json

#endif // JSON_PARSER_H
