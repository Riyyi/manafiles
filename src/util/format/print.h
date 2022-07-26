/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio> // FILE

#include "util/format/format.h"
#include "util/timer.h"

namespace Util::Format {

static Util::Timer s_timer;

enum class Type {
	None,     // Foreground
	Info,     // Blue
	Warn,     // Yellow
	Critical, // Red
	Success,  // Green
	Comment,  // White
};

void prettyVariadicFormat(FILE* file, Type type, bool bold, std::string_view format, TypeErasedParameters& parameters);

#define FORMAT_FUNCTION(name, type, bold)                                                      \
	template<size_t N, typename... Parameters>                                                 \
	void name(const char(&format)[N] = "", const Parameters&... parameters)                    \
	{                                                                                          \
		VariadicParameters variadicParameters { parameters... };                               \
		prettyVariadicFormat(stdout, Type::type, bold, { format, N - 1 }, variadicParameters); \
	}                                                                                          \
	template<size_t N, typename... Parameters>                                                 \
	void name(FILE* file, const char(&format)[N] = "", const Parameters&... parameters)        \
	{                                                                                          \
		VariadicParameters variadicParameters { parameters... };                               \
		prettyVariadicFormat(file, Type::type, bold, { format, N - 1 }, variadicParameters);   \
	}

FORMAT_FUNCTION(dbgln, None, false);
FORMAT_FUNCTION(dbgbln, None, true);
FORMAT_FUNCTION(infoln, Info, false);
FORMAT_FUNCTION(infobln, Info, true);
FORMAT_FUNCTION(warnln, Warn, false);
FORMAT_FUNCTION(warnbln, Warn, true);
FORMAT_FUNCTION(criticalln, Critical, false);
FORMAT_FUNCTION(criticalbln, Critical, true);
FORMAT_FUNCTION(successln, Success, false);
FORMAT_FUNCTION(successbln, Success, true);
FORMAT_FUNCTION(commentln, Comment, false);
FORMAT_FUNCTION(commentbln, Comment, true);

// -----------------------------------------

class FormatPrint {
public:
	FormatPrint(FILE* file, Type type, bool bold);
	virtual ~FormatPrint();

	Builder& builder() { return m_builder; }

private:
	FILE* m_file;
	Type m_type;
	bool m_bold;

	std::stringstream m_stream;
	Builder m_builder;
};

template<typename T>
const FormatPrint& operator<<(const FormatPrint& formatPrint, const T& value)
{
	_format(const_cast<FormatPrint&>(formatPrint).builder(), value);
	return formatPrint;
}

FormatPrint dbg();
FormatPrint dbgb();
FormatPrint info();
FormatPrint infob();
FormatPrint warn();
FormatPrint warnb();
FormatPrint critical();
FormatPrint criticalb();
FormatPrint success();
FormatPrint successb();
FormatPrint comment();
FormatPrint commentb();

// -----------------------------------------

} // namespace Util::Format

namespace Util {

using Util::Format::commentbln;
using Util::Format::commentln;
using Util::Format::criticalbln;
using Util::Format::criticalln;
using Util::Format::dbgbln;
using Util::Format::dbgln;
using Util::Format::infobln;
using Util::Format::infoln;
using Util::Format::successbln;
using Util::Format::successln;
using Util::Format::warnbln;
using Util::Format::warnln;

using Util::Format::comment;
using Util::Format::commentb;
using Util::Format::critical;
using Util::Format::criticalb;
using Util::Format::dbg;
using Util::Format::dbgb;
using Util::Format::info;
using Util::Format::infob;
using Util::Format::success;
using Util::Format::successb;
using Util::Format::warn;
using Util::Format::warnb;

} // namespace Util
