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
	if (m_indent > 0) {
	m_output += '[';
		m_output += '\n';
	}

	// Empty Array early return
	if (value.m_value.array->empty()) {
		m_output += ']';
		return;
	}

	size_t i = 0;
	auto it = value.m_value.array->elements().cbegin();
	if (!m_indent) {
		for (; i < value.m_value.array->size() - 1; ++i, ++it) {
			dumpHelper(*it, indentLevel + 1);
			m_output += ',';
		}
		dumpHelper(*it, indentLevel + 1);
	}
	else {
		std::string indentation = std::string(m_indent * (indentLevel + 1), m_indentCharacter);

		for (; i < value.m_value.array->size() - 1; ++i, ++it) {
			m_output += indentation;
			dumpHelper(*it, indentLevel + 1);
			m_output += ",\n";
		}
		m_output += indentation;
		dumpHelper(*it, indentLevel + 1);
		m_output += '\n';

		// Append indentation
		m_output += std::string(m_indent * indentLevel, m_indentCharacter);
	}

	m_output += "]";
}

void Serializer::dumpObject(const Value& value, const uint32_t indentLevel)
{
	if (m_indent > 0) {
	m_output += '{';
		m_output += '\n';
	}

	// Empty Object early return
	if (value.m_value.object->empty()) {
		m_output += '}';
		return;
	}

	size_t i = 0;
	auto it = value.m_value.object->members().cbegin();
	if (!m_indent) {
		for (; i < value.m_value.object->size() - 1; ++i, ++it) {
			m_output += '"' + it->first + "\":";
			dumpHelper(it->second, indentLevel + 1);
			m_output += ',';
		}
		m_output += '"' + it->first + "\":";
		dumpHelper(it->second, indentLevel + 1);
	}
	else {
		std::string indentation = std::string(m_indent * (indentLevel + 1), m_indentCharacter);

		for (; i < value.m_value.object->size() - 1; ++i, ++it) {
			m_output += indentation;
			m_output += '"' + it->first + "\": ";
			dumpHelper(it->second, indentLevel + 1);
			m_output += ",\n";
		}
		m_output += indentation;
		m_output += '"' + it->first + "\": ";
		dumpHelper(it->second, indentLevel + 1);
		m_output += '\n';

		// Append indentation
		m_output += std::string(m_indent * indentLevel, m_indentCharacter);
	}

	m_output += '}';
}

} // namespace Json
