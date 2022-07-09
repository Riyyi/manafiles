/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_VALUE_H
#define JSON_VALUE_H

#include <cstddef> // nullptr_t
#include <cstdint> // uint32_t
#include <initializer_list>
#include <iostream> // istream, ostream
#include <string>
#include <utility> // forward

#include "util/json/fromjson.h"
#include "util/json/tojson.h"

namespace Json {

class Array;
class Object;

class Value {
private:
	friend Detail::jsonConstructor;

public:
	enum class Type {
		Null,   // null       (case sensitive!)
		Bool,   // true/false (case sensitive!)
		Number, // 123
		String, // ""
		Array,  // []
		Object, // {}
	};

	// Constructors
	Value(std::nullptr_t = nullptr);
	Value(Type type);
	Value(const std::initializer_list<Value>& values);
	template<typename T>
	Value(T value)
	{
		toJson(*this, std::forward<T>(value));
	}

	// Destructor
	virtual ~Value() { clear(); }

	// Copy constructor
	Value(const Value& other);

	// Assignment operator
	Value& operator=(const Value& other);

	// --------------------------------------

	static Value parse(const std::string& input);
	std::string dump(const uint32_t indent = 0, const char indentCharacter = ' ') const;

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

	bool asBool() const { return m_value.asBool; }
	double asDouble() const { return m_value.asDouble; }
	const std::string& asString() const { return *m_value.asString; }
	const Array& asArray() const { return *m_value.asArray; }
	const Object& asObject() const { return *m_value.asObject; }

private:
	void create();
	void clear();
	void copyFrom(const Value& other);

	Type m_type { Type::Null };

	union {
		bool asBool;
		double asDouble;
		std::string* asString;
		Array* asArray;
		Object* asObject;
	} m_value {};
};

std::istream& operator>>(std::istream& input, Value& value);
std::ostream& operator<<(std::ostream& output, const Value& value);

} // namespace Json

#endif // JSON_VALUE_H
