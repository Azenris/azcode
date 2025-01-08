
#pragma once

#include <string>
#include <vector>
#include <iosfwd>

#include "result_code.h"

enum class KeywordID
{
	False,
	True,
};

struct KeywordType
{
	KeywordID id;
	const char *name;
};

std::ostream & operator << ( std::ostream &out, const KeywordType &keywordType );
std::ostream & operator << ( std::ostream &out, const KeywordID &keywordID );

enum class TokenID
{
	Keyword,
	Identifier,
	StringLiteral,
	Number,
	Minus,
	Plus,
	Divide,
	DivideAssign,
	Assign,
	Equal,
	GreaterThan,
	GreaterOrEqual,
	LesserThan,
	LesserOrEqual,
	BitwiseAnd,
	LogicalAnd,
	BitwiseOr,
	LogicalOr,
	BitwiseNot,
	LogicalNot,
	ParenOpen,
	ParenClose,
	BlockOpen,
	BlockClose,
	Period,
	Comma,
	Colon,
	SemiColon,
	EndOfFile,
};

struct TokenType
{
	TokenID id;
	const char *name;
};

struct Token
{
	TokenID id;
	std::string valueString;
	int64_t valueNumber;
};

std::ostream & operator << ( std::ostream &out, const Token &token );

struct Lexer
{
	std::vector<Token> run( std::string data );
};