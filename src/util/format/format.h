/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <span>
#include <sstream> // stringstream
#include <string>
#include <string_view>

#include "util/format/formatter.h"

namespace Util::Format {

class Builder;
class Parser;

struct Parameter {
	const void* value;
	void (*format)(Builder& builder, Parser& parser, const void* value);
};

template<typename T>
void formatParameterValue(Builder& builder, Parser& parser, const void* value)
{
	Formatter<T> formatter;
	formatter.parse(parser);
	formatter.format(builder, *static_cast<const T*>(value));
}

// Type erasure improves both compile time and binary size significantly
class TypeErasedParameters {
public:
	const Parameter parameter(size_t index) { return m_parameters[index]; }

	size_t tell() const { return m_index; }
	size_t size() const { return m_parameters.size(); }
	bool isEOF() const { return m_index >= m_parameters.size(); }
	void ignore() { m_index++; }

protected:
	size_t m_index { 0 };
	std::span<const Parameter> m_parameters;
};

template<typename... Parameters>
class VariadicParameters final : public TypeErasedParameters {
public:
	VariadicParameters(const Parameters&... parameters)
		: m_templatedParameters({ { &parameters, formatParameterValue<Parameters> }... })
	{
		m_parameters = m_templatedParameters;
	}

private:
	std::array<Parameter, sizeof...(Parameters)> m_templatedParameters;
};

// -----------------------------------------

void variadicFormatImpl(Builder& builder, Parser& parser, TypeErasedParameters& parameters);
void variadicFormat(std::stringstream& stream, std::string_view format, TypeErasedParameters& parameters);

// -----------------------------------------

template<typename... Parameters>
std::string format(std::string_view format, const Parameters&... parameters)
{
	std::stringstream stream;
	VariadicParameters variadicParameters { parameters... };
	variadicFormat(stream, format, variadicParameters);
	return stream.str();
}

template<typename... Parameters>
void formatTo(std::string& output, std::string_view format, const Parameters&... parameters)
{
	std::stringstream stream;
	VariadicParameters variadicParameters { parameters... };
	variadicFormat(stream, format, variadicParameters);
	output += stream.str();
}

// -----------------------------------------

class FormatOperatorStyle {
public:
	FormatOperatorStyle(std::string& output);
	virtual ~FormatOperatorStyle();

	Builder& builder() { return m_builder; }

private:
	std::string& m_output;
	std::stringstream m_stream;
	Builder m_builder;
};

template<typename T>
const FormatOperatorStyle& operator<<(const FormatOperatorStyle& formatOperatorStyle, const T& value)
{
	_format(const_cast<FormatOperatorStyle&>(formatOperatorStyle).builder(), value);
	return formatOperatorStyle;
}

FormatOperatorStyle formatTo(std::string& output);

} // namespace Util::Format

namespace Util {

using Util::Format::format;
using Util::Format::formatTo;

} // namespace Util
