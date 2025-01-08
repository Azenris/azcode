
#pragma once

#include <string>
#include <vector>
#include <iosfwd>

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
	Number,
	Minus,
	Plus,
	Divide,
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
	QuoteOpen,
	QuoteClose,
	Comment,
	CommentBlockOpen,
	CommentBlockClose,
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