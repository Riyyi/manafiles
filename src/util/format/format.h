/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <sstream> // stringstream
#include <string>
#include <string_view>
#include <vector>

#include "util/format/builder.h"
#include "util/format/parser.h"
#include "util/format/toformat.h"

namespace Util::Format {

struct Parameter {
	const void* value;
	void (*format)(Builder& builder, const void* value);
};

template<typename T>
void formatParameterValue(Builder& builder, const void* value)
{
	format(builder, *static_cast<const T*>(value));
}

class TypeErasedParameters {
public:
	const Parameter parameter(size_t index) { return m_parameters[index]; }

	size_t tell() const { return m_index; }
	size_t size() const { return m_parameters.size(); }
	bool isEOF() const { return m_index >= m_parameters.size(); }
	void ignore() { m_index++; }

protected:
	size_t m_index { 0 };
	std::vector<Parameter> m_parameters;
};

template<typename... Parameters>
class VariadicParameters final : public TypeErasedParameters {
public:
	VariadicParameters(const Parameters&... parameters)
	{
		m_parameters = { { &parameters, formatParameterValue<Parameters> }... };
	}
};

// -----------------------------------------

void variadicFormatImpl(Builder& builder, Parser& parser, TypeErasedParameters& parameters);
void variadicFormat(std::stringstream& stream, std::string_view format, TypeErasedParameters& parameters);

// -----------------------------------------

enum class Type {
	None,    // Foreground
	Info,    // Blue
	Warn,    // Yellow
	Danger,  // Red
	Success, // Green
	Comment, // White
};

void prettyVariadicFormat(Type type, bool bold, std::string_view format, TypeErasedParameters& parameters);

#define FORMAT_FUNCTION(name, type, bold)                                              \
	template<size_t N, typename... Parameters>                                         \
	void name(const char(&format)[N] = "", const Parameters&... parameters)            \
	{                                                                                  \
		VariadicParameters variadicParameters { parameters... };                       \
		prettyVariadicFormat(Type::type, bold, { format, N - 1 }, variadicParameters); \
	}

FORMAT_FUNCTION(dbgln, None, false);
FORMAT_FUNCTION(dbgbln, None, true);
FORMAT_FUNCTION(infoln, Info, false);
FORMAT_FUNCTION(infobln, Info, true);
FORMAT_FUNCTION(warnln, Warn, false);
FORMAT_FUNCTION(warnbln, Warn, true);
FORMAT_FUNCTION(dangerln, Danger, false);
FORMAT_FUNCTION(dangerbln, Danger, true);
FORMAT_FUNCTION(successln, Success, false);
FORMAT_FUNCTION(successbln, Success, true);
FORMAT_FUNCTION(commentln, Comment, false);
FORMAT_FUNCTION(commentbln, Comment, true);

// -----------------------------------------

class Dbg {
public:
	Dbg(Type type, bool bold);
	virtual ~Dbg();

	Builder& builder() { return m_builder; }

private:
	Type m_type;
	bool m_bold;

	std::stringstream m_stream;
	Builder m_builder;
};

template<typename T>
const Dbg& operator<<(const Dbg& debug, const T& value)
{
	format(const_cast<Dbg&>(debug).builder(), value);
	return debug;
}

Dbg dbg();
Dbg dbgb();
Dbg info();
Dbg infob();
Dbg warn();
Dbg warnb();
Dbg danger();
Dbg dangerb();
Dbg success();
Dbg successb();
Dbg comment();
Dbg commentb();

// -----------------------------------------

template<typename... Parameters>
void strln(std::string& fill, std::string_view format, const Parameters&... parameters)
{
	std::stringstream stream;
	VariadicParameters variadicParameters { parameters... };
	variadicFormat(stream, format, variadicParameters);
	fill = stream.str();
}

class Str {
public:
	Str(std::string& fill);
	virtual ~Str();

	Builder& builder() { return m_builder; }

private:
	std::string& m_fill;
	std::stringstream m_stream;
	Builder m_builder;
};

template<typename T>
const Str& operator<<(const Str& string, const T& value)
{
	format(const_cast<Str&>(string).builder(), value);
	return string;
}

Str str(std::string& fill);

} // namespace Util::Format

using Util::Format::commentbln;
using Util::Format::commentln;
using Util::Format::dangerbln;
using Util::Format::dangerln;
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
using Util::Format::danger;
using Util::Format::dangerb;
using Util::Format::dbg;
using Util::Format::dbgb;
using Util::Format::info;
using Util::Format::infob;
using Util::Format::success;
using Util::Format::successb;
using Util::Format::warn;
using Util::Format::warnb;

using Util::Format::str;
using Util::Format::strln;

using FormatBuilder = Util::Format::Builder;
