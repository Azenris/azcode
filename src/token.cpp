
#include <iostream>
#include <unordered_map>

#include "token.h"

std::ostream & operator << ( std::ostream &out, const KeywordType &keywordType )
{
	return out << "Keyword: " << Keywords[ static_cast<i32>( keywordType.id ) ].name;
}

std::ostream & operator << ( std::ostream &out, const KeywordID &keywordID )
{
	return out << "Keyword: " << Keywords[ static_cast<i32>( keywordID ) ].name;
}

static std::unordered_map<std::string, KeywordID> KeywordsMap =
{
	{ "return", KeywordID::Return },
	{ "false", KeywordID::False },
	{ "true", KeywordID::True },
};

const KeywordType *get_keyword( const char *keyword )
{
	auto iter = KeywordsMap.find( keyword );
	if ( iter != KeywordsMap.end() )
		return &Keywords[ static_cast<i32>( iter->second ) ];
	return nullptr;
}

const KeywordType *get_keyword( const std::string &keyword )
{
	auto iter = KeywordsMap.find( keyword );
	if ( iter != KeywordsMap.end() )
		return &Keywords[ static_cast<i32>( iter->second ) ];
	return nullptr;
}

std::ostream & operator << ( std::ostream &out, const Token &token )
{
	const TokenType *tokenType = &TokenTypes[ static_cast<i32>( token.id ) ];

	out << "Token: " << tokenType->name;

	switch ( token.id )
	{
	case TokenID::Keyword:
	case TokenID::Identifier:
	case TokenID::StringLiteral:
	case TokenID::Number:
		out << " '" << token.value << "'";
		break;

	case TokenID::EndOfFile:
		break;

	default:
		out << " '" << tokenType->symbol << "'";
	}

	return out << " [line:" << token.line << "]";
}

std::ostream & operator << ( std::ostream &out, const TokenID &TokenID )
{
	return out << "Token: " << TokenTypes[ static_cast<i32>( TokenID ) ].name;
}