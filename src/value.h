
#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <iosfwd>

#include "ids.h"

struct Node;
struct Interpreter;

constexpr i32 TypeShift = 16;

#define TYPE_PAIR( t0, t1 ) ( ( static_cast<i32>( t0 ) << TypeShift ) | static_cast<i32>( t1 ) )

struct Value
{
	using InBuiltFunc = Value (*)( Interpreter *interpreter, Value &self, Node *args );

	ValueType type;
	int scope;

	union
	{
		TokenID tokenID;
		KeywordID keywordID;
		i32 valueI32;
		i64 valueI64;
		Node *valueNode;
		Value *valueRef;
		InBuiltFunc valueInbuiltFunc;
	};

	std::string valueString;
	std::vector<Value> arr;
	std::unordered_map<std::string, Value> map;

	// -- --
	Value()
		: type( ValueType::Undefined )
		, scope( -2 )
	{
	}

	Value( ValueType type )
		: type( type )
		, scope( -2 )
	{
	}

	Value( nullptr_t )
		: type( ValueType::Undefined )
		, scope( -2 )
	{
	}

	Value( i32 val )
		: type( ValueType::NumberI32 )
		, scope( -2 )
		, valueI32( val )
	{
	}

	Value( i64 val )
		: type( ValueType::NumberI64 )
		, scope( -2 )
		, valueI64( val )
	{
	}

	Value( const std::string &str )
		: type( ValueType::StringLiteral )
		, scope( -2 )
		, valueString( str )
	{
	}

	Value( const char *str )
		: type( ValueType::StringLiteral )
		, scope( -2 )
		, valueString( str )
	{
	}

	Value( TokenID tokenID, const char *str )
		: type( ValueType::TokenID )
		, scope( -2 )
		, tokenID( tokenID )
		, valueString( str )
	{
	}

	Value( KeywordID keywordID, const char *str )
		: type( ValueType::KeywordID )
		, scope( -2 )
		, keywordID( keywordID )
		, valueString( str )
	{
	}

	Value( KeywordID keywordID )
		: type( ValueType::Command )
		, scope( -2 )
		, keywordID( keywordID )
	{
	}

	Value( Node *node )
		: type( ValueType::Node )
		, scope( -2 )
		, valueNode( node )
	{
	}

	Value( InBuiltFunc inbuiltFunc )
		: type( inbuiltFunc ? ValueType::InbuiltFunc : ValueType::Undefined )
		, scope( -2 )
		, valueInbuiltFunc( inbuiltFunc )
	{
	}

	Value( Value *value )
		: type( ValueType::Reference )
		, scope( -2 )
		, valueRef( value )
	{
	}

	Value & operator = ( const Value &rhs );
	Value operator [] ( i64 index );

	void update_parent( Value *parent );
	bool get_as_bool( Node *node );
	i64 get_as_i64( Node *node );
	std::string get_as_string( Node *node );
	i64 count() const;
	void clear();
	Value &deref();
	const Value &deref() const;
	void unfold();
};

std::ostream & operator << ( std::ostream &out, const Value &value );

bool operator == ( const Value &lhs, const Value &rhs );
bool operator != ( const Value &lhs, const Value &rhs );

bool operator < ( const Value &lhs, const Value &rhs );
bool operator > ( const Value &lhs, const Value &rhs );
bool operator <= ( const Value &lhs, const Value &rhs );
bool operator >= ( const Value &lhs, const Value &rhs );

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