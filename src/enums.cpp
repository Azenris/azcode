
#include <iostream>
#include <unordered_map>

#include "enums.h"
#include "net.h"

static std::unordered_map<std::string, EnumType> EnumsMap =
{
	{ "INVALID_SOCKET",
		{
			.token = TokenID::Number,
			.value = { -1 },
		},
	},
	{ "FILE_MODE_APP",
		{
			.token = TokenID::Number,
			.value = FILE_MODE_APP,
		},
	},
	{ "FILE_MODE_BINARY",
		{
			.token = TokenID::Number,
			.value = FILE_MODE_BINARY,
		},
	},
	{ "FILE_MODE_IN",
		{
			.token = TokenID::Number,
			.value = FILE_MODE_IN,
		},
	},
	{ "FILE_MODE_OUT",
		{
			.token = TokenID::Number,
			.value = FILE_MODE_OUT,
		},
	},
	{ "FILE_MODE_TRUNC",
		{
			.token = TokenID::Number,
			.value = FILE_MODE_TRUNC,
		},
	},
	{ "FILE_MODE_ATE",
		{
			.token = TokenID::Number,
			.value = FILE_MODE_ATE,
		},
	},
	{ "FILE_MODE_NOREPLACE",
		{
			.token = TokenID::Number,
			.value = FILE_MODE_NOREPLACE,
		},
	},
};

const EnumType *get_enum( const char *name )
{
	auto iter = EnumsMap.find( name );
	if ( iter != EnumsMap.end() )
		return &iter->second;
	return nullptr;
}

const EnumType *get_enum( const std::string &name )
{
	auto iter = EnumsMap.find( name );
	if ( iter != EnumsMap.end() )
		return &iter->second;
	return nullptr;
}