
#include <iostream>

#include "interpreter.h"

Value Interpreter::run( Node *node )
{
	switch ( node->type )
	{
	case NodeType::Block:
		{
			Value value;
			for ( auto child : node->children )
			{
				value = run( child );
				// TODO : if the node was  a return, it should break from this block
			}
			return value;
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
		return data[ node->left->value.valueString ] = run( node->right );

	case NodeType::AssignmentOp:
		switch ( node->token->id )
		{
		case TokenID::MinusAssign:		return data[ node->left->value.valueString ] -= run( node->right ); break;
		case TokenID::PlusAssign:		return data[ node->left->value.valueString ] += run( node->right ); break;
		case TokenID::DivideAssign:		return data[ node->left->value.valueString ] /= run( node->right ); break;
		case TokenID::AsteriskAssign:	return data[ node->left->value.valueString ] *= run( node->right ); break;
		case TokenID::AmpAssign:		return data[ node->left->value.valueString ] &= run( node->right ); break;
		case TokenID::PipeAssign:		return data[ node->left->value.valueString ] |= run( node->right ); break;
		case TokenID::HatAssign:		return data[ node->left->value.valueString ] ^= run( node->right ); break;
		case TokenID::PercentAssign:	return data[ node->left->value.valueString ] %= run( node->right ); break;
		}
		break;

	case NodeType::Operation:
		switch ( node->token->id )
		{
		case TokenID::Minus:			return run( node->left ) - run( node->right ); break;
		case TokenID::Plus:				return run( node->left ) + run( node->right ); break;
		case TokenID::Divide:			return run( node->left ) / run( node->right ); break;
		case TokenID::Asterisk:			return run( node->left ) * run( node->right ); break;
		case TokenID::Amp:				return run( node->left ) & run( node->right ); break;
		case TokenID::Pipe:				return run( node->left ) | run( node->right ); break;
		case TokenID::Hat:				return run( node->left ) ^ run( node->right ); break;
		case TokenID::Percent:			return run( node->left ) % run( node->right ); break;
		}
		break;

	case NodeType::FunctionCall:
		break;

	case NodeType::Return:
		return run( node->left );
	}

	return Value();
}

void Interpreter::cleanup()
{
	data.clear();
}