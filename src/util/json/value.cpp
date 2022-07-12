/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // all_of
#include <cassert>   // assert
#include <cstdint>   // uint32_t
#include <iostream>  // istream, ostream
#include <string>
#include <utility> // move

#include "util/json/array.h"
#include "util/json/job.h"
#include "util/json/object.h"
#include "util/json/serializer.h"
#include "util/json/value.h"

namespace Json {

Value::Value(std::nullptr_t)
	: Value(Type::Null)
{
}

Value::Value(Type type)
	: m_type(type)
{
	create();
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
		m_value.array = new Array(values);
	}
	else {
		m_type = Type::Object;
		m_value.object = new Object;

		for (auto& value : values) {
			m_value.object->emplace(std::move(*value[0].m_value.string),
			                          std::move(value[1]));
		}
	}
}

// Copy constructor
Value::Value(const Value& other)
{
	copyFrom(other);
}

// Assignment operator
Value& Value::operator=(const Value& other)
{
	if (this != &other) {
		clear();
		copyFrom(other);
	}

	return *this;
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
		m_value.array = new Array;
	}

	assert(m_type == Type::Array);
	m_value.array->emplace_back(value);
}

void Value::emplace(const std::string& key, Value value)
{
	// Implicitly convert null to an object
	if (m_type == Type::Null) {
		m_type = Type::Object;
		m_value.object = new Object;
	}

	assert(m_type == Type::Object);
	m_value.object->emplace(key, value);
}

bool Value::exists(size_t index) const
{
	return index < size();
}

bool Value::exists(const std::string& key) const
{
	assert(m_type == Type::Object);
	return m_value.object->members().find(key) != m_value.object->members().end();
}

// ------------------------------------------

Value& Value::operator[](size_t index)
{
	// Implicitly convert null to an array
	if (m_type == Type::Null) {
		m_type = Type::Array;
		m_value.array = new Array;
	}

	assert(m_type == Type::Array);
	return (*m_value.array)[index];
}

Value& Value::operator[](const std::string& key)
{
	// Implicitly convert null to an object
	if (m_type == Type::Null) {
		m_type = Type::Object;
		m_value.object = new Object;
	}

	assert(m_type == Type::Object);
	return (*m_value.object)[key];
}

const Value& Value::operator[](size_t index) const
{
	assert(m_type == Type::Array);
	return (*m_value.array)[index];
}

const Value& Value::operator[](const std::string& key) const
{
	assert(m_type == Type::Object);
	return (*m_value.object)[key];
}

Value& Value::at(size_t index)
{
	assert(m_type == Type::Array);
	return m_value.array->at(index);
}

Value& Value::at(const std::string& key)
{
	assert(m_type == Type::Object);
	return m_value.object->at(key);
}

const Value& Value::at(size_t index) const
{
	assert(m_type == Type::Array);
	return m_value.array->at(index);
}

const Value& Value::at(const std::string& key) const
{
	assert(m_type == Type::Object);
	return m_value.object->at(key);
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
		return m_value.array->size();
	case Type::Object:
		return m_value.object->size();
	default:
		return 1;
	}
}

// ------------------------------------------

void Value::create()
{
	switch (m_type) {
	case Type::Bool:
		m_value.boolean = false;
		break;
	case Type::Number:
		m_value.number = 0.0;
		break;
	case Type::String:
		m_value.string = new std::string;
		break;
	case Type::Array:
		m_value.array = new Array;
		break;
	case Type::Object:
		m_value.object = new Object;
		break;
	default:
		break;
	}
}

void Value::clear()
{
	switch (m_type) {
	case Type::String:
		delete m_value.string;
		break;
	case Type::Array:
		delete m_value.array;
		break;
	case Type::Object:
		delete m_value.object;
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
		m_value.boolean = other.m_value.boolean;
		break;
	case Type::Number:
		m_value.number = other.m_value.number;
		break;
	case Type::String:
		m_value.string = new std::string(*other.m_value.string);
		break;
	case Type::Array:
		m_value.array = new Array(*other.m_value.array);
		break;
	case Type::Object:
		m_value.object = new Object(*other.m_value.object);
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
