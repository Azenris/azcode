
#include <iostream>

#include "value.h"

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
	case ValueType::Reference: l.valueRef = r.valueRef; break;
	}

	l.valueString = r.valueString;
	l.arr = r.arr;
	l.map = r.map;

	return *this;
}

bool Value::get_as_bool( Node *node )
{
	switch ( type )
	{
	case ValueType::Undefined: return false;
	case ValueType::NumberI32: return valueI32 != 0;
	case ValueType::NumberI64: return valueI64 != 0;
	case ValueType::StringLiteral: return !valueString.empty();
	case ValueType::Struct: return !map.empty();
	case ValueType::Arr: return !arr.empty();
	case ValueType::TokenID: return true;
	case ValueType::KeywordID: return true;
	case ValueType::Node: return valueNode;
	case ValueType::Reference: return valueRef->get_as_bool( node );
	}

	std::cerr << "Cannot convert from " << *this << " to bool. (Line: " << node->token->line << ")" << std::endl;
	exit( RESULT_CODE_VALUE_CANNOT_CONVERT );
}

i64 Value::get_as_i64( Node *node )
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
		return valueRef->get_as_i64( node );
	}

	std::cerr << "Cannot convert from " << *this << " to i64. (Line: " << node->token->line << ")" << std::endl;
	exit( RESULT_CODE_VALUE_CANNOT_CONVERT );
}

std::string Value::get_as_string( Node *node )
{
	switch ( type )
	{
	case ValueType::Undefined: return "Undefined";
	case ValueType::NumberI32: return std::to_string( valueI32 );
	case ValueType::NumberI64: return std::to_string( valueI64 );
	case ValueType::StringLiteral: return valueString;
	case ValueType::Reference: return valueRef->get_as_string( node );
	}

	std::cerr << "Cannot convert from " << *this << " to std::string. (Line: " << node->token->line << ")" << std::endl;
	exit( RESULT_CODE_VALUE_CANNOT_CONVERT );
}

Value Value::operator [] ( i64 index )
{
	if ( type == ValueType::Reference )
		return (*this->valueRef)[ index ];

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
	case ValueType::Reference: return valueRef->count();
	}
	return 0;
}

void Value::clear()
{
	type = ValueType::Undefined;
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

std::ostream & operator << ( std::ostream &out, const Value &value )
{
	switch ( value.type )
	{
	case ValueType::Undefined:			return out << "Undefined";
	case ValueType::NumberI32:			return out << value.valueI32;
	case ValueType::NumberI64:			return out << value.valueI64;
	case ValueType::StringLiteral:		return out << value.valueString;

	case ValueType::Struct:
		out << "{ ";
		if ( !value.map.empty() )
		{
			auto iter = value.map.begin();
			out << iter->first << ":" << iter->second;
			for ( ++iter; iter != value.map.end(); ++iter )
				out << ", " << iter->first << ":" << iter->second;
		}
		return out << " }";

	case ValueType::Arr:
		out << "[ ";
		if ( !value.arr.empty() )
		{
			out << value.arr[ 0 ];
			for ( u64 i = 1, count = value.arr.size(); i < count; ++i )
				out << ", " << value.arr[ i ];
		}
		return out << " ]{" << value.arr.size() << "}";

	case ValueType::TokenID:			return out << value.valueString << " (id:" << static_cast<i32>( value.tokenID ) << ")";
	case ValueType::KeywordID:			return out << value.valueString << " (id:" << static_cast<i32>( value.keywordID ) << ")";
	case ValueType::Node:				return out << "Node[" << value.valueNode << "]";
	case ValueType::Reference:			return out << *value.valueRef;
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
	case ValueType::Struct:				return out << "Struct";
	case ValueType::Arr:				return out << "Array";
	case ValueType::TokenID:			return out << "TokenID";
	case ValueType::KeywordID:			return out << "KeywordID";
	case ValueType::Node:				return out << "Node";
	case ValueType::Reference:			return out << "Reference";
	}

	return out << "Unhandled value ValueType( '" << static_cast<i32>( valueType ) << "' )";
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

	std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_COMPARITOR );
}

bool operator != ( const Value &lhs, const Value &rhs )
{
	return !( lhs == rhs );
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

	std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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
		std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
		exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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

	std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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
		std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
		exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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

	std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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
		std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
		exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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

	std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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
		std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
		exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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

	std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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
		std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
		exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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

	std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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
		std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
		exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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

	std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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
		std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
		exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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

	std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
	exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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
		std::cerr << "Unhandled value types( " << l.type << ", " << r.type << " )" << std::endl;
		exit( RESULT_CODE_VALUE_UNDEFINED_ARITHMETIC );
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