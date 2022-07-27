/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <cstdio>  // FILE, stdout
#include <sstream> // stringstream
#include <string_view>

#include "util/format/format.h"

namespace Util::Format {

enum class TerminalColor : uint8_t {
	None = 0,
	Black = 30,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White,
	BrightBlack = 90,
	BrightRed,
	BrightGreen,
	BrightYellow,
	BrightBlue,
	BrightMagenta,
	BrightCyan,
	BrightWhite
};

// Bit field
enum class Emphasis : uint8_t {
	None = 0,               // Attribute 0
	Bold = 1,               // Attribute 1
	Faint = 1 << 1,         // Attribute 2
	Italic = 1 << 2,        // Attribute 3
	Underline = 1 << 3,     // Attribute 4
	Blink = 1 << 4,         // Attribute 5
	Reverse = 1 << 5,       // Attribute 7
	Conceal = 1 << 6,       // Attribute 8
	Strikethrough = 1 << 7, // Attribute 9
};

class TextStyle {
private:
	friend TextStyle fg(TerminalColor foreground);
	friend TextStyle bg(TerminalColor background);

public:
	TextStyle(Emphasis emphasis);

	// Operator pipe equal, reads the same way as +=
	TextStyle& operator|=(const TextStyle& rhs);

	TerminalColor foregroundColor() const { return m_foregroundColor; }
	TerminalColor backgroundColor() const { return m_backgroundColor; }
	Emphasis emphasis() const { return m_emphasis; }

private:
	TextStyle(bool isForeground, TerminalColor color);

	TerminalColor m_foregroundColor { TerminalColor::None };
	TerminalColor m_backgroundColor { TerminalColor::None };
	Emphasis m_emphasis { 0 };
};

TextStyle fg(TerminalColor foreground);
TextStyle bg(TerminalColor background);
TextStyle operator|(TextStyle lhs, const TextStyle& rhs);
TextStyle operator|(Emphasis lhs, Emphasis rhs);
bool operator&(Emphasis lhs, Emphasis rhs);

// -----------------------------------------

void coloredVariadicFormat(std::stringstream& stream, const TextStyle& style, std::string_view format, TypeErasedParameters& parameters);

template<typename... Parameters>
std::string format(const TextStyle& style, std::string_view format, const Parameters&... parameters)
{
	std::stringstream stream;
	VariadicParameters variadicParameters { parameters... };
	coloredVariadicFormat(stream, style, format, variadicParameters);
	return stream.str();
}

template<typename... Parameters>
void formatTo(std::string& output, const TextStyle& style, std::string_view format, const Parameters&... parameters)
{
	std::stringstream stream;
	VariadicParameters variadicParameters { parameters... };
	coloredVariadicFormat(stream, style, format, variadicParameters);
	output += stream.str();
}

// -----------------------------------------

void coloredVariadicPrint(FILE* file, const TextStyle& style, std::string_view format, TypeErasedParameters& parameters);

template<size_t N, typename... Parameters>
void print(const TextStyle& style, const char (&format)[N], const Parameters&... parameters)
{
	VariadicParameters variadicParameters { parameters... };
	coloredVariadicPrint(stdout, style, { format, N - 1 }, variadicParameters);
}

template<size_t N, typename... Parameters>
void print(FILE* file, const TextStyle& style, const char (&format)[N], const Parameters&... parameters)
{
	VariadicParameters variadicParameters { parameters... };
	coloredVariadicPrint(file, style, { format, N - 1 }, variadicParameters);
}

// -----------------------------------------

class ColorPrintOperatorStyle {
public:
	ColorPrintOperatorStyle(FILE* file, const TextStyle& style);
	virtual ~ColorPrintOperatorStyle();

	Builder& builder() { return m_builder; }

private:
	FILE* m_file;
	TextStyle m_style;

	std::stringstream m_stream;
	Builder m_builder;
};

template<typename T>
const ColorPrintOperatorStyle& operator<<(const ColorPrintOperatorStyle& colorPrintOperatorStyle, const T& value)
{
	_format(const_cast<ColorPrintOperatorStyle&>(colorPrintOperatorStyle).builder(), value);
	return colorPrintOperatorStyle;
}

ColorPrintOperatorStyle print(const TextStyle& style);
ColorPrintOperatorStyle print(FILE* file, const TextStyle& style);

} // namespace Util::Format

namespace Util {

using Util::Format::format;
using Util::Format::formatTo;
using Util::Format::print;

} // namespace Util
