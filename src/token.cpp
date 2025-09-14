
#include <iostream>
#include <unordered_map>

#include "token.h"

static std::unordered_map<std::string, KeywordID> KeywordsMap =
{
	{ "import", KeywordID::Import },
	{ "return", KeywordID::Return },
	{ "false", KeywordID::False },
	{ "true", KeywordID::True },
	{ "if", KeywordID::If },
	{ "else", KeywordID::Else },
	{ "print", KeywordID::Print },
	{ "println", KeywordID::Println },
	{ "assert", KeywordID::Assert },
	{ "for", KeywordID::For },
	{ "while", KeywordID::While },
	{ "break", KeywordID::Break },
	{ "continue", KeywordID::Continue },
	{ "exit", KeywordID::Exit },
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