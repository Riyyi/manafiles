/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint> // uint8_t
#include <cstdio>  // FILE, stdout
#include <sstream> // stringstream
#include <string_view>

#include "util/format/format.h"
#include "util/timer.h"

namespace Util::Format {

void variadicPrint(FILE* file, std::string_view format, TypeErasedParameters& parameters);

template<size_t N, typename... Parameters>
void print(const char (&format)[N], const Parameters&... parameters)
{
	VariadicParameters variadicParameters { parameters... };
	variadicPrint(stdout, { format, N - 1 }, variadicParameters);
}

template<size_t N, typename... Parameters>
void print(FILE* file, const char (&format)[N], const Parameters&... parameters)
{
	VariadicParameters variadicParameters { parameters... };
	variadicPrint(file, { format, N - 1 }, variadicParameters);
}

// -----------------------------------------

class PrintOperatorStyle {
public:
	PrintOperatorStyle(FILE* file);
	virtual ~PrintOperatorStyle();

	Builder& builder() { return m_builder; }

private:
	FILE* m_file;

	std::stringstream m_stream;
	Builder m_builder;
};

template<typename T>
const PrintOperatorStyle& operator<<(const PrintOperatorStyle& printOperatorStyle, const T& value)
{
	_format(const_cast<PrintOperatorStyle&>(printOperatorStyle).builder(), value);
	return printOperatorStyle;
}

PrintOperatorStyle print();
PrintOperatorStyle print(FILE* file);

// -----------------------------------------

} // namespace Util::Format

namespace Util {

using Util::Format::print;

} // namespace Util
