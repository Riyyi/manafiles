/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_ARRAY_H
#define JSON_ARRAY_H

#include <initializer_list>
#include <utility> // move
#include <vector>

#include "util/json/parser.h"

namespace Json {

class Array {
public:
	Array() {}
	virtual ~Array() {}

	Array(const std::initializer_list<Value>& values)
		: m_values(values)
	{}

	Array(const Array& other)
		: m_values(other.m_values)
	{
	}

	void emplace_back(Value value)
	{
		m_values.emplace_back(std::move(value));
	}

	Value& at(size_t index)
	{
		if (index + 1 > m_values.size()) {
			m_values.resize(index + 1);
		}

		return m_values.at(index);
	}
	Value& operator[](size_t index) { return at(index); }
	const Value& at(size_t index) const { return m_values.at(index); }
	const Value& operator[](size_t index) const { return m_values.at(index); }

	size_t size() const { return m_values.size(); }
	const std::vector<Value>& values() const { return m_values; }

private:
	std::vector<Value> m_values;
};

} // namespace Json

#endif // JSON_ARRAY_H
