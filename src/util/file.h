/*
 * Copyright (C) 2021-2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

namespace Util {

class File {
public:
	File(const std::string& path);
	virtual ~File();

	static File create(const std::string& path);

	void clear();
	File& append(const std::string& data);
	File& replace(size_t index, size_t length, const std::string& data);
	File& flush();

	const char* c_str() const { return m_data.c_str(); }
	const std::string& data() const { return m_data; }
	const std::string& path() const { return m_path; }

private:
	std::string m_path;
	std::string m_data;
};

} // namespace Util
