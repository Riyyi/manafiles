/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <cstdint> // int32_t, uint32_t, int64_t
#include <sstream> // stringstream
#include <string_view>

namespace Util::Format {

class Builder {
public:
	explicit Builder(std::stringstream& builder)
		: m_builder(builder)
	{
	}

	void putLiteral(std::string_view literal);

	void putI32(int32_t number) const { m_builder << number; }  // int
	void putU32(uint32_t number) const { m_builder << number; } // unsigned int
	void putI64(int64_t number) const { m_builder << number; }  // long int
	void putU64(size_t number) const { m_builder << number; }   // long unsigned int
	void putF32(float number, size_t precision = 6) const;
	void putF64(double number, size_t precision = 6) const;
	void putCharacter(char character) const { m_builder.write(&character, 1); }
	void putString(const std::string_view string) const { m_builder.write(string.data(), string.length()); }
	void putPointer(const void* pointer) const { m_builder << pointer; }

	const std::stringstream& builder() const { return m_builder; }
	std::stringstream& builder() { return m_builder; }

private:
	std::stringstream& m_builder;
};

} // namespace Util::Format
