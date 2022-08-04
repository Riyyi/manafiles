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

void Builder::putU64(size_t value, char fill, Align align, Sign sign, bool zeroPadding, size_t width, bool isNegative) const
{
	std::string string = (std::stringstream {} << value).str();

	// Sign
	std::string signCharacter = "";
	switch (sign) {
	case Sign::None:
	case Sign::Negative:
		if (isNegative) {
			signCharacter = '-';
		}
		break;
	case Sign::Both:
		signCharacter = (isNegative) ? '-' : '+';
		break;
	case Sign::Space:
		signCharacter = (isNegative) ? '-' : ' ';
		break;
	default:
		VERIFY_NOT_REACHED();
	};
	if (align != Align::None || !zeroPadding) {
		string.insert(0, signCharacter);
	}

	// Zero padding
	if (zeroPadding) {
		fill = '0';
	}

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
	case Align::None:
		if (zeroPadding) {
			m_builder << signCharacter;
			if (signCharacter.empty()) {
				m_builder << '0';
			}
		}
		m_builder << std::string(width - length - zeroPadding, fill);
		m_builder.write(string.data(), length);
		break;
	default:
		VERIFY_NOT_REACHED();
	};
}

void Builder::putI64(int64_t value, char fill, Align align, Sign sign, bool zeroPadding, size_t width) const
{
	bool isNegative = value < 0;
	value = isNegative ? -value : value;
	putU64(static_cast<uint64_t>(value), fill, align, sign, zeroPadding, width, isNegative);
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

void Builder::putString(std::string_view string, char fill, Align align, size_t width) const
{
	size_t length = string.length();
	if (width < length) {
		m_builder.write(string.data(), length);
		return;
	}

	switch (align) {
	case Align::None:
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
