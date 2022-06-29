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

class Object {
public:
	Object() {}
	virtual ~Object() {}

	Object(const Object& other)
		: m_members(other.m_members)
	{
	}

	void emplace(const std::string& name, Value value)
	{
		m_members.emplace(name, std::move(value));
	}

	Value& at(const std::string& name)
	{
		if (m_members.find(name) == m_members.end()) {
			emplace(name, {});
		}

		return m_members.at(name);
	}
	Value& operator[](const std::string& name) { return at(name); }
	const Value& at(const std::string& name) const { return m_members.at(name); }
	const Value& operator[](const std::string& name) const { return m_members.at(name); }

	size_t size() const { return m_members.size(); }
	const std::map<std::string, Value>& members() const { return m_members; }

private:
	std::map<std::string, Value> m_members;
};

} // namespace Json

#endif // JSON_OBJECT_H
