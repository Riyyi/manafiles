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

std::string formatType(Type type)
{
	std::string output;

	formatTo(output, "[");
	switch (type) {
	case Type::Trace:
		formatTo(output, "trace");
		break;
	case Type::Debug:
		formatTo(output, fg(TerminalColor::Magenta), "debug");
		break;
	case Type::Success:
		formatTo(output, fg(TerminalColor::Green), "success");
		break;
	case Type::Info:
		formatTo(output, fg(TerminalColor::Blue), "info");
		break;
	case Type::Warn:
		formatTo(output, Emphasis::Bold | fg(TerminalColor::Yellow), "warn");
		break;
	case Type::Error:
		formatTo(output, Emphasis::Bold | fg(TerminalColor::Red), "error");
		break;
	case Type::Critical:
		formatTo(output, Emphasis::Bold | fg(TerminalColor::White) | bg(TerminalColor::Red), "critical");
		break;
	default:
		break;
	};
	formatTo(output, "] ");

	return output;
}

// -----------------------------------------

LogOperatorStyle::LogOperatorStyle(FILE* file, Type type)
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
	return LogOperatorStyle(stdout, Type::Trace);
}

LogOperatorStyle debug()
{
	return LogOperatorStyle(stdout, Type::Debug);
}

LogOperatorStyle success()
{
	return LogOperatorStyle(stdout, Type::Success);
}

LogOperatorStyle info()
{
	return LogOperatorStyle(stdout, Type::Info);
}

LogOperatorStyle warn()
{
	return LogOperatorStyle(stderr, Type::Warn);
}

LogOperatorStyle error()
{
	return LogOperatorStyle(stderr, Type::Error);
}

LogOperatorStyle critical()
{
	return LogOperatorStyle(stderr, Type::Critical);
}

} // namespace Util::Format
