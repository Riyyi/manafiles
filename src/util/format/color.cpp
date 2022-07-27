/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdint> // uint8_t
#include <cstdio>  // FILE, fputs, stdout
#include <sstream> // stringstream
#include <string>
#include <string_view>

#include "util/format/color.h"
#include "util/format/format.h"
#include "util/meta/assert.h"

namespace Util::Format {

TextStyle::TextStyle(Emphasis emphasis)
	: m_emphasis(emphasis)
{
}

TextStyle::TextStyle(bool isForeground, TerminalColor color)
{
	if (isForeground) {
		m_foregroundColor = color;
	}
	else {
		m_backgroundColor = color;
	}
}

// -----------------------------------------

TextStyle& TextStyle::operator|=(const TextStyle& rhs)
{
	if (m_foregroundColor == TerminalColor::None) {
		m_foregroundColor = rhs.m_foregroundColor;
	}
	else {
		VERIFY(rhs.m_foregroundColor == TerminalColor::None, "can't OR a terminal color");
	}

	if (m_backgroundColor == TerminalColor::None) {
		m_backgroundColor = rhs.m_backgroundColor;
	}
	else {
		VERIFY(rhs.m_backgroundColor == TerminalColor::None, "can't OR a terminal color");
	}

	m_emphasis = static_cast<Emphasis>(static_cast<uint8_t>(m_emphasis) | static_cast<uint8_t>(rhs.m_emphasis));

	return *this;
}

TextStyle fg(TerminalColor foreground)
{
	return TextStyle(true, foreground);
}

TextStyle bg(TerminalColor background)
{
	return TextStyle(false, background);
}

TextStyle operator|(TextStyle lhs, const TextStyle& rhs)
{
	return lhs |= rhs;
}

TextStyle operator|(Emphasis lhs, Emphasis rhs)
{
	return TextStyle { lhs } | rhs;
}

bool operator&(Emphasis lhs, Emphasis rhs)
{
	return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}

// -----------------------------------------

void setDisplayAttributes(std::stringstream& stream, const TextStyle& style)
{
	bool hasForeground = style.foregroundColor() != TerminalColor::None;
	bool hasBackground = style.backgroundColor() != TerminalColor::None;
	bool hasEmphasis = style.emphasis() != Emphasis::None;
	if (!hasForeground && !hasBackground && !hasEmphasis) {
		return;
	}

	stream.write("\033[", 2);

	if (hasForeground) {
		stream << format("{}", static_cast<uint8_t>(style.foregroundColor()));
	}

	if (hasBackground) {
		if (hasForeground) {
			stream.write(";", 1);
		}
		stream << format("{}", static_cast<uint8_t>(style.backgroundColor()) + 10);
	}

	stream.write("m", 1);

	if (hasEmphasis) {

#define ESCAPE_ATTRIBUTE(escape, attribute) \
	if (style.emphasis() & escape) {        \
		stream.write("\033[", 2);           \
		stream.write(attribute, 1);         \
		stream.write("m", 1);               \
	}

		ESCAPE_ATTRIBUTE(Emphasis::Bold, "1");
		ESCAPE_ATTRIBUTE(Emphasis::Faint, "2");
		ESCAPE_ATTRIBUTE(Emphasis::Italic, "3");
		ESCAPE_ATTRIBUTE(Emphasis::Underline, "4");
		ESCAPE_ATTRIBUTE(Emphasis::Blink, "5");
		ESCAPE_ATTRIBUTE(Emphasis::Reverse, "7");
		ESCAPE_ATTRIBUTE(Emphasis::Conceal, "8");
		ESCAPE_ATTRIBUTE(Emphasis::Strikethrough, "9");
	}
}

void coloredVariadicFormat(std::stringstream& stream, const TextStyle& style, std::string_view format, TypeErasedParameters& parameters)
{
	setDisplayAttributes(stream, style);
	variadicFormat(stream, format, parameters);
	stream.write("\033[0m", 4);
}

// -----------------------------------------

void coloredVariadicPrint(FILE* file, const TextStyle& style, std::string_view format, TypeErasedParameters& parameters)
{
	std::stringstream stream;
	setDisplayAttributes(stream, style);
	variadicFormat(stream, format, parameters);
	stream.write("\033[0m", 4);

	std::string string = stream.str();
	fputs(string.c_str(), file);
}

// -----------------------------------------

ColorPrintOperatorStyle::ColorPrintOperatorStyle(FILE* file, const TextStyle& style)
	: m_file(file)
	, m_style(style)
	, m_stream()
	, m_builder(m_stream)
{
	setDisplayAttributes(m_stream, style);
}

ColorPrintOperatorStyle::~ColorPrintOperatorStyle()
{
	m_stream.write("\033[0m", 4);
	std::string string = m_stream.str();
	fputs(string.c_str(), m_file);
}

ColorPrintOperatorStyle print(const TextStyle& style)
{
	return ColorPrintOperatorStyle(stdout, style);
}

ColorPrintOperatorStyle print(FILE* file, const TextStyle& style)
{
	return ColorPrintOperatorStyle(file, style);
}

} // namespace Util::Format
