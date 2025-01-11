
#include <iostream>

#include "interpreter.h"

Value Interpreter::run( Node *node )
{
	Value value = process_node( node );
	if ( value.type == ValueType::NumberI32 )
		return { static_cast<i32>( 0 ) };
	return value;
}

Value Interpreter::process_node( Node *node )
{
	Value value;

	while ( node )
	{
		switch ( node->type )
		{
		case NodeType::Block:
			for ( auto child : node->children )
				value = process_node( child );
			break;

		case NodeType::EndStatement:
			break;

		case NodeType::Identifier:
			break;

		case NodeType::StringLiteral:
			break;

		case NodeType::Number:
			break;

		case NodeType::Assignment:
			break;

		case NodeType::Operation:
			break;

		case NodeType::FunctionCall:
			break;
		}
	}

	return value;
}

Value &Interpreter::get_or_create_variable( Node *node )
{
	
}

Value *Interpreter::get_variable( Node *node )
{
	
}