
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

const KeywordType *get_keyword( const char *keyword );
const KeywordType *get_keyword( const std::string &keyword );

enum class TokenID
{
	Keyword,
	Identifier,
	StringLiteral,
	Number,
	Minus,
	Plus,
	Divide,
	Asterisk,
	MinusAssign,
	PlusAssign,
	DivideAssign,
	AsteriskAssign,
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
	BraceOpen,
	BraceClose,
	Period,
	Comma,
	Colon,
	SemiColon,
	NewLine,
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
	Value value;
};

std::ostream & operator << ( std::ostream &out, const Token &token );
std::ostream & operator << ( std::ostream &out, const TokenID &TokenID );

constexpr KeywordType Keywords[] =
{
	{
		.id = KeywordID::False,
		.name = "False",
	},
	{
		.id = KeywordID::True,
		.name = "True",
	},
};

constexpr TokenType TokenTypes[] =
{
	{
		.id = TokenID::Keyword,
		.name = "Keyword",
	},
	{
		.id = TokenID::Identifier,
		.name = "Identifier",
	},
	{
		.id = TokenID::StringLiteral,
		.name = "StringLiteral",
	},
	{
		.id = TokenID::Number,
		.name = "Number",
	},
	{
		.id = TokenID::Minus,
		.name = "Minus",
	},
	{
		.id = TokenID::Plus,
		.name = "Plus",
	},
	{
		.id = TokenID::Divide,
		.name = "Divide",
	},
	{
		.id = TokenID::Asterisk,
		.name = "Asterisk",
	},
	{
		.id = TokenID::MinusAssign,
		.name = "MinusAssign",
	},
	{
		.id = TokenID::PlusAssign,
		.name = "PlusAssign",
	},
	{
		.id = TokenID::DivideAssign,
		.name = "DivideAssign",
	},
	{
		.id = TokenID::AsteriskAssign,
		.name = "AsteriskAssign",
	},
	{
		.id = TokenID::Assign,
		.name = "Assign",
	},
	{
		.id = TokenID::Equal,
		.name = "Equal",
	},
	{
		.id = TokenID::GreaterThan,
		.name = "GreaterThan",
	},
	{
		.id = TokenID::GreaterOrEqual,
		.name = "GreaterOrEqual",
	},
	{
		.id = TokenID::LesserThan,
		.name = "LesserThan",
	},
	{
		.id = TokenID::LesserOrEqual,
		.name = "LesserOrEqual",
	},
	{
		.id = TokenID::BitwiseAnd,
		.name = "BitwiseAnd",
	},
	{
		.id = TokenID::LogicalAnd,
		.name = "LogicalAnd",
	},
	{
		.id = TokenID::BitwiseOr,
		.name = "BitwiseOr",
	},
	{
		.id = TokenID::LogicalOr,
		.name = "LogicalOr",
	},
	{
		.id = TokenID::LogicalNot,
		.name = "LogicalNot",
	},
	{
		.id = TokenID::BitwiseNot,
		.name = "BitwiseNot",
	},
	{
		.id = TokenID::ParenOpen,
		.name = "ParenOpen",
	},
	{
		.id = TokenID::ParenClose,
		.name = "ParenClose",
	},
	{
		.id = TokenID::BraceOpen,
		.name = "BraceOpen",
	},
	{
		.id = TokenID::BraceClose,
		.name = "BraceClose",
	},
	{
		.id = TokenID::Period,
		.name = "Period",
	},
	{
		.id = TokenID::Comma,
		.name = "Comma",
	},
	{
		.id = TokenID::Colon,
		.name = "Colon",
	},
	{
		.id = TokenID::SemiColon,
		.name = "SemiColon",
	},
	{
		.id = TokenID::NewLine,
		.name = "NewLine",
	},
	{
		.id = TokenID::EndOfFile,
		.name = "EndOfFile",
	},
};