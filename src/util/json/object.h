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

	void emplace(const std::string& key, Value value)
	{
		m_members.emplace(key, std::move(value));
	}

	Value& at(const std::string& key)
	{
		if (m_members.find(key) == m_members.end()) {
			emplace(key, {});
		}

		return m_members.at(key);
	}
	Value& operator[](const std::string& key) { return at(key); }
	const Value& at(const std::string& key) const { return m_members.at(key); }
	const Value& operator[](const std::string& key) const { return m_members.at(key); }

	const std::map<std::string, Value>& members() const { return m_members; }

private:
	std::map<std::string, Value> m_members;
};

} // namespace Json

#endif // JSON_OBJECT_H
