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

FormatOperatorStyle::FormatOperatorStyle(std::string& output)
	: m_output(output)
	, m_stream()
	, m_builder(m_stream)
{
}

FormatOperatorStyle::~FormatOperatorStyle()
{
	m_output = m_stream.str();
}

FormatOperatorStyle formatTo(std::string& output)
{
	return FormatOperatorStyle(output);
}

} // namespace Util::Format
