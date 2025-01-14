
#include <iostream>

#include "value.h"

Value::operator bool()
{
	switch ( type )
	{
	case ValueType::Undefined: return false;
	case ValueType::NumberI32: return valueI32 != 0;
	case ValueType::NumberI64: return valueI64 != 0;
	case ValueType::StringLiteral: return !valueString.empty();
	case ValueType::Arr: return !arr.empty();
	case ValueType::KeywordID: return true;
	case ValueType::Node: return valueNode;
	}
	return false;
}

Value::operator i64()
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
	}

	std::cerr << "Cannot convert from " << *this << " to i64." << std::endl;
	exit( RESULT_CODE_VALUE_CANNOT_CONVERT );
}

Value &Value::operator [] ( i64 index )
{
	if ( type != ValueType::Arr )
	{
		std::cerr << "Attempting to access subscript of value that isn't an array. ( " << *this << " )." << std::endl;
		exit( RESULT_CODE_VALUE_SUBSCRIPT_OF_NON_ARRAY );
	}

	if ( index < 0 || index >= static_cast<i64>( arr.size() ) )
	{
		std::cerr << "Attempting to access subscript of value out of bounds[ " << index << " ]. ( " << *this << " )." << std::endl;
		exit( RESULT_CODE_VALUE_SUBSCRIPT_OUT_OF_RANGE );
	}

	return arr[ index ];
}

std::ostream & operator << ( std::ostream &out, const Value &value )
{
	switch ( value.type )
	{
	case ValueType::Undefined:			return out << "Undefined";
	case ValueType::NumberI32:			return out << value.valueI32;
	case ValueType::NumberI64:			return out << value.valueI64;
	case ValueType::StringLiteral:		return out << value.valueString;

	case ValueType::Arr:
		out << "[ ";
		if ( !value.arr.empty() )
		{
			out << value.arr[ 0 ];
			for ( u64 i = 1, count = value.arr.size(); i < count; ++i )
				out << ", " << value.arr[ i ];
		}
		return out << " ]{" << value.arr.size() << "}";

	case ValueType::KeywordID:			return out << value.valueString << " (id:" << value.valueI32 << ")";
	case ValueType::Node:				return out << "Node[" << value.valueNode << "]";
	}

	return out << "Unhandled value ValueType( " << static_cast<i32>( value.type ) << " )";
}

std::ostream & operator << ( std::ostream &out, const ValueType &valueType )
{
	switch ( valueType )
	{
	case ValueType::Undefined:			return out << "Undefined";
	case ValueType::NumberI32:			return out << "NumberI32";
	case ValueType::NumberI64:			return out << "NumberI64";
	case ValueType::StringLiteral:		return out << "StringLiteral";
	case ValueType::Arr:				return out << "Array";
	case ValueType::KeywordID:			return out << "KeywordID";
	case ValueType::Node:				return out << "Node";
	}

	return out << "Unhandled value ValueType( '" << static_cast<i32>( valueType ) << "' )";
}

Value operator - ( const Value &lhs, const Value &rhs )
{
	if ( lhs.type == ValueType::Undefined || rhs.type == ValueType::Undefined )
	{
		std::cerr << "Using '-' operator with( " << lhs.type << " - " << rhs.type << " )" << std::endl;
		return nullptr;
	}

	switch ( TYPE_PAIR( static_cast<i32>( lhs.type ), static_cast<i32>( rhs.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return lhs.valueI32 - rhs.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return lhs.valueI32 - rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return lhs.valueI64 - rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return lhs.valueI32 - static_cast<i32>( std::stoll( rhs.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( lhs.valueString ) ) - lhs.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return lhs.valueI64 - std::stoll( rhs.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( lhs.valueString ) - rhs.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( lhs.valueString ) - std::stoll( rhs.valueString ) );
	}

	std::cerr << "Unhandled value types( " << lhs.type << ", " << rhs.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
}

Value & operator -= ( Value &lhs, const Value &rhs )
{
	lhs = lhs - rhs;
	return lhs;
}

Value operator + ( const Value &lhs, const Value &rhs )
{
	if ( lhs.type == ValueType::Undefined || rhs.type == ValueType::Undefined )
	{
		std::cerr << "Using '+' operator with( " << lhs.type << " + " << rhs.type << " )" << std::endl;
		return nullptr;
	}

	switch ( TYPE_PAIR( static_cast<i32>( lhs.type ), static_cast<i32>( rhs.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return lhs.valueI32 + rhs.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return lhs.valueI32 + rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return lhs.valueI64 + rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return lhs.valueI32 + static_cast<i32>( std::stoll( rhs.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( lhs.valueString ) ) + lhs.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return lhs.valueI64 + std::stoll( rhs.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( lhs.valueString ) + rhs.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( lhs.valueString ) + std::stoll( rhs.valueString ) );
	}

	std::cerr << "Unhandled value types( " << lhs.type << ", " << rhs.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
}

Value & operator += ( Value &lhs, const Value &rhs )
{
	lhs = lhs + rhs;
	return lhs;
}

Value operator / ( const Value &lhs, const Value &rhs )
{
	if ( lhs.type == ValueType::Undefined || rhs.type == ValueType::Undefined )
	{
		std::cerr << "Using '/' operator with( " << lhs.type << " / " << rhs.type << " )" << std::endl;
		return nullptr;
	}

	switch ( TYPE_PAIR( static_cast<i32>( lhs.type ), static_cast<i32>( rhs.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return lhs.valueI32 / rhs.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return lhs.valueI32 / rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return lhs.valueI64 / rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return lhs.valueI32 / static_cast<i32>( std::stoll( rhs.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( lhs.valueString ) ) / lhs.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return lhs.valueI64 / std::stoll( rhs.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( lhs.valueString ) / rhs.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( lhs.valueString ) / std::stoll( rhs.valueString ) );
	}

	std::cerr << "Unhandled value types( " << lhs.type << ", " << rhs.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
}

Value & operator /= ( Value &lhs, const Value &rhs )
{
	lhs = lhs / rhs;
	return lhs;
}

Value operator * ( const Value &lhs, const Value &rhs )
{
	if ( lhs.type == ValueType::Undefined || rhs.type == ValueType::Undefined )
	{
		std::cerr << "Using '*' operator with( " << lhs.type << " * " << rhs.type << " )" << std::endl;
		return nullptr;
	}

	switch ( TYPE_PAIR( static_cast<i32>( lhs.type ), static_cast<i32>( rhs.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return lhs.valueI32 * rhs.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return lhs.valueI32 * rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return lhs.valueI64 * rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return lhs.valueI32 * static_cast<i32>( std::stoll( rhs.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( lhs.valueString ) ) * lhs.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return lhs.valueI64 * std::stoll( rhs.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( lhs.valueString ) * rhs.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( lhs.valueString ) * std::stoll( rhs.valueString ) );
	}

	std::cerr << "Unhandled value types( " << lhs.type << ", " << rhs.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
}

Value & operator *= ( Value &lhs, const Value &rhs )
{
	lhs = lhs * rhs;
	return lhs;
}

Value operator & ( const Value &lhs, const Value &rhs )
{
	if ( lhs.type == ValueType::Undefined || rhs.type == ValueType::Undefined )
	{
		std::cerr << "Using '&' operator with( " << lhs.type << " & " << rhs.type << " )" << std::endl;
		return nullptr;
	}

	switch ( TYPE_PAIR( static_cast<i32>( lhs.type ), static_cast<i32>( rhs.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return lhs.valueI32 & rhs.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return lhs.valueI32 & rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return lhs.valueI64 & rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return lhs.valueI32 & static_cast<i32>( std::stoll( rhs.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( lhs.valueString ) ) & lhs.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return lhs.valueI64 & std::stoll( rhs.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( lhs.valueString ) & rhs.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( lhs.valueString ) & std::stoll( rhs.valueString ) );
	}

	std::cerr << "Unhandled value types( " << lhs.type << ", " << rhs.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
}

Value & operator &= ( Value &lhs, const Value &rhs )
{
	lhs = lhs & rhs;
	return lhs;
}

Value operator | ( const Value &lhs, const Value &rhs )
{
	if ( lhs.type == ValueType::Undefined || rhs.type == ValueType::Undefined )
	{
		std::cerr << "Using '|' operator with( " << lhs.type << " | " << rhs.type << " )" << std::endl;
		return nullptr;
	}

	switch ( TYPE_PAIR( static_cast<i32>( lhs.type ), static_cast<i32>( rhs.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return lhs.valueI32 | rhs.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return lhs.valueI32 | rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return lhs.valueI64 | rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return lhs.valueI32 | static_cast<i32>( std::stoll( rhs.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( lhs.valueString ) ) | lhs.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return lhs.valueI64 | std::stoll( rhs.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( lhs.valueString ) | rhs.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( lhs.valueString ) | std::stoll( rhs.valueString ) );
	}

	std::cerr << "Unhandled value types( " << lhs.type << ", " << rhs.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
}

Value & operator |= ( Value &lhs, const Value &rhs )
{
	lhs = lhs | rhs;
	return lhs;
}

Value operator ^ ( const Value &lhs, const Value &rhs )
{
	if ( lhs.type == ValueType::Undefined || rhs.type == ValueType::Undefined )
	{
		std::cerr << "Using '^' operator with( " << lhs.type << " ^ " << rhs.type << " )" << std::endl;
		return nullptr;
	}

	switch ( TYPE_PAIR( static_cast<i32>( lhs.type ), static_cast<i32>( rhs.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return lhs.valueI32 ^ rhs.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return lhs.valueI32 ^ rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return lhs.valueI64 ^ rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return lhs.valueI32 ^ static_cast<i32>( std::stoll( rhs.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( lhs.valueString ) ) ^ lhs.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return lhs.valueI64 ^ std::stoll( rhs.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( lhs.valueString ) ^ rhs.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( lhs.valueString ) ^ std::stoll( rhs.valueString ) );
	}

	std::cerr << "Unhandled value types( " << lhs.type << ", " << rhs.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
}

Value & operator ^= ( Value &lhs, const Value &rhs )
{
	lhs = lhs ^ rhs;
	return lhs;
}

Value operator % ( const Value &lhs, const Value &rhs )
{
	if ( lhs.type == ValueType::Undefined || rhs.type == ValueType::Undefined )
	{
		std::cerr << "Using '%' operator with( " << lhs.type << " % " << rhs.type << " )" << std::endl;
		return nullptr;
	}

	switch ( TYPE_PAIR( static_cast<i32>( lhs.type ), static_cast<i32>( rhs.type ) ) )
	{
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI32 ):			return lhs.valueI32 % rhs.valueI32;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::NumberI64 ):			return lhs.valueI32 % rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI64, ValueType::NumberI64 ):			return lhs.valueI64 % rhs.valueI64;
	case TYPE_PAIR( ValueType::NumberI32, ValueType::StringLiteral ):		return lhs.valueI32 % static_cast<i32>( std::stoll( rhs.valueString ) );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI32 ):		return std::to_string( static_cast<i32>( std::stoll( lhs.valueString ) ) % lhs.valueI32 );
	case TYPE_PAIR( ValueType::NumberI64, ValueType::StringLiteral ):		return lhs.valueI64 % std::stoll( rhs.valueString );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::NumberI64 ):		return std::to_string( std::stoll( lhs.valueString ) % rhs.valueI64 );
	case TYPE_PAIR( ValueType::StringLiteral, ValueType::StringLiteral ):	return std::to_string( std::stoll( lhs.valueString ) % std::stoll( rhs.valueString ) );
	}

	std::cerr << "Unhandled value types( " << lhs.type << ", " << rhs.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
}

Value & operator %= ( Value &lhs, const Value &rhs )
{
	lhs = lhs % rhs;
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