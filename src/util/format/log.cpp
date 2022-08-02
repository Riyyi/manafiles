/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio> // FILE
#include <string>

#include "util/format/color.h"
#include "util/format/log.h"

namespace Util::Format {

std::string formatTimeElapsed()
{
	return format("{:.3}s ", s_timer.elapsedNanoseconds() / 1000000000.0);
}

std::string formatType(LogType type)
{
	std::string output;

	formatTo(output, "[");
	switch (type) {
	case LogType::Trace:
		formatTo(output, "trace");
		break;
	case LogType::Debug:
		formatTo(output, fg(TerminalColor::Magenta), "debug");
		break;
	case LogType::Success:
		formatTo(output, fg(TerminalColor::Green), "success");
		break;
	case LogType::Info:
		formatTo(output, fg(TerminalColor::Blue), "info");
		break;
	case LogType::Warn:
		formatTo(output, Emphasis::Bold | fg(TerminalColor::Yellow), "warn");
		break;
	case LogType::Error:
		formatTo(output, Emphasis::Bold | fg(TerminalColor::Red), "error");
		break;
	case LogType::Critical:
		formatTo(output, Emphasis::Bold | fg(TerminalColor::White) | bg(TerminalColor::Red), "critical");
		break;
	default:
		break;
	};
	formatTo(output, "] ");

	return output;
}

// -----------------------------------------

LogOperatorStyle::LogOperatorStyle(FILE* file, LogType type)
	: m_file(file)
	, m_type(type)
	, m_stream()
	, m_builder(m_stream)
{
	m_stream << formatTimeElapsed();
	m_stream << formatType(type);
}

LogOperatorStyle::~LogOperatorStyle()
{
	m_stream.write("\n", 1);
	std::string string = m_stream.str();
	fputs(string.c_str(), m_file);
}

LogOperatorStyle trace()
{
	return LogOperatorStyle(stdout, LogType::Trace);
}

LogOperatorStyle debug()
{
	return LogOperatorStyle(stdout, LogType::Debug);
}

LogOperatorStyle success()
{
	return LogOperatorStyle(stdout, LogType::Success);
}

LogOperatorStyle info()
{
	return LogOperatorStyle(stdout, LogType::Info);
}

LogOperatorStyle warn()
{
	return LogOperatorStyle(stderr, LogType::Warn);
}

LogOperatorStyle error()
{
	return LogOperatorStyle(stderr, LogType::Error);
}

LogOperatorStyle critical()
{
	return LogOperatorStyle(stderr, LogType::Critical);
}

} // namespace Util::Format
