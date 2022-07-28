/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // nullptr_t, size_t
#include <cstdint> // uint8_t, uint32_t
#include <initializer_list>
#include <iostream> // istream, ostream
#include <string>
#include <utility> // forward

#include "util/format/builder.h"
#include "util/json/fromjson.h"
#include "util/json/tojson.h"

namespace Util::JSON {

class Array;
class Object;

class Value {
private:
	friend Detail::jsonConstructor;
	friend class Parser;
	friend class Serializer;

public:
	enum class Type : uint8_t {
		Null,   // null       (case sensitive!)
		Bool,   // true/false (case sensitive!)
		Number, // 123
		String, // ""
		Array,  // []
		Object, // {}
	};

	// --------------------------------------

	// Constructors
	Value(std::nullptr_t = nullptr);
	Value(Type type);
	Value(const std::initializer_list<Value>& values);
	template<typename T>
	Value(T value)
	{
		toJson(*this, std::forward<T>(value));
	}

	// Rule of Five:
	// Copy constructor
	Value(const Value& other);
	// Move constructor
	Value(Value&& other) noexcept;
	// Copy assignment
	// Move assignment
	Value& operator=(Value other);
	// Destructor
	virtual ~Value() { destroy(); }

	friend void swap(Value& left, Value& right) noexcept;

	// --------------------------------------

	static Value parse(std::string_view input);
	static Value parse(std::ifstream& file);
	std::string dump(const uint32_t indent = 0, const char indentCharacter = ' ') const;

	void clear();

	void emplace_back(Value value);
	void emplace(const std::string& key, Value value);

	bool exists(size_t index) const;
	bool exists(const std::string& key) const;

	// --------------------------------------

	// Array index operator
	Value& operator[](size_t index);
	Value& operator[](const std::string& key);
	const Value& operator[](size_t index) const;
	const Value& operator[](const std::string& key) const;

	Value& at(size_t index);
	Value& at(const std::string& key);
	const Value& at(size_t index) const;
	const Value& at(const std::string& key) const;

	// --------------------------------------

	template<typename T>
	T get() const
	{
		T type;
		fromJson(*this, type);
		return type;
	}

	template<typename T>
	void getTo(T& type) const
	{
		fromJson(*this, type);
	}

	// --------------------------------------

	Type type() const { return m_type; }
	size_t size() const;

	bool asBool() const { return m_value.boolean; }
	double asDouble() const { return m_value.number; }
	const std::string& asString() const { return *m_value.string; }
	const Array& asArray() const { return *m_value.array; }
	const Object& asObject() const { return *m_value.object; }

private:
	void destroy();

	Type m_type { Type::Null };

	union {
		bool boolean;
		double number;
		std::string* string;
		Array* array;
		Object* object;
	} m_value {};
};

std::istream& operator>>(std::istream& input, Value& value);
std::ostream& operator<<(std::ostream& output, const Value& value);

void format(Util::Format::Builder& builder, const Value& value);

} // namespace Util::JSON

/**
 * User-defined string literal
 *
 * Example usage: auto json = "[ 3.14, true, null ]"_json;
 */
inline Util::JSON::Value operator"" _json(const char* input, size_t length)
{
	return Util::JSON::Value::parse(std::string(input, length));
}
