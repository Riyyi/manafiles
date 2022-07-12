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

	Array(const std::vector<Value>& elements)
		: m_elements(elements)
	{}

	Array(const Array& other)
		: m_elements(other.m_elements)
	{
	}

	void emplace_back(Value element);
	void reserve(size_t size) { m_elements.reserve(size); }

	Value& operator[](size_t index);

	Value& at(size_t index) { return m_elements.at(index); }
	const Value& at(size_t index) const { return m_elements.at(index); }

	size_t size() const { return m_elements.size(); }
	const std::vector<Value>& elements() const { return m_elements; }

private:
	std::vector<Value> m_elements;
};

} // namespace Json

#endif // JSON_ARRAY_H
