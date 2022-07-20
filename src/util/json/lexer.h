/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef JSON_LEXER_H
#define JSON_LEXER_H

// The JavaScript Object Notation (JSON) Data Interchange Format
// https://www.rfc-editor.org/rfc/pdfrfc/rfc8259.txt.pdf

#include <cstddef> // size_t
#include <string>
#include <vector>

#include "util/genericlexer.h"

namespace Json {

class Job;

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
class Lexer final : public Util::GenericLexer {
public:
	Lexer(Job* job);
	virtual ~Lexer();

	void analyze();

private:
	bool consumeString();
	bool consumeNumberOrLiteral(Token::Type type);
	bool consumeNumber();
	bool consumeLiteral();

	Job* m_job { nullptr };

	size_t m_column { 0 };
	size_t m_line { 0 };

	std::vector<Token>* m_tokens { nullptr };
};

} // namespace Json

#endif // JSON_LEXER_H
