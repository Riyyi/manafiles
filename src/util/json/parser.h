/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <vector>

#include "util/json/lexer.h"

namespace Util::JSON {

class Job;
class Value;

class Parser {
public:
	Parser(Job* job);
	virtual ~Parser();

	Value parse();

private:
	bool isEOF();
	Token peek();
	Token consume();
	void ignoreUntil(Token::Type type);

	Value consumeLiteral();
	Value consumeNumber();
	Value consumeString();
	Value consumeArray();
	Value consumeObject();

	Job* m_job { nullptr };

	size_t m_index { 0 };

	std::vector<Token>* m_tokens { nullptr };
};

} // namespace Util::JSON
