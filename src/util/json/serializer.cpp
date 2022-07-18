/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdint>  // uint32_t
#include <iterator> // next
#include <sstream>  // ostringstream
#include <string>

#include "util/json/array.h"
#include "util/json/lexer.h"
#include "util/json/object.h"
#include "util/json/serializer.h"

namespace Json {

Serializer::Serializer(const uint32_t indent, const char indentCharacter)
	: m_indent(indent)
	, m_indentCharacter(indentCharacter)
{
}

Serializer::~Serializer()
{
}

// ------------------------------------------

std::string Serializer::dump(const Value& value)
{
	dumpHelper(value);
	return m_output;
}

// ------------------------------------------

void Serializer::dumpHelper(const Value& value, const uint32_t indentLevel)
{
	switch (value.m_type) {
	case Value::Type::Null:
		m_output += "null";
		break;
	case Value::Type::Bool:
		m_output += value.m_value.boolean ? "true" : "false";
		break;
	case Value::Type::Number: {
		std::ostringstream os;
		os << value.m_value.number;
		m_output += os.str();
		break;
	}
	case Value::Type::String:
		m_output += "\"" + *value.m_value.string + "\"";
		break;
	case Value::Type::Array:
		dumpArray(value, indentLevel);
		break;
	case Value::Type::Object:
		dumpObject(value, indentLevel);
		break;
	default:
		break;
	}
}

void Serializer::dumpArray(const Value& value, const uint32_t indentLevel)
{
	// Append [
	m_output += "[";
	if (m_indent > 0) {
		m_output += '\n';
	}

	auto values = value.m_value.array->elements();
	auto last = values.end();
	for (auto it = values.begin(); it != last; ++it) {
		m_output += std::string(m_indent * (indentLevel + 1), m_indentCharacter);
		dumpHelper(*it, indentLevel + 1);

		// Add comma, except after the last element
		if (std::next(it) != last) {
			m_output += ",";
		}
		if (m_indent > 0) {
			m_output += '\n';
		}
	}

	// Append indentation
	m_output += std::string(m_indent * indentLevel, m_indentCharacter);

	// Append ]
	m_output += "]";
}

void Serializer::dumpObject(const Value& value, const uint32_t indentLevel)
{
	// Append {
	m_output += "{";
	if (m_indent > 0) {
		m_output += '\n';
	}

	auto members = value.m_value.object->members();
	auto last = members.end();
	for (auto it = members.begin(); it != last; ++it) {
		m_output += std::string(m_indent * (indentLevel + 1), m_indentCharacter);
		m_output += "\"" + it->first + "\":";
		if (m_indent > 0) {
			m_output += ' ';
		}
		dumpHelper(it->second, indentLevel + 1);

		// Add comma, except after the last element
		if (std::next(it) != last) {
			m_output += ",";
		}
		if (m_indent > 0) {
			m_output += '\n';
		}
	}

	// Append indentation
	m_output += std::string(m_indent * indentLevel, m_indentCharacter);

	// Append }
	m_output += "}";
}

} // namespace Json
