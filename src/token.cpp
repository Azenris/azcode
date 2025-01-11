
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
	out << "Token: " << TokenTypes[ static_cast<i32>( token.id ) ].name;

	switch ( token.id )
	{
	case TokenID::Keyword:
	case TokenID::Identifier:
	case TokenID::StringLiteral:
	case TokenID::Number:
		out << " '" << token.value << "'";
		break;

	case TokenID::Minus:
		out << " '-'";
		break;

	case TokenID::MinusAssign:
		out << " '-='";
		break;

	case TokenID::Plus:
		out << " '+'";
		break;

	case TokenID::PlusAssign:
		out << " '++'";
		break;

	case TokenID::Divide:
		out << " '/'";
		break;

	case TokenID::DivideAssign:
		out << " '/='";
		break;

	case TokenID::Asterisk:
		out << " '*'";
		break;

	case TokenID::AsteriskAssign:
		out << " '*='";
		break;

	case TokenID::Assign:
		out << " '='";
		break;

	case TokenID::Equal:
		out << " '=='";
		break;

	case TokenID::GreaterThan:
		out << " '>'";
		break;

	case TokenID::GreaterOrEqual:
		out << " '>='";
		break;

	case TokenID::LesserThan:
		out << " '<'";
		break;

	case TokenID::LesserOrEqual:
		out << " '<='";
		break;

	case TokenID::BitwiseAnd:
		out << " '&'";
		break;

	case TokenID::LogicalAnd:
		out << " '&&'";
		break;

	case TokenID::BitwiseOr:
		out << " '|'";
		break;

	case TokenID::LogicalOr:
		out << " '||'";
		break;

	case TokenID::BitwiseNot:
		out << " '~'";
		break;

	case TokenID::LogicalNot:
		out << " '!'";
		break;

	case TokenID::ParenOpen:
		out << " '('";
		break;

	case TokenID::ParenClose:
		out << " ')'";
		break;

	case TokenID::BraceOpen:
		out << " '{'";
		break;

	case TokenID::BraceClose:
		out << " '}'";
		break;

	case TokenID::Period:
		out << " '.'";
		break;

	case TokenID::Comma:
		out << " ','";
		break;

	case TokenID::Colon:
		out << " ':'";
		break;

	case TokenID::SemiColon:
		out << " ';'";
		break;

	case TokenID::NewLine:
		break;

	case TokenID::EndOfFile:
		break;
	}

	return out;
}

std::ostream & operator << ( std::ostream &out, const TokenID &TokenID )
{
	return out << "Token: " << TokenTypes[ static_cast<i32>( TokenID ) ].name;
}