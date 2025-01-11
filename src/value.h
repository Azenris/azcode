
#pragma once

#include <string>
#include <iosfwd>

enum class ValueType
{
	Undefined,
	NumberI32,
	NumberI64,
	StringLiteral,
};

struct Value
{
	Value()
		: type( ValueType::Undefined )
	{
	}

	Value( nullptr_t )
		: type( ValueType::Undefined )
	{
	}

	Value( i32 val )
		: type( ValueType::NumberI32 )
		, valueI32( val )
	{
	}

	Value( i64 val )
		: type( ValueType::NumberI64 )
		, valueI64( val )
	{
	}

	Value( const std::string &str )
		: type( ValueType::StringLiteral )
		, valueString( str )
	{
	}

	Value( const char *str )
		: type( ValueType::StringLiteral )
		, valueString( str )
	{
	}

	ValueType type;

	union
	{
		i32 valueI32;
		i64 valueI64;
	};

	std::string valueString;
};

std::ostream & operator << ( std::ostream &out, const Value &value );