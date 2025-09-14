
#pragma once

#include <string>
#include <vector>
#include <iosfwd>

#include "ids.h"
#include "token_types.h"

struct Token
{
	TokenID id;
	Value value;
	i32 line;
	i32 file;
};

// --------------------------------------------------------------------

template <>
struct std::formatter<KeywordType>
{
	constexpr auto parse( std::format_parse_context &ctx )
	{
		return ctx.begin();
	}

	std::format_context::iterator format( const KeywordType &keywordType, std::format_context &ctx ) const
	{
		return std::format_to( ctx.out(), "Keyword: {}", Keywords[ static_cast<i32>( keywordType.id ) ].name );
	}
};

template <>
struct std::formatter<KeywordID>
{
	constexpr auto parse( std::format_parse_context &ctx )
	{
		return ctx.begin();
	}

	std::format_context::iterator format( const KeywordID &keywordID, std::format_context &ctx ) const
	{
		return std::format_to( ctx.out(), "Keyword: {}", Keywords[ static_cast<i32>( keywordID ) ].name );
	}
};

template <>
struct std::formatter<Token>
{
	constexpr auto parse( std::format_parse_context &ctx )
	{
		return ctx.begin();
	}

	std::format_context::iterator format( const Token &token, std::format_context &ctx ) const
	{
		const TokenType *tokenType = &TokenTypes[ static_cast<i32>( token.id ) ];

		switch ( token.id )
		{
		case TokenID::Keyword:
		case TokenID::Identifier:
		case TokenID::StringLiteral:
		case TokenID::Number:
			return std::format_to( ctx.out(), "Token: {} '{}' [line: {}]", tokenType->name, token.value, token.line );

		case TokenID::EndOfFile:
			return std::format_to( ctx.out(), "Token: EOF [line: {}]", token.line );
		}

		return std::format_to( ctx.out(), "Token: {} '{}' [line: {}]", tokenType->name, tokenType->symbol, token.line );
	}
};

template <>
struct std::formatter<TokenID>
{
	constexpr auto parse( std::format_parse_context &ctx )
	{
		return ctx.begin();
	}

	std::format_context::iterator format( const TokenID &TokenID, std::format_context &ctx ) const
	{
		return std::format_to( ctx.out(), "Token: {}", TokenTypes[ static_cast<i32>( TokenID ) ].name );
	}
};