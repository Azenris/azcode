
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
		if ( child->type == NodeID::Return )
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
			if ( *fmt == '%' )
			{
				if ( *( fmt + 1 ) != '%' )
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
				else
				{
					fmt += 1;
					std::cout << std::string_view( start, fmt - start );
					fmt += 1;
					start = fmt;
				}
			}
			else
			{
				fmt += 1;
			}
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
	case NodeID::Entry:
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
				if ( child->type == NodeID::Return )
					return value;
			}
			return value;
		}
		break;

	case NodeID::Block:
		return process_codeblock( this, node );

	case NodeID::Identifier:
		return &get_value( node );

	case NodeID::CreateIdentifier:
		return &get_or_create_value( node );

	case NodeID::StringLiteral:
		return node->value;

	case NodeID::Number:
		return node->value;

	case NodeID::StructAssignment:
		{
			Value &lwo = get_or_create_value( node->left );
			lwo.clear();
			lwo.type = ValueType::Struct;
			for ( auto child : node->children )
			{
				switch ( child->type )
				{
				case NodeID::StructAssignment:
					lwo.map[ child->left->value.valueString ] = run( child );
					break;

				case NodeID::DeclFunc:
					lwo.map[ child->left->value.valueString ] = child;
					break;

				case NodeID::Assignment:
					lwo.map[ child->left->value.valueString ] = run( child->right );
					break;

				default:
					std::cerr << "[Interpreter] Unexpected struct assignment type \"" << child->type << "\" (Line: " << child->token->line << ")" << std::endl;
					exit( RESULT_CODE_UNEXPECTED_STRUCT_ASSIGNMENT_TYPE );
				}
			}
			return &lwo;
		}

	case NodeID::CreateArray:
		{
			Value arr( ValueType::Arr );
			for ( auto child : node->children )
				arr.arr.push_back( run( child ) );
			return arr;
		}
		break;

	case NodeID::ArrayAccess:
		return run( node->left )[ static_cast<i64>( run( node->right ) ) ];

	case NodeID::Count:
		{
			Value &value = get_value( node->left );

			if ( value.type == ValueType::Struct )
			{
				auto iter = value.map.find( static_cast<std::string>( node->token->value ) );
				if ( iter != value.map.end() )
					return iter->second;
			}
			return value.count();
		}
		break;

	case NodeID::Assignment:
		return run( node->left ) = run( node->right );

	case NodeID::Operation:
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

	case NodeID::AssignmentOp:
		switch ( node->token->id )
		{
		case TokenID::MinusAssign:
			{
				Value value = run( node->left );
				return value -= run( node->right );
			}

		case TokenID::PlusAssign:
			{
				Value value = run( node->left );
				return value += run( node->right );
			}

		case TokenID::DivideAssign:
			{
				Value value = run( node->left );
				return value /= run( node->right );
			}

		case TokenID::AsteriskAssign:
			{
				Value value = run( node->left );
				return value *= run( node->right );
			}

		case TokenID::AmpAssign:
			{
				Value value = run( node->left );
				return value &= run( node->right );
			}

		case TokenID::PipeAssign:
			{
				Value value = run( node->left );
				return value |= run( node->right );
			}

		case TokenID::HatAssign:
			{
				Value value = run( node->left );
				return value ^= run( node->right );
			}

		case TokenID::PercentAssign:
			{
				Value value = run( node->left );
				return value %= run( node->right );
			}
		}
		break;

	case NodeID::Equal:
		return run( node->left ) == run( node->right );

	case NodeID::NotEqual:
		return run( node->left ) != run( node->right );

	case NodeID::DeclFunc:
		get_or_create_value( node->left ) = node;
		break;

	case NodeID::FunctionArgs:
		break;

	case NodeID::FunctionCall:
		{
			Value ret = run( node->left );
			Value &call = ret.deref();

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
					for ( auto child : funcNode->children )
					{
						Value value = run( child );
						if ( child->type == NodeID::Return )
						{
							// check if the value will go out of scope with the return
							// it will have to pass-by-value
							if ( value.deref().scope == static_cast<i32>( data.size() - 1 ) )
								value = value.deref();
							data.pop_back();
							return value;
						}
					}
					data.pop_back();
				}
			}
			else
			{
				std::cerr << "[Interpreter] Not callable \"" << node->left->value.valueString << "\" (Line: " << node->token->line << ")" << std::endl;
				exit( RESULT_CODE_UNEXPECTED_NOT_CALLABLE );
			}
		}

	case NodeID::If:
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

	case NodeID::Return:
		return run( node->left );

	case NodeID::Print:
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

	case NodeID::Println:
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
	Value *value = nullptr;

	for ( i32 scope = static_cast<i32>( data.size() ) - 1; scope >= 0; --scope )
	{
		auto iter = data[ scope ].find( node->value.valueString );
		if ( iter != data[ scope ].end() )
		{
			value = &iter->second;
			break;
		}
	}

	if ( value && value->type != ValueType::Undefined )
	{
		value->scope = static_cast<i32>( data.size() - 1 );

		for ( auto &child : node->children )
		{
			value = &value->map[ child->value.valueString ];
			if ( !value )
			{
				std::cerr << "[Interpreter] Variable unknown \"" << node->value.valueString << "\" (Line: " << node->token->line << ")" << std::endl;
				exit( RESULT_CODE_VARIABLE_UNKNOWN );
			}

			value->scope = static_cast<i32>( data.size() - 1 );
		}

		return *value;
	}

	std::cerr << "[Interpreter] Variable unknown \"" << node->value.valueString << "\" (Line: " << node->token->line << ")" << std::endl;
	exit( RESULT_CODE_VARIABLE_UNKNOWN );
}

Value &Interpreter::get_or_create_value( Node *node )
{
	Value *value = nullptr;

	// Check if its a local variable, will use the current scope
	if ( node->scope == -1 )
	{
		value = &data.back()[ node->value.valueString ];
		value->scope = static_cast<i32>( data.size() - 1 );
	}
	else
	{
		bool found = false;

		for ( i32 scope = static_cast<i32>( data.size() ) - 1; scope >= 0; --scope )
		{
			auto iter = data[ scope ].find( node->value.valueString );

			if ( iter != data[ scope ].end() )
			{
				value = &iter->second;
				found = true;
				break;
			}
		}

		if ( !found )
		{
			value = &data[ 0 ][ node->value.valueString ];
			value->scope = static_cast<i32>( data.size() - 1 );
		}
	}

	if ( value )
	{
		int scope = value->scope;

		for ( auto &child : node->children )
		{
			value = &value->map[ child->value.valueString ];
			value->scope = scope;
		}
	}

	return *value;
}

void Interpreter::cleanup()
{
	data.clear();
}