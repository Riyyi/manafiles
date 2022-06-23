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

namespace Json {

class Array;
class Object;

class Value {
public:
	enum class Type {
		Null,   // null       (case sensitive!)
		Bool,   // true/false (case sensitive!)
		Number, // 123
		String, // ""
		Array,  // []
		Object, // {}
	};

	Value(std::nullptr_t = nullptr) {}
	virtual ~Value() { clear(); }

	// Copy constructor
	Value(const Value& other);
	// Assignment operator
	Value& operator=(const Value& other);

	Value(bool value);
	Value(int value);
	Value(double value);
	Value(const char* value);
	Value(const std::string& value);
	Value(const Array& value);
	Value(const Object& value);
	Value(const std::initializer_list<Value>& values);

	// ------------------------------------------

	static Value parse(const std::string& input);
	std::string dump(const uint32_t indent = 0, const char indentCharacter = ' ') const;

	// ------------------------------------------

	// Array index operator
	Value& operator[](size_t index);
	Value& operator[](const std::string& key);
	const Value& operator[](size_t index) const;
	const Value& operator[](const std::string& key) const;

	// ------------------------------------------

	Type type() const { return m_type; }

	bool asBool() const { return m_value.asBool; }
	double asDouble() const { return m_value.asDouble; }
	const std::string& asString() const { return *m_value.asString; }
	const Array& asArray() const { return *m_value.asArray; }
	const Object& asObject() const { return *m_value.asObject; }

private:
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
