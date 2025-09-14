
#pragma once

struct EnumType
{
	TokenID token;
	Value value;
};

const EnumType *get_enum( const char *name );
const EnumType *get_enum( const std::string &name );

enum FILE_MODE
{
	FILE_MODE_APP       = std::ios::app,
	FILE_MODE_BINARY    = std::ios::binary,
	FILE_MODE_IN        = std::ios::in,
	FILE_MODE_OUT       = std::ios::out,
	FILE_MODE_TRUNC     = std::ios::trunc,
	FILE_MODE_ATE       = std::ios::ate,
	FILE_MODE_NOREPLACE = std::ios::noreplace,
};