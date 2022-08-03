/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <cstdint> // int32_t, int64_t, uint8_t, uint32_t
#include <sstream> // stringstream
#include <string_view>

namespace Util::Format {

class Builder {
public:
	enum class Align : uint8_t {
		None,
		Left = 60,   // '<'
		Right = 62,  // '>'
		Center = 94, // '^'
	};

	enum class Sign : uint8_t {
		None,
		Negative = 45, // '-'
		Both = 43,     // '+'
		Space = 32,    // ' '
	};

	explicit Builder(std::stringstream& builder)
		: m_builder(builder)
	{
	}

	void putLiteral(std::string_view literal);

	void putI32(int32_t number) const { m_builder << number; }  // int
	void putU32(uint32_t number) const { m_builder << number; } // unsigned int
	void putI64(int64_t number) const { m_builder << number; }  // long int
	void putU64(size_t number) const { m_builder << number; }   // long unsigned int
	void putF32(float number, uint8_t precision = 6) const;
	void putF64(double number, uint8_t precision = 6) const;
	void putCharacter(char character) const { m_builder.write(&character, 1); }
	void putString(std::string_view string, size_t width = 0, Align align = Align::Left, char fill = ' ') const;
	void putPointer(const void* pointer) const { m_builder << pointer; }

	const std::stringstream& builder() const { return m_builder; }
	std::stringstream& builder() { return m_builder; }

private:
	std::stringstream& m_builder;
};

} // namespace Util::Format
