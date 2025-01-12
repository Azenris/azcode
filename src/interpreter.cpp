
#include <iostream>

#include "interpreter.h"

Value Interpreter::run( Node *node )
{
	Value value;

	switch ( node->type )
	{
	case NodeType::Block:
		for ( auto child : node->children )
		{
			value = run( child );
			// TODO : if the node was  a return, it should break from this block
		}
		break;

	case NodeType::EndStatement:
		break;

	case NodeType::Identifier:
		return data[ node->value.valueString ];

	case NodeType::StringLiteral:
		return node->value;

	case NodeType::Number:
		return node->value;

	case NodeType::Assignment:
		{
			//data[ node->left->value.valueString ] = run( node->right );
			Value vv = run( node->right );
			data[ node->left->value.valueString ] = vv;
			std::cout << "temp: assigning var(\"" << node->left->value.valueString << "\") to " << vv << std::endl;
		}
		break;

	case NodeType::Operation:
		switch ( node->token->id )
		{
		case TokenID::Minus:			value = run( node->left ) - run( node->right ); break;
		case TokenID::Plus:				value = run( node->left ) + run( node->right ); break;
		case TokenID::Divide:			value = run( node->left ) / run( node->right ); break;
		case TokenID::Asterisk:			value = run( node->left ) * run( node->right ); break;
		case TokenID::MinusAssign:		value = run( node->left ) - run( node->right ); break;
		case TokenID::PlusAssign:		value = run( node->left ) + run( node->right ); break;
		case TokenID::DivideAssign:		value = run( node->left ) / run( node->right ); break;
		case TokenID::AsteriskAssign:	value = run( node->left ) - run( node->right ); break;
		}
		break;

	case NodeType::FunctionCall:
		break;

	case NodeType::Return:
		return run( node->left );
	}

	return value;
}