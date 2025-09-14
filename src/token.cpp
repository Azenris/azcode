
#include <iostream>
#include <unordered_map>

#include "token.h"

static const std::unordered_map<std::string, KeywordID> KeywordsMap = []()
{
	std::unordered_map<std::string, KeywordID> map;
	for ( u64 i = 0, count = ARRAY_LENGTH( Keywords ); i < count; ++i )
		map.insert( { Keywords[ i ].identifier, Keywords[ i ].id } );
	return map;
}();

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