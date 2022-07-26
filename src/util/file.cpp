/*
 * Copyright (C) 2021-2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdint> // int32_t
#include <filesystem>
#include <fstream> // ifstream, ios, ofstream
#include <memory>  // make_unique
#include <string>

#include "util/file.h"
#include "util/meta/assert.h"

namespace Util {

File::File(const std::string& path)
	: m_path(path)
{
	// Create input stream object and open file
	std::ifstream file(path, std::ios::in);
	VERIFY(file.is_open(), "failed to open file: '{}'", path);

	// Get length of the file
	file.seekg(0, std::ios::end);
	int32_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	VERIFY(size != -1, "failed to read file length: '{}', path");

	// Allocate memory filled with zeros
	auto buffer = std::make_unique<char[]>(size);

	// Fill buffer with file contents
	file.read(buffer.get(), size);
	file.close();

	m_data = std::string(buffer.get(), size);
}

File::~File()
{
}

// -----------------------------------------

File File::create(const std::string& path)
{
	if (!std::filesystem::exists(path)) {
		std::ofstream { path };
	}

	return Util::File(path);
}

// -----------------------------------------

void File::clear()
{
	m_data.clear();
}

File& File::append(const std::string& data)
{
	m_data.append(data);

	return *this;
}

File& File::replace(size_t index, size_t length, const std::string& data)
{
	m_data.replace(index, length, data);

	return *this;
}

File& File::flush()
{
	// Create output stream object and open file
	std::ofstream file(m_path, std::ios::out | std::ios::trunc);
	VERIFY(file.is_open(), "failed to open file: '{}'", m_path);

	// Write data to disk
	file.write(m_data.c_str(), m_data.size());

	return *this;
}

} // namespace Util
