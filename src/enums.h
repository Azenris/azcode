
#pragma once

struct EnumType
{
	TokenID token;
	Value value;
};

const EnumType *get_enum( const char *name );
const EnumType *get_enum( const std::string &name );