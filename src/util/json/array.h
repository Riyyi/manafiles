/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <utility> // move
#include <vector>

#include "util/json/parser.h"

namespace Util::JSON {

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

	// Capacity

	bool empty() const { return m_elements.empty(); }
	size_t size() const { return m_elements.size(); }
	void reserve(size_t size) { m_elements.reserve(size); }

	// Element access

	Value& operator[](size_t index);

	Value& at(size_t index) { return m_elements.at(index); }
	const Value& at(size_t index) const { return m_elements.at(index); }

	const std::vector<Value>& elements() const { return m_elements; }

	// Modifiers

	void clear() { m_elements.clear(); }
	void emplace_back(Value element);

private:
	std::vector<Value> m_elements;
};

} // namespace Util::JSON
