/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdint>  // uint32_t
#include <sstream>  // ostringstream
#include <string>

#include "util/json/array.h"
#include "util/json/lexer.h"
#include "util/json/object.h"
#include "util/json/serializer.h"

namespace Json {

Serializer::Serializer(const Value& value, const uint32_t indent, const char indentCharacter)
	: m_value(value)
	, m_indent(indent)
	, m_indentCharacter(indentCharacter)
{
}

Serializer::~Serializer()
{
}

// ------------------------------------------

std::string Serializer::dump()
{
	return dumpHelper(m_value);
}

// ------------------------------------------

std::string Serializer::dumpHelper(const Value& value, const uint32_t indentLevel)
{
	switch (value.type()) {
	case Value::Type::Null:
		return "null";
		break;
	case Value::Type::Bool:
		return value.asBool() ? "true" : "false";
		break;
	case Value::Type::Number: {
		std::ostringstream os;
		os << value.asDouble();
		return os.str();
		break;
	}
	case Value::Type::String:
		return "\"" + value.asString() + "\"";
		break;
	case Value::Type::Array:
		return dumpArray(value, indentLevel);
		break;
	case Value::Type::Object:
		return dumpObject(value, indentLevel);
		break;
	default:
		break;
	}

	return "";
}

std::string Serializer::dumpArray(const Value& value, const uint32_t indentLevel)
{
	std::string result;

	// Append [
	result += "[";
	if (m_indent > 0) {
		result += '\n';
	}

	auto values = value.asArray().values();
	for (auto it = values.begin(); it != values.end(); ++it) {
		result += std::string(m_indent * (indentLevel + 1), m_indentCharacter);
		result += dumpHelper(*it, indentLevel + 1);

		// Add comma, except after the last element
		if (it != std::prev(values.end(), 1)) {
			result += ",";
		}
		if (m_indent > 0) {
			result += '\n';
		}
	}

	// Append indentation
	result += std::string(m_indent * indentLevel, m_indentCharacter);

	// Append ]
	result += "]";

	return result;
}

std::string Serializer::dumpObject(const Value& value, const uint32_t indentLevel)
{
	std::string result;

	// Append {
	result += "{";
	if (m_indent > 0) {
		result += '\n';
	}

	auto members = value.asObject().members();
	for (auto it = members.begin(); it != members.end(); ++it) {
		result += std::string(m_indent * (indentLevel + 1), m_indentCharacter);
		result += "\"" + it->first + "\":";
		if (m_indent > 0) {
			result += ' ';
		}
		result += dumpHelper(it->second, indentLevel + 1);

		// Add comma, except after the last element
		if (it != members.end()) {
			result += ",";
		}
		if (m_indent > 0) {
			result += '\n';
		}
	}

	// Append indentation
	result += std::string(m_indent * indentLevel, m_indentCharacter);

	// Append }
	result += "}";

	return result;
}

} // namespace Json
