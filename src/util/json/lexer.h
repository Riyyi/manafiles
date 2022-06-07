/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_LEXER_H
#define JSON_LEXER_H

// The JavaScript Object Notation (JSON) Data Interchange Format
// home/rick/code/cpp/manafiles/ https://www.rfc-editor.org/rfc/pdfrfc/rfc8259.txt.pdf

#include <cstddef> // size_t
#include <string>
#include <vector>

namespace Json {

struct Token {
	enum class Type {
		None,
		BraceOpen,    // {
		BraceClose,   // }
		BracketOpen,  // [
		BracketClose, // ]
		Colon,        // :
		Comma,        // ,
		String,       // "foobar"
		Number,       // 123.456
		Literal,      // false/null/true (case sensitive)
	};

	Type type { Type::None };
	size_t line { 0 };
	size_t column { 0 };
	std::string symbol;
};

// Lexical analyzer
class Lexer {
public:
	Lexer(const std::string& input);
	virtual ~Lexer();

	void analyze();

	const std::vector<Token>& tokens() const { return m_tokens; }

private:
	char peek();
	char peekNext();

	char consume();
	bool consumeSpecific(char character);

	bool getString();
	bool getNumber();
	bool getLiteral();

	std::string m_input;
	size_t m_index { 0 };
	size_t m_column { 1 };
	size_t m_line { 1 };

	std::vector<Token> m_tokens;
};

} // namespace Json

#endif // JSON_LEXER_H
