/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // all_of
#include <cassert>   // assert
#include <cstdint>   // uint32_t
#include <iostream>
#include <memory> // make_shared, shared_ptr
#include <string>
#include <utility> // move

#include "util/json/array.h"
#include "util/json/job.h"
#include "util/json/object.h"
#include "util/json/serializer.h"
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

Value::Value(Type type)
	: m_type(type)
{
	create();
}

Value::Value(bool value)
	: m_type(Type::Bool)
{
	m_value.asBool = value;
}

Value::Value(int value)
	: m_type(Type::Number)
{
	m_value.asDouble = value;
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

Value::Value(const std::initializer_list<Value>& values)
{
	bool isObject = std::all_of(values.begin(), values.end(), [](const Value& value) {
		return value.type() == Type::Array
		       && value.size() == 2
		       && value[0].m_type == Type::String;
	});

	if (!isObject) {
		m_type = Type::Array;
		m_value.asArray = new Array(values);
	}
	else {
		m_type = Type::Object;
		m_value.asObject = new Object;

		for (auto& value : values) {
			m_value.asObject->emplace(std::move(*value[0].m_value.asString),
			                          std::move(value[1]));
		}
	}
}

// ------------------------------------------

Value Value::parse(const std::string& input)
{
	Job job(input);
	Value value = job.fire();

	return value;
}

std::string Value::dump(const uint32_t indent, const char indentCharacter) const
{
	Serializer serializer(*this, indent, indentCharacter);
	return serializer.dump();
}

void Value::emplace_back(Value value)
{
	// Implicitly convert null to an array
	if (m_type == Type::Null) {
		m_type = Type::Array;
		m_value.asArray = new Array;
	}

	assert(m_type == Type::Array);
	m_value.asArray->emplace_back(value);
}

void Value::emplace(const std::string& key, Value value)
{
	// Implicitly convert null to an object
	if (m_type == Type::Null) {
		m_type = Type::Object;
		m_value.asObject = new Object;
	}

	assert(m_type == Type::Array);
	m_value.asObject->emplace(key, value);
}

// ------------------------------------------

Value& Value::operator[](size_t index)
{
	// Implicitly convert null to an array
	if (m_type == Type::Null) {
		m_type = Type::Array;
		m_value.asArray = new Array;
	}

	assert(m_type == Type::Array);
	return m_value.asArray->at(index);
}

Value& Value::operator[](const std::string& key)
{
	// Implicitly convert null to an object
	if (m_type == Type::Null) {
		m_type = Type::Object;
		m_value.asObject = new Object;
	}

	assert(m_type == Type::Object);
	return m_value.asObject->at(key);
}

const Value& Value::operator[](size_t index) const
{
	assert(m_type == Type::Array);
	return m_value.asArray->at(index);
}

const Value& Value::operator[](const std::string& key) const
{
	assert(m_type == Type::Object);
	return m_value.asObject->at(key);
}

// ------------------------------------------

size_t Value::size() const
{
	switch (m_type) {
	case Type::Null:
		return 0;
	case Type::Bool:
	case Type::Number:
	case Type::String:
		return 1;
	case Type::Array:
		return m_value.asArray->size();
	case Type::Object:
		return m_value.asObject->size();
	default:
		return 1;
	}
}

// ------------------------------------------

void Value::create()
{
	switch (m_type) {
	case Type::Bool:
		m_value.asBool = false;
		break;
	case Type::Number:
		m_value.asDouble = 0.0;
		break;
	case Type::String:
		m_value.asString = new std::string;
		break;
	case Type::Array:
		m_value.asArray = new Array;
		break;
	case Type::Object:
		m_value.asObject = new Object;
		break;
	default:
		break;
	}
}

void Value::clear()
{
	switch (m_type) {
	case Type::String:
		delete m_value.asString;
		break;
	case Type::Array:
		delete m_value.asArray;
		break;
	case Type::Object:
		delete m_value.asObject;
		break;
	default:
		break;
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

// ------------------------------------------

std::istream& operator>>(std::istream& input, Value& value)
{
	std::string inputString;

	char buffer[4096];
	while (input.read(buffer, sizeof(buffer))) {
		inputString.append(buffer, sizeof(buffer));
	}
	inputString.append(buffer, input.gcount());

	Job job(inputString);
	value = job.fire();

	return input;
}

std::ostream& operator<<(std::ostream& output, const Value& value)
{
	return output << value.dump(4);
}

} // namespace Json
