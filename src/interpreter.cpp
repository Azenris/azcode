
#include <iostream>

#include "interpreter.h"

Value Interpreter::run( Node *node )
{
	switch ( node->type )
	{
	case NodeType::Entry:
		{
			data.clear();
			data.push_back(
			{
				// Built-In Variables
				{
					{ "Get-Version", { "0.0.1" } }
				},
			} );

			Value value;
			for ( auto child : node->children )
			{
				value = run( child );
				if ( child->type == NodeType::Return )
				{
					data.pop_back();
					return value;
				}
			}
			return value;
		}
		break;

	case NodeType::Block:
		{
			data.emplace_back();
			Value value;
			for ( auto child : node->children )
			{
				value = run( child );
				if ( child->type == NodeType::Return )
				{
					data.pop_back();
					return value;
				}
			}
			data.pop_back();
			return value;
		}
		break;

	case NodeType::EndStatement:
		break;

	case NodeType::Identifier:
		return get_value( node );

	case NodeType::StringLiteral:
		return node->value;

	case NodeType::Number:
		return node->value;

	case NodeType::Assignment:
		return get_or_create_value( node->left ) = run( node->right );

	case NodeType::AssignmentOp:
		switch ( node->token->id )
		{
		case TokenID::MinusAssign:		return get_value( node->left ) -= run( node->right ); break;
		case TokenID::PlusAssign:		return get_value( node->left ) += run( node->right ); break;
		case TokenID::DivideAssign:		return get_value( node->left ) /= run( node->right ); break;
		case TokenID::AsteriskAssign:	return get_value( node->left ) *= run( node->right ); break;
		case TokenID::AmpAssign:		return get_value( node->left ) &= run( node->right ); break;
		case TokenID::PipeAssign:		return get_value( node->left ) |= run( node->right ); break;
		case TokenID::HatAssign:		return get_value( node->left ) ^= run( node->right ); break;
		case TokenID::PercentAssign:	return get_value( node->left ) %= run( node->right ); break;
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

	case NodeType::DeclFunc:
		get_or_create_value( node->left ) = node;
		break;

	case NodeType::FunctionArgs:
		break;

	case NodeType::FunctionCall:
		{
			Value call = get_value( node->left );
			if ( call.type == ValueType::Node )
			{
				Node *funcNode = call.valueNode;
				if ( funcNode )
				{
					if ( funcNode->right->children.size() != node->children.size() )
					{
						std::cerr << "[Interpreter] Function wants " << funcNode->right->children.size() << " args, but was given " << node->children.size() << " args. (Line: " << node->token->line << ")" << std::endl;
						exit( RESULT_CODE_FUNCTION_ARG_COUNT );
					}

					// -- setup arguments --
					data.emplace_back();

					for ( i32 argIdx = 0, argCount = static_cast<i32>( node->children.size() ); argIdx < argCount; ++argIdx )
						data[ funcNode->scope ][ funcNode->right->children[ argIdx ]->value.valueString ] = run( node->children[ argIdx ] );

					// -- process the codeblock of the function --
					Value value;
					for ( auto child : funcNode->children )
					{
						value = run( child );
						if ( child->type == NodeType::Return )
						{
							data.pop_back();
							return value;
						}
					}
					data.pop_back();
					return value;
				}
			}
			else
			{
				std::cerr << "[Interpreter] Not callable \"" << node->left->value.valueString << "\" (Line: " << node->token->line << ")" << std::endl;
			}
		}
		break;

	case NodeType::Return:
		return run( node->left );
	}

	return Value();
}

Value &Interpreter::get_value( Node *node )
{
	for ( i32 scope = static_cast<i32>( data.size() ) - 1; scope >= 0; --scope )
	{
		auto iter = data[ scope ].find( node->value.valueString );
		if ( iter != data[ scope ].end() )
			return iter->second;
	}

	std::cerr << "[Interpreter] Variable unknown \"" << node->value.valueString << "\" (Line: " << node->token->line << ")" << std::endl;
	exit( RESULT_CODE_VARIABLE_UNKNOWN );
}

Value &Interpreter::get_or_create_value( Node *node )
{
	// Check if its a local variable, will use the current scope
	if ( node->scope == -1 )
		return data.back()[ node->value.valueString ];

	for ( i32 scope = static_cast<i32>( data.size() ) - 1; scope >= 0; --scope )
	{
		auto iter = data[ scope ].find( node->value.valueString );
		if ( iter != data[ scope ].end() )
			return iter->second;
	}

	return data[ 0 ][ node->value.valueString ];
}

void Interpreter::cleanup()
{
	data.clear();
}