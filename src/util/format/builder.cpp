/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstddef> // size_t
#include <iomanip> // setprecision
#include <ios>     // defaultfloat, fixed
#include <limits>  // numeric_limits
#include <sstream> // stringstream
#include <string>
#include <string_view>

#include "util/format/builder.h"

namespace Util::Format {

void Builder::putLiteral(std::string_view literal)
{
	for (size_t i = 0; i < literal.length(); ++i) {
		putCharacter(literal[i]);
		if (literal[i] == '{' || literal[i] == '}') {
			++i;
		}
	}
}

void Builder::putF32(float number, size_t precision) const
{
	precision = (precision > std::numeric_limits<float>::digits10) ? 6 : precision;

	std::stringstream stream;
	stream
		<< std::fixed << std::setprecision(precision)
		<< number
		<< std::defaultfloat << std::setprecision(6);
	std::string string = stream.str();
	m_builder << string;
}

void Builder::putF64(double number, size_t precision) const
{
	precision = (precision > std::numeric_limits<double>::digits10) ? 6 : precision;

	std::stringstream stream;
	stream
		<< std::fixed << std::setprecision(precision)
		<< number
		<< std::defaultfloat << std::setprecision(6);
	std::string string = stream.str();
	m_builder << string;
}

} // namespace Util::Format
