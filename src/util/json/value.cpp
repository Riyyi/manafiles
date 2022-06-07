/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <string>
#include <utility> // move

#include "util/json/array.h"
#include "util/json/object.h"
#include "util/json/value.h"

namespace Json {

Value::Value(const Value& other)
{
	copyFrom(other);
}

Value& Value::operator=(const Value& other)
{
	if (this != &other) {
		clear();
		copyFrom(other);
	}

	return *this;
}

Value::Value(bool value)
	: m_type(Type::Bool)
{
	m_value.asBool = value;
}

Value::Value(double value)
	: m_type(Type::Number)
{
	m_value.asDouble = value;
}

Value::Value(const char* value)
	: m_type(Type::String)
{
	m_value.asString = new std::string(value);
}

Value::Value(const std::string& value)
	: m_type(Type::String)
{
	m_value.asString = new std::string(value);
}

Value::Value(const Array& value)
	: m_type(Type::Array)
{
	m_value.asArray = new Array(value);
}

Value::Value(const Object& value)
	: m_type(Type::Object)
{
	m_value.asObject = new Object(value);
}

// ------------------------------------------

void Value::clear()
{
	if (m_type == Type::String) {
		delete m_value.asString;
	}
	if (m_type == Type::Array) {
		delete m_value.asArray;
	}
	if (m_type == Type::Object) {
		delete m_value.asObject;
	}
}

void Value::copyFrom(const Value& other)
{
	m_type = other.m_type;

	switch (m_type) {
	case Type::Bool:
		m_value.asBool = other.m_value.asBool;
		break;
	case Type::Number:
		m_value.asDouble = other.m_value.asDouble;
		break;
	case Type::String:
		m_value.asString = new std::string(*other.m_value.asString);
		break;
	case Type::Array:
		m_value.asArray = new Array(*other.m_value.asArray);
		break;
	case Type::Object:
		m_value.asObject = new Object(*other.m_value.asObject);
		break;
	default:
		break;
	}
}

} // namespace Json
