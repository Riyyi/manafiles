/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // max, min

#include "util/genericlexer.h"
#include "util/meta/assert.h"

namespace Util {

GenericLexer::GenericLexer(std::string_view input)
	: m_input(input)
{
}

GenericLexer::~GenericLexer()
{
}

// -----------------------------------------

size_t GenericLexer::tell() const
{
	return m_index;
}

size_t GenericLexer::tellRemaining() const
{
	return m_input.length() - m_index;
}

bool GenericLexer::isEOF() const
{
	return m_index >= m_input.length();
}

char GenericLexer::peek(size_t offset) const
{
	return (m_index + offset < m_input.length()) ? m_input[m_index + offset] : '\0';
}

void GenericLexer::ignore(size_t count)
{
	m_index += std::min(count, m_input.length() - m_index);
}

void GenericLexer::retreat(size_t count)
{
	m_index -= std::min(count, m_index);
}

char GenericLexer::consume()
{
	VERIFY(!isEOF());
	return m_input[m_index++];
}

bool GenericLexer::consumeSpecific(const char& character)
{
	if (peek() != character) {
		return false;
	}

	ignore();
	return true;
}

} // namespace Util
