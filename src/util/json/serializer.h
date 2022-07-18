/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_SERIALIZER_H
#define JSON_SERIALIZER_H

#include <cstdint> // uint32_t
#include <string>

#include "util/json/value.h"

namespace Json {

class Serializer {
public:
	Serializer(const uint32_t indent = 0, const char indentCharacter = ' ');
	virtual ~Serializer();

	std::string dump(const Value& value);

private:
	std::string dumpHelper(const Value& value, const uint32_t indentLevel = 0);
	std::string dumpArray(const Value& value, const uint32_t indentLevel = 0);
	std::string dumpObject(const Value& value, const uint32_t indentLevel = 0);

	uint32_t m_indent { 0 };
	char m_indentCharacter { ' ' };
};

} // namespace Json

#endif // JSON_SERIALIZER_H
