/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include "util/json/array.h"
#include "util/json/value.h"

namespace Json {

void Array::emplace_back(Value value)
{
	m_values.emplace_back(std::move(value));
}

Value& Array::at(size_t index)
{
	if (index + 1 > m_values.size()) {
		m_values.resize(index + 1);
	}

	return m_values.at(index);
}

} // namespace Json
