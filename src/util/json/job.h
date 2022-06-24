/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_JOB_H
#define JSON_JOB_H

#include <cstddef> // size_t
#include <string>

#include "util/json/lexer.h"

namespace Json {

class Job {
public:
	Job(const std::string& input);
	virtual ~Job();

	enum class Color {
		None,
		Info,
		Warn,
		Danger,
		Success,
		Comment,
	};

	void printErrorLine(Token token, const char* message);

	void setLineNumbersWidth(size_t width) { m_lineNumbersWidth = width; }

	const std::string& input() { return m_input; }

private:
	std::string m_input;
	size_t m_lineNumbersWidth { 0 };
};

} // namespace Json

#endif // JSON_JOB_H
