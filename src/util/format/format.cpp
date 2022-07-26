/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <sstream> // stringstream
#include <string>
#include <string_view>

#include "util/format/builder.h"
#include "util/format/format.h"
#include "util/format/parser.h"

namespace Util::Format {

void variadicFormatImpl(Builder& builder, Parser& parser, TypeErasedParameters& parameters)
{
	const auto literal = parser.consumeLiteral();
	builder.putLiteral(literal);

	if (!parameters.isEOF()) {
		std::string_view specifier;
		if (parser.consumeSpecifier(specifier)) {
			parser.applySpecifier(builder, specifier);
		}

		auto& parameter = parameters.parameter(parameters.tell());
		parameter.format(builder, parameter.value);
		parameters.ignore();

		builder.resetSpecifiers();
	}

	if (!parser.isEOF()) {
		variadicFormatImpl(builder, parser, parameters);
	}
}

void variadicFormat(std::stringstream& stream, std::string_view format, TypeErasedParameters& parameters)
{
	Builder builder { stream };
	Parser parser { format, parameters.size() };

	variadicFormatImpl(builder, parser, parameters);
}

// -----------------------------------------

void prettyVariadicFormat(Type type, bool bold, std::string_view format, TypeErasedParameters& parameters)
{
	std::stringstream stream;

	if (type != Type::None || bold) {
		stream << "\033[";
		switch (type) {
		case Type::Info:
			stream << "34";
			break;
		case Type::Warn:
			stream << "33";
			break;
		case Type::Critital:
			stream << "31";
			break;
		case Type::Success:
			stream << "32";
			break;
		case Type::Comment:
			stream << "37";
			break;
		default:
			break;
		};

		if (bold) {
			stream << ";1";
		}
		stream << "m";
	}

	variadicFormat(stream, format, parameters);

	if (type != Type::None || bold) {
		stream << "\033[0m";
	}

	printf("%s\n", stream.str().c_str());
}

// -----------------------------------------

Dbg::Dbg(Type type, bool bold)
	: m_type(type)
	, m_bold(bold)
	, m_stream()
	, m_builder(m_stream)
{
	if (type != Type::None || m_bold) {
		m_stream << "\033[";
		switch (type) {
		case Type::Info:
			m_stream << "34";
			break;
		case Type::Warn:
			m_stream << "33";
			break;
		case Type::Critical:
			m_stream << "31";
			break;
		case Type::Success:
			m_stream << "32";
			break;
		case Type::Comment:
			m_stream << "37";
			break;
		default:
			break;
		};

		if (m_bold) {
			m_stream << ";1";
		}
		m_stream << "m";
	}
}

Dbg::~Dbg()
{
	if (m_type != Type::None || m_bold) {
		m_stream << "\033[0m";
	}

	printf("%s", m_stream.str().c_str());
}

Dbg dbg()
{
	return Dbg(Type::None, false);
}

Dbg dbgb()
{
	return Dbg(Type::None, true);
}

Dbg info()
{
	return Dbg(Type::Info, false);
}

Dbg infob()
{
	return Dbg(Type::Info, true);
}

Dbg warn()
{
	return Dbg(Type::Warn, false);
}

Dbg warnb()
{
	return Dbg(Type::Warn, true);
}

Dbg critical()
{
	return Dbg(Type::Critical, false);
}

Dbg criticalb()
{
	return Dbg(Type::Critical, true);
}

Dbg success()
{
	return Dbg(Type::Success, false);
}

Dbg successb()
{
	return Dbg(Type::Success, true);
}

Dbg comment()
{
	return Dbg(Type::Comment, false);
}

Dbg commentb()
{
	return Dbg(Type::Comment, true);
}

// -----------------------------------------

Str::Str(std::string& fill)
	: m_fill(fill)
	, m_stream()
	, m_builder(m_stream)
{
}

Str::~Str()
{
	m_fill = m_stream.str();
}

Str str(std::string& fill)
{
	return Str(fill);
}

} // namespace Util::Format
