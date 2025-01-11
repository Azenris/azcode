
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
		break;

	case NodeType::StringLiteral:
		return node->value;

	case NodeType::Number:
		return node->value;

	case NodeType::Assignment:
		{
			Value vv = run( node->right );
			data[ node->left->value.valueString ] = vv;
			//data[ node->left->value.valueString ] = run( node->right );
			
			std::cout << "temp: assigning var(\"" << node->left->value.valueString << "\") to " << vv << std::endl;
		}
		
		// {
			// i64 
		// }
		
	
		// parser_consume( parser, TokenID::Assign );
		// Node *node = new_node( NodeType::Assignment );
		// node->token = token;
		// node->value = parser_parse( parser );
		break;

	case NodeType::Operation:
		switch ( node->token->id )
		{
		case TokenID::Minus:			value = node->left->value - node->right->value; break;
		case TokenID::Plus:				value = node->left->value + node->right->value; break;
		case TokenID::Divide:			value = node->left->value / node->right->value; break;
		case TokenID::Asterisk:			value = node->left->value * node->right->value; break;
		case TokenID::MinusAssign:		value = node->left->value - node->right->value; break;
		case TokenID::PlusAssign:		value = node->left->value + node->right->value; break;
		case TokenID::DivideAssign:		value = node->left->value / node->right->value; break;
		case TokenID::AsteriskAssign:	value = node->left->value - node->right->value; break;
		}
		break;

	case NodeType::FunctionCall:
		break;

	case NodeType::Return:
		return run( node->left );
	}

	return value;
}