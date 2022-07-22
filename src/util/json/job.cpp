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

namespace Util::JSON {

Job::Job(std::string_view input)
	: m_input(input)
{
	// FIXME: Make this work for all newline types: \n, \r, \r\n
	m_lineNumbersWidth = std::count(m_input.begin(), m_input.end(), '\n');
	m_lineNumbersWidth += m_input.back() == '\n' ? 0 : 1;
	m_lineNumbersWidth = std::to_string(m_lineNumbersWidth).length();
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
	fprintf(stderr,
	        errorFormat.c_str(),
	        token.line + 1,
	        token.column + 1,
	        message);

	// Get the JSON line that caused the error
	std::istringstream input(m_input.data());
	std::string line;
	for (size_t i = 0; std::getline(input, line); ++i) {
		if (i == token.line) {
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
	fprintf(stderr,
	        lineFormat.c_str(),
	        token.line + 1,
	        line.substr(0, token.column).c_str(),
	        line.substr(token.column).c_str());

	// Arrow pointer
	std::string arrowFormat = " %s | "
							  "\033[31;1m" // Bold red
							  "%s^%s"
							  "\033[0m" // Reset
							  "\n";
	fprintf(stderr,
	        arrowFormat.c_str(),
	        std::string(m_lineNumbersWidth, ' ').c_str(),
	        std::string(token.column, ' ').c_str(),
	        std::string(line.length() - token.column, '~').c_str());
}

} // namespace Util::JSON
