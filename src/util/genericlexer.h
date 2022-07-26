/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <string_view>

namespace Util {

class GenericLexer {
public:
	GenericLexer(std::string_view input);
	virtual ~GenericLexer();

	// Position

	size_t tell() const;
	size_t tellRemaining() const;
	bool isEOF() const;

	// Access

	char peek(size_t offset = 0) const;

	// Modifiers

	void ignore(size_t count = 1);
	void retreat(size_t count = 1);
	char consume();
	bool consumeSpecific(const char& character);

protected:
	size_t m_index { 0 };
	std::string_view m_input;
};

} // namespace Util
