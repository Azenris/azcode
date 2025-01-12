
#pragma once

#include <string>
#include <vector>
#include <iosfwd>

enum class KeywordID
{
	Return,
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

constexpr KeywordType Keywords[] =
{
	{
		.id = KeywordID::Return,
		.name = "Return",
	},
	{
		.id = KeywordID::False,
		.name = "False",
	},
	{
		.id = KeywordID::True,
		.name = "True",
	},
};

// TODO:
// BrokenBar
// Backtick

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
	Tilde,
	Amp,
	Pipe,
	Hat,
	Percent,
	Exclamation,
	MinusAssign,
	PlusAssign,
	DivideAssign,
	AsteriskAssign,
	TildeAssign,
	AmpAssign,
	PipeAssign,
	HatAssign,
	PercentAssign,
	ExclamationAssign,
	DoubleAmp,
	DoublePipe,
	Assign,
	Equal,
	GreaterThan,
	GreaterOrEqual,
	LesserThan,
	LesserOrEqual,
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
	const char *symbol;
};

std::ostream & operator << ( std::ostream &out, const TokenID &TokenID );

constexpr TokenType TokenTypes[] =
{
	{
		.id = TokenID::Keyword,
		.name = "Keyword",
		.symbol = nullptr,
	},
	{
		.id = TokenID::Identifier,
		.name = "Identifier",
		.symbol = nullptr,
	},
	{
		.id = TokenID::StringLiteral,
		.name = "StringLiteral",
		.symbol = nullptr,
	},
	{
		.id = TokenID::Number,
		.name = "Number",
		.symbol = nullptr,
	},
	{
		.id = TokenID::Minus,
		.name = "Minus",
		.symbol = "-",
	},
	{
		.id = TokenID::Plus,
		.name = "Plus",
		.symbol = "+",
	},
	{
		.id = TokenID::Divide,
		.name = "Divide",
		.symbol = "/",
	},
	{
		.id = TokenID::Asterisk,
		.name = "Asterisk",
		.symbol = "*",
	},
	{
		.id = TokenID::Tilde,
		.name = "Tilde",
		.symbol = "~",
	},
	{
		.id = TokenID::Amp,
		.name = "Amp",
		.symbol = "&",
	},
	{
		.id = TokenID::Pipe,
		.name = "Pipe",
		.symbol = "|",
	},
	{
		.id = TokenID::Hat,
		.name = "Hat",
		.symbol = "^",
	},
	{
		.id = TokenID::Percent,
		.name = "Percent",
		.symbol = "%",
	},
	{
		.id = TokenID::Exclamation,
		.name = "Exclamation",
		.symbol = "!",
	},
	{
		.id = TokenID::MinusAssign,
		.name = "MinusAssign",
		.symbol = "-=",
	},
	{
		.id = TokenID::PlusAssign,
		.name = "PlusAssign",
		.symbol = "+=",
	},
	{
		.id = TokenID::DivideAssign,
		.name = "DivideAssign",
		.symbol = "/=",
	},
	{
		.id = TokenID::AsteriskAssign,
		.name = "AsteriskAssign",
		.symbol = "*=",
	},
	{
		.id = TokenID::TildeAssign,
		.name = "TildeAssign",
		.symbol = "~=",
	},
	{
		.id = TokenID::AmpAssign,
		.name = "AmpAssign",
		.symbol = "&=",
	},
	{
		.id = TokenID::PipeAssign,
		.name = "PipeAssign",
		.symbol = "|=",
	},
	{
		.id = TokenID::HatAssign,
		.name = "HatAssign",
		.symbol = "^=",
	},
	{
		.id = TokenID::PercentAssign,
		.name = "PercentAssign",
		.symbol = "%=",
	},
	{
		.id = TokenID::ExclamationAssign,
		.name = "ExclamationAssign",
		.symbol = "!=",
	},
	{
		.id = TokenID::DoubleAmp,
		.name = "DoubleAmp",
		.symbol = "&&",
	},
	{
		.id = TokenID::DoublePipe,
		.name = "DoublePipe",
		.symbol = "||",
	},
	{
		.id = TokenID::Assign,
		.name = "Assign",
		.symbol = "=",
	},
	{
		.id = TokenID::Equal,
		.name = "Equal",
		.symbol = "==",
	},
	{
		.id = TokenID::GreaterThan,
		.name = "GreaterThan",
		.symbol = ">",
	},
	{
		.id = TokenID::GreaterOrEqual,
		.name = "GreaterOrEqual",
		.symbol = ">=",
	},
	{
		.id = TokenID::LesserThan,
		.name = "LesserThan",
		.symbol = "<",
	},
	{
		.id = TokenID::LesserOrEqual,
		.name = "LesserOrEqual",
		.symbol = "<=",
	},
	{
		.id = TokenID::ParenOpen,
		.name = "ParenOpen",
		.symbol = "(",
	},
	{
		.id = TokenID::ParenClose,
		.name = "ParenClose",
		.symbol = ")",
	},
	{
		.id = TokenID::BraceOpen,
		.name = "BraceOpen",
		.symbol = "{",
	},
	{
		.id = TokenID::BraceClose,
		.name = "BraceClose",
		.symbol = "}",
	},
	{
		.id = TokenID::Period,
		.name = "Period",
		.symbol = ".",
	},
	{
		.id = TokenID::Comma,
		.name = "Comma",
		.symbol = ",",
	},
	{
		.id = TokenID::Colon,
		.name = "Colon",
		.symbol = ":",
	},
	{
		.id = TokenID::SemiColon,
		.name = "SemiColon",
		.symbol = ";",
	},
	{
		.id = TokenID::NewLine,
		.name = "NewLine",
		.symbol = "\\n",
	},
	{
		.id = TokenID::EndOfFile,
		.name = "EndOfFile",
		.symbol = nullptr,
	},
};

struct Token
{
	TokenID id;
	Value value;
	i32 line;
};

std::ostream & operator << ( std::ostream &out, const Token &token );