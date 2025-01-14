
#include <iostream>
#include <print>

#include "interpreter.h"

static Value process_codeblock( Interpreter *interpreter, Node *node )
{
	interpreter->data.emplace_back();
	Value value;
	for ( auto child : node->children )
	{
		value = interpreter->run( child );
		if ( child->type == NodeType::Return )
		{
			interpreter->data.pop_back();
			return value;
		}
	}
	interpreter->data.pop_back();
	return value;
}

static void print_string( Interpreter *interpreter, Node *node )
{
	Value value = interpreter->run( node->left );

	if ( value.type == ValueType::StringLiteral && !value.valueString.empty() )
	{
		std::vector<Value> args;
		args.reserve( node->children.size() );
		Node *arg = node->right;
		for ( auto argNode : node->children )
			args.push_back( interpreter->run( argNode ) );
		const char *fmt = value.valueString.c_str();
		const char *start = fmt;
		char *end;
		while ( *fmt != '\0' )
		{
			if ( *fmt == '%' && *( fmt + 1 ) != '%' )
			{
				fmt += 1;

				i32 num;
				if ( to_int( &num, fmt, &end ) == ToIntResult::Success && num >= 0 && num < static_cast<i32>( args.size() ) )
				{
					fmt = end;
					std::cout << std::string_view( start, fmt - ( start + 2 ) ) << args[ num ];
					start = fmt;
				}
				else
				{
					std::cerr << "[Interpreter] Println format token id unexpected. (Line: " << node->token->line << ")" << std::endl;
					exit( RESULT_CODE_PRINT_FORMAT_TOKEN_ID_UNEXPECTED );
				}
			}

			fmt += 1;
		}
		std::cout << std::string_view( start, fmt - start );
	}
	else
	{
		std::cerr << "[Interpreter] Println format expected as a string. (Line: " << node->token->line << ")" << std::endl;
		exit( RESULT_CODE_PRINT_FORMAT_UNEXPECTED );
	}
}

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
					return value;
			}
			return value;
		}
		break;

	case NodeType::Block:
		return process_codeblock( this, node );

	case NodeType::Identifier:
		return get_value( node );

	case NodeType::StringLiteral:
		return node->value;

	case NodeType::Number:
		return node->value;

	case NodeType::CreateArray:
		{
			Value arr( ValueType::Arr );
			for ( auto child : node->children )
				arr.arr.push_back( run( child ) );
			return arr;
		}

	case NodeType::ArrayAccess:
		return run( node->left )[ static_cast<i64>( run( node->right ) ) ];

	case NodeType::Count:
		return run( node->left ).count();

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

	case NodeType::Equal:
		return run( node->left ) == run( node->right );

	case NodeType::NotEqual:
		return run( node->left ) != run( node->right );

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
					if ( ( funcNode->right ? funcNode->right->children.size() : 0 ) != node->children.size() )
					{
						std::cerr << "[Interpreter] Function wants " << ( funcNode->right ? funcNode->right->children.size() : 0 )
							<< " args, but was given " << node->children.size() << " args. (Line: " << node->token->line << ")" << std::endl;
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

	case NodeType::If:
		{
			if ( run( node->left ) )
			{
				return process_codeblock( this, node );
			}
			else if ( node->right )
			{
				return run( node->right );
			}
		}
		break;

	case NodeType::Return:
		return run( node->left );

	case NodeType::Print:
		{
			if ( node->children.empty() )
			{
				std::cout << run( node->left );
			}
			else
			{
				print_string( this, node );
			}
		}
		break;

	case NodeType::Println:
		{
			if ( node->children.empty() )
			{
				std::cout << run( node->left ) << std::endl;
			}
			else
			{
				print_string( this, node );
				std::cout << std::endl;
			}
		}
		break;
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