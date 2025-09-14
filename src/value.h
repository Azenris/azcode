
#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <iosfwd>
#include <print>

#include "ids.h"
#include "result_code.h"
#include "token_types.h"

struct Node;
struct Interpreter;

constexpr i32 TypeShift = 16;

#define TYPE_PAIR( t0, t1 ) ( ( static_cast<i32>( t0 ) << TypeShift ) | static_cast<i32>( t1 ) )

enum SCOPE
{
	SCOPE_UNSET = -3,
	SCOPE_STRUCT = -2,
	SCOPE_LOCAL = -1,
	SCOPE_GLOBAL = 0,
};

struct Value
{
	using InBuiltFunc = Value (*)( Interpreter *interpreter, Value &self, Node *args );

	ValueType type;
	i32 scope;

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

	std::shared_ptr<std::fstream> file;
	std::string valueString;
	std::vector<Value> arr;
	std::unordered_map<std::string, Value> map;

	// -- --
	Value()
		: type( ValueType::Undefined )
		, scope( SCOPE_UNSET )
	{
	}

	Value( ValueType type );

	Value( nullptr_t )
		: type( ValueType::Undefined )
		, scope( SCOPE_UNSET )
	{
	}

	Value( i32 val )
		: type( ValueType::NumberI32 )
		, scope( SCOPE_UNSET )
		, valueI32( val )
	{
	}

	Value( i64 val )
		: type( ValueType::NumberI64 )
		, scope( SCOPE_UNSET )
		, valueI64( val )
	{
	}

	Value( const std::string &str )
		: type( ValueType::StringLiteral )
		, scope( SCOPE_UNSET )
		, valueString( str )
	{
	}

	Value( const char *str )
		: type( ValueType::StringLiteral )
		, scope( SCOPE_UNSET )
		, valueString( str )
	{
	}

	Value( TokenID tokenID, const char *str )
		: type( ValueType::TokenID )
		, scope( SCOPE_UNSET )
		, tokenID( tokenID )
		, valueString( str )
	{
	}

	Value( KeywordID keywordID, const char *str )
		: type( ValueType::KeywordID )
		, scope( SCOPE_UNSET )
		, keywordID( keywordID )
		, valueString( str )
	{
	}

	Value( KeywordID keywordID )
		: type( ValueType::Command )
		, scope( SCOPE_UNSET )
		, keywordID( keywordID )
	{
	}

	Value( Node *node )
		: type( ValueType::Node )
		, scope( SCOPE_UNSET )
		, valueNode( node )
	{
	}

	Value( InBuiltFunc inbuiltFunc )
		: type( ValueType::InbuiltFunc )
		, scope( SCOPE_UNSET )
		, valueInbuiltFunc( inbuiltFunc )
	{
	}

	Value( Value *value )
		: type( ValueType::Reference )
		, scope( SCOPE_UNSET )
		, valueRef( value )
	{
	}

	Value( std::string &name, std::fstream &fileStream )
		: type( ValueType::File )
		, scope( SCOPE_UNSET )
		, file( std::make_shared<std::fstream>() )
		, valueString( name )
	{
		*file = std::move( fileStream );
	}

	Value & operator = ( const Value &rhs );
	Value operator [] ( i64 index );

	void update_parent( Value *parent );
	bool get_as_bool( Interpreter *interpreter, Node *node );
	i32 get_as_i32( Interpreter *interpreter, Node *node );
	i64 get_as_i64( Interpreter *interpreter, Node *node );
	std::string get_as_string( Interpreter *interpreter, Node *node );
	i64 count() const;
	void clear();
	Value &deref();
	const Value &deref() const;
	void unfold();
};

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

// --------------------------------------------------------------------

template <>
struct std::formatter<ValueType>
{
	constexpr auto parse( std::format_parse_context &ctx )
	{
		return ctx.begin();
	}

	std::format_context::iterator format( const ValueType &valueType, std::format_context &ctx ) const
	{
		const char *name = "Unknown ValueType";

		switch ( valueType )
		{
		case ValueType::Undefined: name = "Undefined"; break;
		case ValueType::NumberI32: name = "NumberI32"; break;
		case ValueType::NumberI64: name = "NumberI64"; break;
		case ValueType::StringLiteral: name = "StringLiteral"; break;
		case ValueType::Struct: name = "Struct"; break;
		case ValueType::Arr: name = "Array"; break;
		case ValueType::TokenID: name = "TokenID"; break;
		case ValueType::KeywordID: name = "KeywordID"; break;
		case ValueType::Node: name = "Node"; break;
		case ValueType::InbuiltFunc: name = "InbuiltFunc"; break;
		case ValueType::Reference: name = "Reference"; break;
		case ValueType::File: name = "File"; break;
		case ValueType::Command: name = "Command"; break;
		}

		return std::format_to( ctx.out(), "{}", name );
	}
};

template <>
struct std::formatter<Value> : std::formatter<string_view>
{
	constexpr auto parse( std::format_parse_context &ctx )
	{
		return ctx.begin();
	}

	std::format_context::iterator format( const Value &value, std::format_context &ctx ) const
	{
		switch ( value.type )
		{
		case ValueType::Undefined:			return std::format_to( ctx.out(), "Undefined" );
		case ValueType::NumberI32:			return std::format_to( ctx.out(), "{}", value.valueI32 );
		case ValueType::NumberI64:			return std::format_to( ctx.out(), "{}", value.valueI64 );
		case ValueType::StringLiteral:		return std::format_to( ctx.out(), "{}", value.valueString );

		case ValueType::Struct:
			{
				std::string temp;
				temp.reserve( value.arr.size() * 64 );
				std::format_to( std::back_inserter( temp ), "{{ " );
				if ( !value.map.empty() )
				{
					auto entry = []( std::string &temp, auto iter, bool &f )
					{
						if ( iter->first != "parent" )
						{
							if ( f )
								std::format_to( std::back_inserter( temp ), "{}:{}", iter->first, iter->second );
							else
								std::format_to( std::back_inserter( temp ), ", {}:{}", iter->first, iter->second );
							f = false;
						}
					};

					bool first = true;
					auto iter = value.map.begin();
					entry( temp, iter, first );
					for ( ++iter; iter != value.map.end(); ++iter )
						entry( temp, iter, first );
				}
				std::format_to( std::back_inserter( temp ), " }}" );
				return std::formatter<string_view>::format( temp, ctx );
			}
			break;

		case ValueType::Arr:
			{
				std::string temp;
				temp.reserve( value.arr.size() * 32 );
				std::format_to( std::back_inserter( temp ), "[ " );

				if ( !value.arr.empty() )
				{
					std::format_to( std::back_inserter( temp ), "{}", value.arr[ 0 ] );
					for ( u64 i = 1, count = value.arr.size(); i < count; ++i )
						std::format_to( std::back_inserter( temp ), ", {}", value.arr[ i ] );
				}

				std::format_to( std::back_inserter( temp ), "]({})", value.arr.size() );

				return std::formatter<string_view>::format( temp, ctx );
			}
			break;

		case ValueType::TokenID:			return std::format_to( ctx.out(), "{}", value.valueString );
		case ValueType::KeywordID:			return std::format_to( ctx.out(), "{}", value.valueString );
		case ValueType::Node:				return std::format_to( ctx.out(), "Function" );
		case ValueType::InbuiltFunc:		return std::format_to( ctx.out(), "InbuiltFunc" );
		case ValueType::Reference:			return std::format_to( ctx.out(), "{}", *value.valueRef );
		case ValueType::File:				return std::format_to( ctx.out(), "{}", value.valueString );
		case ValueType::Command:			return std::format_to( ctx.out(), "{}", Keywords[ static_cast<i32>( value.keywordID ) ].name );
		}

		return std::format_to( ctx.out(), "Unhandled value ValueType( {} )", value.type );
	}
};