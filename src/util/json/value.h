/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_VALUE_H
#define JSON_VALUE_H

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

	Value() {}
	virtual ~Value() { clear(); }

	// Copy constructor
	Value(const Value& other);
	// Assignment operator
	Value& operator=(const Value& other);

	Value(bool value);
	Value(double value);
	Value(const char* value);
	Value(const std::string& value);
	Value(const Array& value);
	Value(const Object& value);

	// Array index operator
	Value operator[](size_t index);
	Value operator[](const std::string& key);

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
	} m_value;
};

} // namespace Json

#endif // JSON_VALUE_H
