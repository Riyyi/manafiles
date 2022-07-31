/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <cstdint> // uint32_t
#include <cstring> // strlen
#include <fstream> // ifstream
#include <string>
#include <string_view>

#include "util/format/format.h"
#include "util/meta/compiler.h"

#define CRASH() asm volatile("int $0x03");

#ifndef NDEBUG
	#define VERIFY(expr, ...) (static_cast<bool>(expr) ? (void)0 : Util::__assertion_failed(#expr, __FILE__, __LINE__, FUNCTION_MACRO __VA_OPT__(, ) __VA_ARGS__))
	#define VERIFY_NOT_REACHED() VERIFY(false)
#else
	#define VERIFY(expr, ...) (static_cast<bool>(expr) ? (void)0 : CRASH()
	#define VERIFY_NOT_REACHED() CRASH()
#endif

#ifndef NDEBUG
namespace Util {

template<typename... Parameters>
inline void __assertion_failed(const char* assertion, const char* file, uint32_t line, const char* function, const Parameters&... parameters)
{
	// Get the line that caused the error
	std::ifstream source(file);
	std::string content;
	if (source.is_open()) {
		for (uint32_t i = 0; std::getline(source, content); ++i) {
			if (i == line - 1) {
				break;
			}
		}
	}
	// Replace tab indentation with spaces
	size_t tabs = content.find_first_not_of('\t');
	if (tabs > 0 && tabs < content.size()) {
		content = std::string(tabs * 4, ' ') + content.substr(tabs);
	}

	// Find the assertion in the line
	size_t column = content.find(assertion);
	size_t assertionLength = strlen(assertion);
	if (column == std::string::npos) {
		column = content.find_first_not_of(' ');
		assertionLength = content.length() - column;
	}

	// Error message
	fprintf(stderr,
	        "\033[;1m%s:%u:%zu "
	        "\033[31;1merror: "
	        "\033[0massertion failed",
	        file, line, column + 1);
	if constexpr (sizeof...(Parameters) > 0) {
		fprintf(stderr, ": ");
		// Cant use the formatting library to print asserts caused by the formatting library
		std::string_view functionString = function;
		if (functionString.find("Util::Format::") != std::string_view::npos
		    && functionString.find("Util::GenericLexer::") != std::string_view::npos) {
			std::string message;
			formatTo(message, parameters...);
			fprintf(stderr, "%s", message.c_str());
		}
		else {
			fprintf(stderr, parameters...);
		}
	}

	// Code line
	fprintf(stderr, "\n %u | %s\033[31;1m%s\033[0m%s\n", line,
	        content.substr(0, column).c_str(),                 // Whitespace at front
	        content.substr(column, assertionLength).c_str(),   // Error portion
	        content.substr(column + assertionLength).c_str()); // Rest of the line

	// Arrow pointer
	fprintf(stderr, " %s | %s\033[31;1m^%s\033[0m\n",
	        std::string(std::to_string(line).length(), ' ').c_str(), // Line number spacing
	        std::string(column, ' ').c_str(),                        // Content spacing
	        std::string(assertionLength - 1, '~').c_str());          // Arrow pointer

	CRASH();
}

} // namespace Util
#endif
