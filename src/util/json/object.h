/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_OBJECT_H
#define JSON_OBJECT_H

#include <map>
#include <string>
#include <utility> // move

#include "util/json/parser.h"

namespace Json {

class Value;

class Object {
public:
	Object() {}
	virtual ~Object() {}

	Object(const Object& other)
		: m_members(other.m_members)
	{
	}

	void clear() { m_members.clear(); }
	void emplace(const std::string& name, Value value);

	Value& operator[](const std::string& name);

	Value& at(const std::string& name) { return m_members.at(name); }
	const Value& at(const std::string& name) const { return m_members.at(name); }

	size_t size() const { return m_members.size(); }
	const std::map<std::string, Value>& members() const { return m_members; }

private:
	std::map<std::string, Value> m_members;
};

} // namespace Json

#endif // JSON_OBJECT_H
