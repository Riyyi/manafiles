/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdio> // FILE, stderr, stdout
#include <string>
#include <string_view>

#include "util/format/format.h"
#include "util/format/print.h"
#include "util/timer.h"

namespace Util::Format {

static Util::Timer s_timer;

enum class LogType : uint8_t {
	Trace,    // White
	Debug,    // Purple
	Success,  // Green
	Info,     // Blue
	Warn,     // Bold yellow
	Error,    // Bold red
	Critical, // Bold on red
};

std::string formatTimeElapsed();
std::string formatType(LogType type);

#define LOG_FUNCTION(name, file, type)                                 \
	template<size_t N, typename... Parameters>                         \
	void name(const char(&format)[N], const Parameters&... parameters) \
	{                                                                  \
		print(file, "{}", formatTimeElapsed());                        \
		print(file, "{}", formatType(type));                           \
		VariadicParameters variadicParameters { parameters... };       \
		print(file, format, variadicParameters);                       \
		print(file, "\n");                                             \
	}

LOG_FUNCTION(trace, stdout, LogType::Trace);
LOG_FUNCTION(debug, stdout, LogType::Debug);
LOG_FUNCTION(success, stdout, LogType::Success);
LOG_FUNCTION(info, stdout, LogType::Info);
LOG_FUNCTION(warn, stderr, LogType::Warn);
LOG_FUNCTION(error, stderr, LogType::Error);
LOG_FUNCTION(critical, stderr, LogType::Critical);

// -----------------------------------------

class LogOperatorStyle {
public:
	LogOperatorStyle(FILE* file, LogType type);
	virtual ~LogOperatorStyle();

	Builder& builder() { return m_builder; }

private:
	FILE* m_file;
	LogType m_type;

	std::stringstream m_stream;
	Builder m_builder;
};

template<typename T>
const LogOperatorStyle& operator<<(const LogOperatorStyle& logOperatorStyle, const T& value)
{
	_format(const_cast<LogOperatorStyle&>(logOperatorStyle).builder(), value);
	return logOperatorStyle;
}

LogOperatorStyle trace();
LogOperatorStyle debug();
LogOperatorStyle success();
LogOperatorStyle info();
LogOperatorStyle warn();
LogOperatorStyle error();
LogOperatorStyle critical();

} // namespace Util::Format

namespace Util {

using Util::Format::critical;
using Util::Format::debug;
using Util::Format::error;
using Util::Format::info;
using Util::Format::success;
using Util::Format::trace;
using Util::Format::warn;

} // namespace Util
