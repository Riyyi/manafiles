/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // min
#include <cstddef>   // size_t
#include <iomanip>   // setprecision
#include <ios>       // defaultfloat, fixed
#include <limits>    // numeric_limits
#include <sstream>   // stringstream
#include <string>
#include <string_view>

#include "util/format/builder.h"
#include "util/meta/assert.h"

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

void Builder::putF32(float number, uint8_t precision) const
{
	precision = std::min(precision, static_cast<uint8_t>(std::numeric_limits<float>::digits10));

	std::stringstream stream;
	stream
		<< std::fixed << std::setprecision(precision)
		<< number
		<< std::defaultfloat << std::setprecision(6);
	std::string string = stream.str();
	m_builder << string;
}

void Builder::putF64(double number, uint8_t precision) const
{
	precision = std::min(precision, static_cast<uint8_t>(std::numeric_limits<double>::digits10));

	std::stringstream stream;
	stream
		<< std::fixed << std::setprecision(precision)
		<< number
		<< std::defaultfloat << std::setprecision(6);
	std::string string = stream.str();
	m_builder << string;
}

void Builder::putString(std::string_view string, size_t width, Align align, char fill) const
{
	size_t length = string.length();
	if (width < length) {
		m_builder.write(string.data(), length);
		return;
	}

	switch (align) {
	case Align::Left:
		m_builder.write(string.data(), length);
		m_builder << std::string(width - length, fill);
		break;
	case Align::Center: {
		size_t half = (width - length) / 2;
		m_builder << std::string(half, fill);
		m_builder.write(string.data(), length);
		m_builder << std::string(width - half - length, fill);
		break;
	}
	case Align::Right:
		m_builder << std::string(width - length, fill);
		m_builder.write(string.data(), length);
		break;
	default:
		VERIFY_NOT_REACHED();
	};
}

} // namespace Util::Format
