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

class Array {
public:
	Array() {}
	virtual ~Array() {}

	Array(const Array& other)
		: m_values(other.m_values)
	{
	}

	void emplace(Value value)
	{
		m_values.emplace_back(std::move(value));
	}

	const std::vector<Value>& values() const { return m_values; }

private:
	std::vector<Value> m_values;
};

} // namespace Json


#endif // JSON_ARRAY_H
