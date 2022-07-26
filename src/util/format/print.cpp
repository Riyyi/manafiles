/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>  // FILE, fputs
#include <iomanip> // setprecision
#include <ios>     // defaultfloat, fixed
#include <sstream> // stringstream
#include <string>
#include <string_view>

#include "util/format/print.h"

namespace Util::Format {

void printTimeElapsedAndTypePrefix(std::stringstream& stream, Type type, bool bold)
{
	stream << std::fixed << std::setprecision(3)
		   << s_timer.elapsedNanoseconds() / 1000000000.0 << "s "
		   << std::defaultfloat << std::setprecision(6);

	stream << "[\033[";
	if (bold) {
		stream << "1";
	}
	switch (type) {
	case Type::None:
		stream << ";35mdebug";
		break;
	case Type::Info:
		stream << ";34minfo";
		break;
	case Type::Warn:
		stream << ";33mwarn";
		break;
	case Type::Critical:
		stream << ";31mcritical";
		break;
	case Type::Success:
		stream << ";32msuccess";
		break;
	case Type::Comment:
		stream << "mcomment";
		break;
	default:
		break;
	};
	stream << "\033[0m] ";
}

void prettyVariadicFormat(FILE* file, Type type, bool bold, std::string_view format, TypeErasedParameters& parameters)
{
	std::stringstream stream;

	printTimeElapsedAndTypePrefix(stream, type, bold);

	variadicFormat(stream, format, parameters);

	std::string string = stream.str();
	fputs(string.c_str(), file);
}

// -----------------------------------------

FormatPrint::FormatPrint(FILE* file, Type type, bool bold)
	: m_file(file)
	, m_type(type)
	, m_bold(bold)
	, m_stream()
	, m_builder(m_stream)
{
	printTimeElapsedAndTypePrefix(m_stream, type, bold);
}

FormatPrint::~FormatPrint()
{
	std::string string = m_stream.str();
	fputs(string.c_str(), m_file);
}

FormatPrint dbg()
{
	return FormatPrint(stdout, Type::None, false);
}

FormatPrint dbgb()
{
	return FormatPrint(stdout, Type::None, true);
}

FormatPrint info()
{
	return FormatPrint(stdout, Type::Info, false);
}

FormatPrint infob()
{
	return FormatPrint(stdout, Type::Info, true);
}

FormatPrint warn()
{
	return FormatPrint(stdout, Type::Warn, false);
}

FormatPrint warnb()
{
	return FormatPrint(stdout, Type::Warn, true);
}

FormatPrint critical()
{
	return FormatPrint(stderr, Type::Critical, false);
}

FormatPrint criticalb()
{
	return FormatPrint(stderr, Type::Critical, true);
}

FormatPrint success()
{
	return FormatPrint(stdout, Type::Success, false);
}

FormatPrint successb()
{
	return FormatPrint(stdout, Type::Success, true);
}

FormatPrint comment()
{
	return FormatPrint(stdout, Type::Comment, false);
}

FormatPrint commentb()
{
	return FormatPrint(stdout, Type::Comment, true);
}

} // namespace Util::Format
