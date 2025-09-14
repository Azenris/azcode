
#include <iostream>
#include <algorithm>

#include "value.h"
#include "interpreter.h"

[[noreturn]] static void value_fatal( RESULT_CODE resultCode, const char *message )
{
	std::println( stderr, "[Value] {}", message );
	exit( resultCode );
}

[[noreturn]] static void value_fatal( RESULT_CODE resultCode, const std::string message )
{
	std::println( stderr, "[Value] {}", message );
	exit( resultCode );
}

template <typename... T>
[[noreturn]] static void value_fatal( RESULT_CODE resultCode, std::format_string<T...> fmt, T&&... args )
{
	std::println( stderr, "[Value] {}", std::format( fmt, std::forward<T>( args )...) );
	exit( resultCode );
}

[[noreturn]] static void value_fatal( RESULT_CODE resultCode, struct Interpreter *interpreter, Node *node, const char *message )
{
	std::println( stderr, "[Value] {} {}", message, interpreter->fail_at( node ) );
	exit( resultCode );
}

[[noreturn]] static void value_fatal( RESULT_CODE resultCode, struct Interpreter *interpreter, Node *node, const std::string message )
{
	std::println( stderr, "[Value] {} {}", message, interpreter->fail_at( node ) );
	exit( resultCode );
}

template <typename... T>
[[noreturn]] static void value_fatal( RESULT_CODE resultCode, struct Interpreter *interpreter, Node *node, std::format_string<T...> fmt, T&&... args )
{
	std::println( stderr, "[Value] {} {}", std::format( fmt, std::forward<T>( args )...), interpreter->fail_at( node ) );
	exit( resultCode );
}

static Value BuiltInNode_Array_Push( Interpreter *interpreter, Value &self, Node *args )
{
	Value &l = self.deref();
	for ( auto arg : args->children )
		l.arr.push_back( interpreter->run( arg ) );
	return 0;
}

static Value BuiltInNode_Array_Pop( Interpreter *interpreter, Value &self, Node *args )
{
	interpreter->expect_arg( "pop", args, 1 );
	Value &l = self.deref();
	Value ret = l.arr.back();
	l.arr.pop_back();
	return ret;
}

static Value BuiltInNode_Array_Count( Interpreter *interpreter, Value &self, Node *args )
{
	interpreter->expect_arg( "count", args, 0 );
	return static_cast<i64>( self.deref().arr.size() );
}

static Value BuiltInNode_Array_Sort( Interpreter *interpreter, Value &self, Node *args )
{
	Value &l = self.deref();

	if ( args->children.empty() )
	{
		std::ranges::sort( l.arr, std::ranges::less() );
	}
	else if ( interpreter->run( args->children[ 0 ] ).get_as_bool( interpreter, args ) )
	{
		std::ranges::sort( l.arr, std::ranges::less() );
	}
	else
	{
		std::ranges::sort( l.arr, std::ranges::greater() );
	}

	return 0;
}

static Value BuiltInNode_Struct_Count( Interpreter *interpreter, Value &self, Node *args )
{
	interpreter->expect_arg( "count", args, 0 );
	return static_cast<i64>( self.deref().map.size() );
}

Value::Value( ValueType type )
	: type( type )
	, scope( SCOPE_UNSET )
{
	switch ( type )
	{
	case ValueType::Struct:
		map[ "parent" ] = nullptr;
		map[ "count" ] = BuiltInNode_Struct_Count;
		break;

	case ValueType::Arr:
		map[ "push" ] = BuiltInNode_Array_Push;
		map[ "pop" ] = BuiltInNode_Array_Pop;
		map[ "count" ] = BuiltInNode_Array_Count;
		map[ "sort" ] = BuiltInNode_Array_Sort;
		break;
	}
}

Value & Value::operator = ( const Value &rhs )
{
	Value &l = deref();
	const Value &r = rhs.deref();

	l.type = r.type;

	switch ( l.type )
	{
	case ValueType::TokenID: l.tokenID = r.tokenID; break;
	case ValueType::KeywordID: l.keywordID = r.keywordID; break;
	case ValueType::NumberI32: l.valueI32 = r.valueI32; break;
	case ValueType::NumberI64: l.valueI64 = r.valueI64; break;
	case ValueType::Node: l.valueNode = r.valueNode; break;
	case ValueType::InbuiltFunc: l.valueInbuiltFunc = r.valueInbuiltFunc; break;
	case ValueType::Reference: l.valueRef = r.valueRef; break;
	case ValueType::File: break;
	case ValueType::Command: l.keywordID = r.keywordID; break;
	}

	l.file = r.file;
	l.valueString = r.valueString;
	l.arr = r.arr;
	l.map = r.map;

	for ( auto &entry : l.map )
		entry.second.update_parent( &l );

	return *this;
}

Value Value::operator [] ( i64 index )
{
	if ( type == ValueType::Reference )
		return (*this->valueRef)[ index ];

	if ( type != ValueType::Arr )
		value_fatal( RESULT_CODE_VALUE_SUBSCRIPT_OF_NON_ARRAY, "Attempting to access subscript of value that isn't an array. ( {} ).", *this );

	if ( index < 0 || index >= static_cast<i64>( arr.size() ) )
		value_fatal( RESULT_CODE_VALUE_SUBSCRIPT_OUT_OF_RANGE, "Attempting to access subscript of value out of bounds[ {} ]. ( {} ).", index, *this );

	return &arr[ index ];
}

void Value::update_parent( Value *parent )
{
	if ( type != ValueType::Struct )
		return;

	Value &value = map[ "parent" ];
	value.type = ValueType::Reference;
	value.scope = SCOPE_STRUCT;
	value.valueRef = parent;

	for ( auto &entry : map )
		entry.second.update_parent( this );
}

bool Value::get_as_bool( Interpreter *interpreter, Node *node )
{
	switch ( type )
	{
	case ValueType::Undefined: return false;
	case ValueType::NumberI32: return valueI32 != 0;
	case ValueType::NumberI64: return valueI64 != 0;
	case ValueType::StringLiteral: return !valueString.empty();
	case ValueType::Struct: return !map.empty();
	case ValueType::Arr: return !arr.empty();
	case ValueType::TokenID: return false;
	case ValueType::KeywordID: return false;
	case ValueType::Node: return valueNode;
	case ValueType::InbuiltFunc: return false;
	case ValueType::Reference: return valueRef->get_as_bool( interpreter, node );
	case ValueType::File: return file && file->is_open();
	case ValueType::Command: return false;
	}

	value_fatal( RESULT_CODE_VALUE_CANNOT_CONVERT, interpreter, node, "Cannot convert from {} to bool.", *this );
}

i32 Value::get_as_i32( Interpreter *interpreter, Node *node )
{
	switch ( type )
	{
	case ValueType::Undefined: return 0;
	case ValueType::NumberI32: return valueI32;
	case ValueType::NumberI64: return static_cast<i32>( valueI64 );

	case ValueType::StringLiteral:
		{
			if ( valueString.empty() )
				return 0;
			i32 idx;
			if ( to_int( &idx, valueString.c_str() ) == ToIntResult::Success )
			{
				return idx;
			}
		}
		break;

	case ValueType::Reference:
		return valueRef->get_as_i32( interpreter, node );

	case ValueType::File:
		return file && file->is_open();
	}

	value_fatal( RESULT_CODE_VALUE_CANNOT_CONVERT, interpreter, node, "Cannot convert from {} to i32.", *this );
}

i64 Value::get_as_i64( Interpreter *interpreter, Node *node )
{
	switch ( type )
	{
	case ValueType::Undefined: return 0;
	case ValueType::NumberI32: return valueI32;
	case ValueType::NumberI64: return valueI64;

	case ValueType::StringLiteral:
		{
			if ( valueString.empty() )
				return 0;
			i64 idx;
			if ( to_int( &idx, valueString.c_str() ) == ToIntResult::Success )
			{
				return idx;
			}
		}
		break;

	case ValueType::Reference:
		return valueRef->get_as_i64( interpreter, node );

	case ValueType::File:
		return file && file->is_open();
	}

	value_fatal( RESULT_CODE_VALUE_CANNOT_CONVERT, interpreter, node, "Cannot convert from {} to i64.", *this );
}

std::string Value::get_as_string( Interpreter *interpreter, Node *node )
{
	switch ( type )
	{
	case ValueType::Undefined: return "Undefined";
	case ValueType::NumberI32: return std::to_string( valueI32 );
	case ValueType::NumberI64: return std::to_string( valueI64 );
	case ValueType::StringLiteral: return valueString;
	case ValueType::Reference: return valueRef->get_as_string( interpreter, node );
	case ValueType::File: return valueString;
	}

	value_fatal( RESULT_CODE_VALUE_CANNOT_CONVERT, interpreter, node, "Cannot convert from {} to string.", *this );
}

i64 Value::count() const
{
	switch ( type )
	{
	case ValueType::Undefined: return 0;
	case ValueType::NumberI32: return 0;
	case ValueType::NumberI64: return 0;
	case ValueType::StringLiteral: return valueString.size();
	case ValueType::Struct: return map.size();
	case ValueType::Arr: return arr.size();
	case ValueType::TokenID: return 0;
	case ValueType::KeywordID: return 0;
	case ValueType::Node: return 0;
	case ValueType::InbuiltFunc: return 0;
	case ValueType::Reference: return valueRef->count();
	case ValueType::File: return 0;
	case ValueType::Command: return 0;
	}
	return 0;
}

void Value::clear()
{
	type = ValueType::Undefined;
	file.reset();
	valueString.clear();
	arr.clear();
	map.clear();
}

Value &Value::deref()
{
	if ( type == ValueType::Reference )
		return *valueRef;
	return *this;
}

const Value &Value::deref() const 
{
	if ( type == ValueType::Reference )
		return *valueRef;
	return *this;
}

bool operator == ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 == r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 == r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 == r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 == r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return false;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return false;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return false;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return false;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return l.valueString == r.valueString;
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_COMPARITOR, "Unhandled value '==' types( {}, {} )", l.type, r.type );
}

bool operator != ( const Value &lhs, const Value &rhs )
{
	return !( lhs == rhs );
}

bool operator < ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 < r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 < r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 < r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 < r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 < static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return static_cast<i32>( std::stoll( l.valueString ) ) < l.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 < std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::stoll( l.valueString ) < r.valueI64;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::stoll( l.valueString ) < std::stoll( r.valueString );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '<' types( {}, {} )", l.type, r.type );
}

bool operator > ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 > r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 > r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 > r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 > r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 > static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return static_cast<i32>( std::stoll( l.valueString ) ) > l.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 > std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::stoll( l.valueString ) > r.valueI64;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::stoll( l.valueString ) > std::stoll( r.valueString );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '>' types( {}, {} )", l.type, r.type );
}

bool operator <= ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 <= r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 <= r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 <= r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 <= r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 <= static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return static_cast<i32>( std::stoll( l.valueString ) ) <= l.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 <= std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::stoll( l.valueString ) <= r.valueI64;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::stoll( l.valueString ) <= std::stoll( r.valueString );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '<=' types( {}, {} )", l.type, r.type );
}

bool operator >= ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 >= r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 >= r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 >= r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 >= r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 >= static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return static_cast<i32>( std::stoll( l.valueString ) ) >= l.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 >= std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::stoll( l.valueString ) >= r.valueI64;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::stoll( l.valueString ) >= std::stoll( r.valueString );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '>=' types( {}, {} )", l.type, r.type );
}

Value operator - ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 - r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 - r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 - r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 - r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 - static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) - l.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 - std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( l.valueString ) - r.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( l.valueString ) - std::stoll( r.valueString ) );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '-' types( {}, {} )", l.type, r.type );
}

Value & operator -= ( Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	Value value;

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			lhs = l.valueI32 - r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			lhs = l.valueI32 - r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			lhs = l.valueI64 - r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			lhs = l.valueI64 - r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		lhs = l.valueI32 - static_cast<i32>( std::stoll( r.valueString ) ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		lhs = std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) - l.valueI32 ); break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		lhs = l.valueI64 - std::stoll( r.valueString ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		lhs = std::to_string( std::stoll( l.valueString ) - r.valueI64 ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	lhs = std::to_string( std::stoll( l.valueString ) - std::stoll( r.valueString ) ); break;
	default:
		value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '-=' types( {}, {} )", l.type, r.type );
	}

	return lhs;
}

Value operator + ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 + r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 + r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 + r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 + r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 + static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) + l.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 + std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( l.valueString ) + r.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( l.valueString ) + std::stoll( r.valueString ) );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '+' types( {}, {} )", l.type, r.type );
}

Value & operator += ( Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	Value value;

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			lhs = l.valueI32 + r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			lhs = l.valueI32 + r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			lhs = l.valueI64 + r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			lhs = l.valueI64 + r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		lhs = l.valueI32 + static_cast<i32>( std::stoll( r.valueString ) ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		lhs = std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) + l.valueI32 ); break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		lhs = l.valueI64 + std::stoll( r.valueString ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		lhs = std::to_string( std::stoll( l.valueString ) + r.valueI64 ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	lhs = std::to_string( std::stoll( l.valueString ) + std::stoll( r.valueString ) ); break;
	default:
		value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '+=' types( {}, {} )", l.type, r.type );
	}

	return lhs;
}

Value operator / ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 / r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 / r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 / r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 / r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 / static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) / l.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 / std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( l.valueString ) / r.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( l.valueString ) / std::stoll( r.valueString ) );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '/' types( {}, {} )", l.type, r.type );
}

Value & operator /= ( Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	Value value;

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			lhs = l.valueI32 / r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			lhs = l.valueI32 / r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			lhs = l.valueI64 / r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			lhs = l.valueI64 / r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		lhs = l.valueI32 / static_cast<i32>( std::stoll( r.valueString ) ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		lhs = std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) / l.valueI32 ); break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		lhs = l.valueI64 / std::stoll( r.valueString ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		lhs = std::to_string( std::stoll( l.valueString ) / r.valueI64 ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	lhs = std::to_string( std::stoll( l.valueString ) / std::stoll( r.valueString ) ); break;
	default:
		value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '/=' types( {}, {} )", l.type, r.type );
	}

	return lhs;
}

Value operator * ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 * r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 * r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 * r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 * r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 * static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) * l.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 * std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( l.valueString ) * r.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( l.valueString ) * std::stoll( r.valueString ) );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '*' types( {}, {} )", l.type, r.type );
}

Value & operator *= ( Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	Value value;

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			lhs = l.valueI32 * r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			lhs = l.valueI32 * r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			lhs = l.valueI64 * r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			lhs = l.valueI64 * r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		lhs = l.valueI32 * static_cast<i32>( std::stoll( r.valueString ) ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		lhs = std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) * l.valueI32 ); break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		lhs = l.valueI64 * std::stoll( r.valueString ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		lhs = std::to_string( std::stoll( l.valueString ) * r.valueI64 ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	lhs = std::to_string( std::stoll( l.valueString ) * std::stoll( r.valueString ) ); break;
	default:
		value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '(=' types( {}, {} )", l.type, r.type );
	}

	return lhs;
}

Value operator & ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 & r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 & r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 & r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 & r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 & static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) & l.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 & std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( l.valueString ) & r.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( l.valueString ) & std::stoll( r.valueString ) );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '&' types( {}, {} )", l.type, r.type );
}

Value & operator &= ( Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	Value value;

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			lhs = l.valueI32 & r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			lhs = l.valueI32 & r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			lhs = l.valueI64 & r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			lhs = l.valueI64 & r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		lhs = l.valueI32 & static_cast<i32>( std::stoll( r.valueString ) ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		lhs = std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) & l.valueI32 ); break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		lhs = l.valueI64 & std::stoll( r.valueString ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		lhs = std::to_string( std::stoll( l.valueString ) & r.valueI64 ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	lhs = std::to_string( std::stoll( l.valueString ) & std::stoll( r.valueString ) ); break;
	default:
		value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '&=' types( {}, {} )", l.type, r.type );
	}

	return lhs;
}

Value operator | ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 | r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 | r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 | r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 | r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 | static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) | l.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 | std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( l.valueString ) | r.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( l.valueString ) | std::stoll( r.valueString ) );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '|' types( {}, {} )", l.type, r.type );
}

Value & operator |= ( Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	Value value;

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			lhs = l.valueI32 | r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			lhs = l.valueI32 | r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			lhs = l.valueI64 | r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			lhs = l.valueI64 | r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		lhs = l.valueI32 | static_cast<i32>( std::stoll( r.valueString ) ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		lhs = std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) | l.valueI32 ); break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		lhs = l.valueI64 | std::stoll( r.valueString ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		lhs = std::to_string( std::stoll( l.valueString ) | r.valueI64 ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	lhs = std::to_string( std::stoll( l.valueString ) | std::stoll( r.valueString ) ); break;
	default:
		value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '|=' types( {}, {} )", l.type, r.type );
	}

	return lhs;
}

Value operator ^ ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 ^ r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 ^ r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 ^ r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 ^ r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 ^ static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) ^ l.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 ^ std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( l.valueString ) ^ r.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( l.valueString ) ^ std::stoll( r.valueString ) );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '^' types( {}, {} )", l.type, r.type );
}

Value & operator ^= ( Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	Value value;

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			lhs = l.valueI32 ^ r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			lhs = l.valueI32 ^ r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			lhs = l.valueI64 ^ r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			lhs = l.valueI64 ^ r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		lhs = l.valueI32 ^ static_cast<i32>( std::stoll( r.valueString ) ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		lhs = std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) ^ l.valueI32 ); break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		lhs = l.valueI64 ^ std::stoll( r.valueString ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		lhs = std::to_string( std::stoll( l.valueString ) ^ r.valueI64 ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	lhs = std::to_string( std::stoll( l.valueString ) ^ std::stoll( r.valueString ) ); break;
	default:
		value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '^=' types( {}, {} )", l.type, r.type );
	}

	return lhs;
}

Value operator % ( const Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return l.valueI32 % r.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return l.valueI32 % r.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			return l.valueI64 % r.valueI32;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return l.valueI64 % r.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return l.valueI32 % static_cast<i32>( std::stoll( r.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) % l.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return l.valueI64 % std::stoll( r.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( l.valueString ) % r.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( l.valueString ) % std::stoll( r.valueString ) );
	}

	value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '%' types( {}, {} )", l.type, r.type );
}

Value & operator %= ( Value &lhs, const Value &rhs )
{
	const Value &l = lhs.deref();
	const Value &r = rhs.deref();

	Value value;

	switch ( TYPE_PAIR( static_cast<i32>( l.type ), static_cast<i32>( r.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			lhs = l.valueI32 % r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			lhs = l.valueI32 % r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI32 ):			lhs = l.valueI64 % r.valueI32; break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			lhs = l.valueI64 % r.valueI64; break;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		lhs = l.valueI32 % static_cast<i32>( std::stoll( r.valueString ) ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		lhs = std::to_string( static_cast<i32>( std::stoll( l.valueString ) ) % l.valueI32 ); break;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		lhs = l.valueI64 % std::stoll( r.valueString ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		lhs = std::to_string( std::stoll( l.valueString ) % r.valueI64 ); break;
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	lhs = std::to_string( std::stoll( l.valueString ) % std::stoll( r.valueString ) ); break;
	default:
		value_fatal( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC, "Unhandled value '%=' types( {}, {} )", l.type, r.type );
	}

	return lhs;
}

ToIntResult to_int( i32 *value, char const *str, char **endOut, i32 base )
{
	i64 v;
	ToIntResult result = to_int( &v, str, endOut, base );
	if ( result != ToIntResult::Success )
		return result;
	if ( v > INT32_MAX )
		return ToIntResult::Overflow;
	if ( v < INT32_MIN )
		return ToIntResult::Underflow;
	*value = static_cast<i32>( v );
	return ToIntResult::Success;
}

ToIntResult to_int( u32 *value, char const *str, char **endOut, i32 base )
{
	u64 v;
	ToIntResult result = to_int( &v, str, endOut, base );
	if ( result != ToIntResult::Success )
		return result;
	if ( v > UINT32_MAX )
		return ToIntResult::Overflow;
	if ( v < 0 )
		return ToIntResult::Underflow;
	*value = static_cast<u32>( v );
	return ToIntResult::Success;
}

ToIntResult to_int( i64 *value, char const *str, char **endOut, i32 base )
{
	char *end;
	errno = 0;
	i64 l = strtol( str, &end, base );
	if ( endOut )
		*endOut = end;
	if ( ( errno == ERANGE && l == LONG_MAX ) || l > INT_MAX )
		return ToIntResult::Overflow;
	if ( ( errno == ERANGE && l == LONG_MIN ) || l < INT_MIN )
		return ToIntResult::Underflow;
	if ( *str == '\0' )
		return ToIntResult::Failed;
	*value = l;
	return ToIntResult::Success;
}

ToIntResult to_int( u64 *value, char const *str, char **endOut, i32 base )
{
	char *end;
	errno = 0;
	u64 l = strtoul( str, &end, base );
	if ( endOut )
		*endOut = end;
	if ( ( errno == ERANGE && l == LONG_MAX ) || l > INT_MAX )
		return ToIntResult::Overflow;
	if ( ( errno == ERANGE && l == LONG_MIN ) || l < INT_MIN )
		return ToIntResult::Underflow;
	if ( *str == '\0' )
		return ToIntResult::Failed;
	*value = l;
	return ToIntResult::Success;
}

void Value::unfold()
{
	if ( type != ValueType::Reference )
		return;

	// Deref before changing the type
	const Value &r = deref();

	// Make sure its not considered reference, so the overloaded assignment operator
	// will assign directly to this Value
	type = ValueType::Undefined;

	// Copy the data to this value
	*this = r;
}