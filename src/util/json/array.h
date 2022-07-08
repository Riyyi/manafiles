/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_ARRAY_H
#define JSON_ARRAY_H

#include <utility> // move
#include <vector>

#include "util/json/parser.h"

namespace Json {

class Value;

class Array {
public:
	Array() {}
	virtual ~Array() {}

	Array(const std::vector<Value>& values)
		: m_values(values)
	{}

	Array(const Array& other)
		: m_values(other.m_values)
	{
	}

	void emplace_back(Value value);

	Value& operator[](size_t index);

	Value& at(size_t index) { return m_values.at(index); }
	const Value& at(size_t index) const { return m_values.at(index); }

	size_t size() const { return m_values.size(); }
	const std::vector<Value>& values() const { return m_values; }

private:
	std::vector<Value> m_values;
};

} // namespace Json

#endif // JSON_ARRAY_H
