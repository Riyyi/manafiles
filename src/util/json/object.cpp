/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include "util/json/object.h"
#include "util/json/value.h"

namespace Util::JSON {

void Object::emplace(const std::string& name, Value value)
{
	m_members.emplace(name, std::move(value));
}

Value& Object::operator[](const std::string& name)
{
	if (m_members.find(name) == m_members.end()) {
		emplace(name, {});
	}

	return m_members.at(name);
}

} // namespace Util::JSON
