/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <string_view>

namespace Util {

class Shell {
public:
	Shell();
	virtual ~Shell() {}

	Shell operator()(const char* command);
	Shell operator()(std::string command);
	Shell operator()(std::string_view command);

	std::string output() const { return m_output; }
	int status() const { return m_status; }

private:
	Shell(const std::string& output, int status);

	std::string m_output;
	int m_status { 0 };
};

} // namespace Util
