
#pragma once

#include <vector>
#include <string>
#include <iosfwd>

#include "ids.h"

struct Node;

constexpr i32 TypeShift = 16;

#define TYPE_PAIR( t0, t1 ) ( ( static_cast<i32>( t0 ) << TypeShift ) | static_cast<i32>( t1 ) )

struct Value
{
	ValueType type;

	union
	{
		TokenID tokenID;
		KeywordID keywordID;
		i32 valueI32;
		i64 valueI64;
		Node *valueNode;
	};

	std::string valueString;
	std::vector<Value> arr;

	// -- --
	Value()
		: type( ValueType::Undefined )
	{
	}

	Value( ValueType type )
		: type( type )
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

	Value( TokenID tokenID, const char *str )
		: type( ValueType::TokenID )
		, tokenID( tokenID )
		, valueString( str )
	{
	}

	Value( KeywordID keywordID, const char *str )
		: type( ValueType::KeywordID )
		, keywordID( keywordID )
		, valueString( str )
	{
	}

	Value( Node *node )
		: type( ValueType::Node )
		, valueNode( node )
	{
	}

	operator bool();
	explicit operator i64();
	Value &operator [] ( i64 index );
	i64 count() const;
};

std::ostream & operator << ( std::ostream &out, const Value &value );

Value operator - ( const Value &lhs, const Value &rhs );
Value & operator -= ( Value &lhs, const Value &rhs );
Value operator + ( const Value &lhs, const Value &rhs );
Value & operator += ( Value &lhs, const Value &rhs );
Value operator / ( const Value &lhs, const Value &rhs );
Value & operator /= ( Value &lhs, const Value &rhs );
Value operator * ( const Value &lhs, const Value &rhs );
Value & operator *= ( Value &lhs, const Value &rhs );
Value operator & ( const Value &lhs, const Value &rhs );
Value & operator &= ( Value &lhs, const Value &rhs );
Value operator | ( const Value &lhs, const Value &rhs );
Value & operator |= ( Value &lhs, const Value &rhs );
Value operator ^ ( const Value &lhs, const Value &rhs );
Value & operator ^= ( Value &lhs, const Value &rhs );
Value operator % ( const Value &lhs, const Value &rhs );
Value & operator %= ( Value &lhs, const Value &rhs );

enum ToIntResult
{
	Success,
	Failed,
	Overflow,
	Underflow,
};

ToIntResult to_int( i32 *value, char const *str, char **endOut = nullptr, i32 base = 0 );
ToIntResult to_int( u32 *value, char const *str, char **endOut = nullptr, i32 base = 0 );
ToIntResult to_int( i64 *value, char const *str, char **endOut = nullptr, i32 base = 0 );
ToIntResult to_int( u64 *value, char const *str, char **endOut = nullptr, i32 base = 0 );