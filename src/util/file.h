/*
 * Copyright (C) 2021 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef FILE_H
#define FILE_H

#include <string>

namespace Util {

class File {
public:
	File(const std::string& path);
	virtual ~File();

	void clear();
	File& append(std::string data);
	File& flush();

	const char* c_str() const { return m_data.c_str(); }
	const std::string& data() const { return m_data; }
	const std::string& path() const { return m_path; }

private:
	std::string m_path;
	std::string m_data;
};

} // namespace Util


#endif // FILE_H