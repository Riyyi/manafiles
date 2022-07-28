/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <string_view>
#include <vector>

#include "util/json/lexer.h"

namespace Util::JSON {

class Value;

class Job {
public:
	Job(std::string_view input);
	virtual ~Job();

	Value fire();

	void printErrorLine(Token token, const char* message);

	bool success() const { return m_success; }
	std::string_view input() const { return m_input; }
	std::vector<Token>* tokens() { return &m_tokens; }

private:
	bool m_success { true };

	std::string_view m_input;
	size_t m_lineNumbersWidth { 0 };

	std::vector<Token> m_tokens;
};

} // namespace Util::JSON
