/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <cstddef> // size_t
#include <map>
#include <memory> // shared_ptr
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "util/json/lexer.h"
#include "util/json/value.h"

namespace Json {

class Job;

class Parser {
public:
	Parser(std::shared_ptr<Job> job);
	virtual ~Parser();

	Value parse();

private:
	Token peek();

	Token consume();
	bool consumeSpecific(Token::Type type);

	Value getArray();
	Value getObject();

	std::shared_ptr<Job> m_job { nullptr };

	std::vector<Token> m_tokens;
	size_t m_index { 0 };
};

} // namespace Json

#endif // JSON_PARSER_H
