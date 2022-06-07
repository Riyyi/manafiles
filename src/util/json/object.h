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

	const std::map<std::string, Value>& members() const { return m_members; }

private:
	std::map<std::string, Value> m_members;
};

} // namespace Json

#endif // JSON_OBJECT_H
