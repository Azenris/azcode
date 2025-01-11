
#include <iostream>

#include "value.h"

std::ostream & operator << ( std::ostream &out, const Value &value )
{
	switch ( value.type )
	{
	case ValueType::Undefined:		return out << "Undefined";
	case ValueType::NumberI32:		return out << value.valueI32;
	case ValueType::NumberI64:		return out << value.valueI64;
	case ValueType::StringLiteral:	return out << value.valueString;
	}

	return out << "Unhandled value ValueType( " << static_cast<i32>( value.type ) << " )";
}