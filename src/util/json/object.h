/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <map>
#include <string>
#include <utility> // move

#include "util/json/parser.h"

namespace Util::JSON {

class Value;

class Object {
public:
	Object() {}
	virtual ~Object() {}

	Object(const Object& other)
		: m_members(other.m_members)
	{
	}

	// Capacity

	bool empty() const { return m_members.empty(); }
	size_t size() const { return m_members.size(); }

	// Member access

	Value& operator[](const std::string& name);

	Value& at(const std::string& name) { return m_members.at(name); }
	const Value& at(const std::string& name) const { return m_members.at(name); }

	const std::map<std::string, Value>& members() const { return m_members; }

	// Modifiers

	void clear() { m_members.clear(); }
	void emplace(const std::string& name, Value value);

private:
	std::map<std::string, Value> m_members;
};

} // namespace Util::JSON
