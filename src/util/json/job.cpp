/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // count
#include <sstream>   // istringstream
#include <string>    // getline

#include "util/json/job.h"
#include "util/json/lexer.h"
#include "util/json/parser.h"
#include "util/json/value.h"

namespace Json {

Job::Job(const std::string& input)
	: m_input(input)
	// FIXME: handle case where the file doenst have a trailing newline
	, m_lineNumbersWidth(std::to_string(std::count(m_input.begin(), m_input.end(), '\n') - 1).length())
{
}

Job::~Job()
{
}

// ------------------------------------------

Value Job::fire()
{
	Lexer lexer(this);
	lexer.analyze();

	if (!m_success) {
		return nullptr;
	}

	Parser parser(this);
	Value value = parser.parse();

	if (!m_success) {
		return nullptr;
	}

	return value;
}

void Job::printErrorLine(Token token, const char* message)
{
	m_success = false;

	// Error message
	std::string errorFormat = "\033[;1m" // Bold
							  "JSON:%zu:%zu: "
							  "\033[31;1m" // Bold red
							  "error: "
							  "\033[0m" // Reset
							  "%s"
							  "\n";
	printf(errorFormat.c_str(),
	       token.line,
	       token.column,
	       message);

	// Get the JSON line that caused the error
	std::istringstream input(m_input);
	std::string line;
	size_t count = 0;
	while (std::getline(input, line)) {
		count++;
		if (count == token.line) {
			break;
		}
	}
	// Replace tab indentation with spaces
	size_t oldLineLength = line.length();
	size_t tabs = line.find_first_not_of('\t');
	if (tabs > 0 && tabs < line.size()) {
		line = std::string(tabs * 4, ' ') + line.substr(tabs);
	}
	token.column += line.length() - oldLineLength;

	// JSON line
	std::string lineFormat = " %"
	                         + std::to_string(m_lineNumbersWidth)
	                         + "zu | "
	                           "%s"
	                           "\033[31;1m" // Bold red
	                           "%s"
	                           "\033[0m" // Reset
	                           "\n";
	printf(lineFormat.c_str(),
	       token.line,
	       line.substr(0, token.column - 1).c_str(),
	       line.substr(token.column - 1).c_str());

	// Arrow pointer
	std::string arrowFormat = " %s | "
							  "\033[31;1m" // Bold red
							  "%s^%s"
							  "\033[0m" // Reset
							  "\n";
	printf(arrowFormat.c_str(),
	       std::string(m_lineNumbersWidth, ' ').c_str(),
	       std::string(token.column - 1, ' ').c_str(),
	       std::string(line.length() - token.column, '~').c_str());
}

} // namespace Json