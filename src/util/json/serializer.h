/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_JSON_SERIALIZER_H
#define UTIL_JSON_SERIALIZER_H

#include <cstdint> // uint32_t
#include <string>

#include "util/json/value.h"

namespace Util::JSON {

class Serializer {
public:
	Serializer(const uint32_t indent = 0, const char indentCharacter = ' ');
	virtual ~Serializer();

	std::string dump(const Value& value);

private:
	void dumpHelper(const Value& value, const uint32_t indentLevel = 0);
	void dumpArray(const Value& value, const uint32_t indentLevel = 0);
	void dumpObject(const Value& value, const uint32_t indentLevel = 0);

	std::string m_output;

	uint32_t m_indent { 0 };
	char m_indentCharacter { ' ' };
};

} // namespace Util::JSON

#endif // UTIL_JSON_SERIALIZER_H
