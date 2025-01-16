
#include <iostream>
#include <print>

#include "interpreter.h"

static Value process_codeblock( Interpreter *interpreter, Node *node )
{
	interpreter->scope_push();
	Value value;
	for ( auto child : node->children )
	{
		value = interpreter->run( child );
		if ( child->type == NodeID::Return )
		{
			// check if the value will go out of scope with the return
			// it will have to pass-by-value
			if ( value.deref().scope == interpreter->scope )
				value = value.deref();
			interpreter->scope_pop();
			return value;
		}
	}
	interpreter->scope_pop();
	return value;
}

static void print_string( Interpreter *interpreter, std::ostream &out, Node *node, Node *conditionNode, Node *argNodes )
{
	Value value = interpreter->run( conditionNode );

	if ( value.type == ValueType::StringLiteral && !value.valueString.empty() )
	{
		std::vector<Value> args;
		args.reserve( argNodes->children.size() );
		for ( auto argNode : argNodes->children )
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
						out << std::string_view( start, fmt - ( start + 2 ) ) << args[ num ];
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
					out << std::string_view( start, fmt - start );
					fmt += 1;
					start = fmt;
				}
			}
			else
			{
				fmt += 1;
			}
		}
		out << std::string_view( start, fmt - start );
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
			scope = 0;

			data.clear();
			// data.insert(
			// {
				// Built-In Variables
				// { "$Version", { { "0.0.1" } } }
			// } );

			Value value;
			for ( auto child : node->children )
			{
				value = run( child );
				if ( child->type == NodeID::Return )
					return value.get_as_i64( child );
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
		return run( node->left )[ run( node->right ).get_as_i64( node ) ];

	case NodeID::Count:
		{
			Value &value = get_value( node->left );

			if ( value.type == ValueType::Struct )
			{
				auto iter = value.map.find( node->token->value.get_as_string( node ) );
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

					scope_push();

					int funcScope = scope;

					for ( i32 argIdx = 0, argCount = static_cast<i32>( node->children.size() ); argIdx < argCount; ++argIdx )
					{
						Node *argNode = funcNode->right->children[ argIdx ];
						get_or_create_value( data[ argNode->value.valueString ], funcScope, argNode ) = run( node->children[ argIdx ] );
					}

					// -- process the codeblock of the function --
					for ( auto child : funcNode->children )
					{
						Value value = run( child );
						if ( child->type == NodeID::Return )
						{
							// check if the value will go out of scope with the return
							// it will have to pass-by-value
							if ( value.deref().scope == scope )
								value = value.deref();
							scope_pop();
							return value;
						}
					}

					scope_pop();
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
			if ( run( node->left ).get_as_bool( node ) )
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
				print_string( this, std::cout, node, node->left, node );
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
				print_string( this, std::cout, node, node->left, node );
				std::cout << std::endl;
			}
		}
		break;

	case NodeID::Assert:
		{
			if ( !run( node->left ).get_as_bool( node ) )
			{
				std::cerr << "[Interpreter] (ASSERT) (Line: " << node->token->line << ")";

				if ( node->right )
				{
					std::cerr << ": ";

					if ( node->children.empty() )
					{
						std::cerr << run( node->right ) << std::endl;
					}
					else
					{
						print_string( this, std::cerr, node, node->right, node );
						std::cerr << std::endl;
					}
				}
				else
				{
					std::cerr << std::endl;
				}

				exit( RESULT_CODE_ASSERT_FAILED );
			}
		}
		break;
	}

	return Value();
}

Value &Interpreter::get_value( Node *node )
{
	auto iter = data.find( node->value.valueString );
	if ( iter != data.end() )
	{
		std::vector<Value*> &values = iter->second;
		for ( i32 scopeIdx = std::min( scope, static_cast<i32>( values.size() ) - 1 ); scopeIdx >= 0; --scopeIdx )
		{
			if ( values[ scopeIdx ] )
			{
				Value *value = values[ scopeIdx ];
				if ( value->type != ValueType::Undefined )
				{
					const char *from = node->value.valueString.c_str();
					for ( auto &child : node->children )
					{
						auto subIter = value->map.find( child->value.valueString );
						if ( subIter == value->map.end() )
						{
							std::cerr << "[Interpreter] \"" << child->value.valueString << "\" is not a variable in \"" << from << "\". (Line: " << child->token->line << ")" << std::endl;
							exit( RESULT_CODE_VARIABLE_UNKNOWN );
						}
						value = &subIter->second;
						from = child->value.valueString.c_str();
						value->scope = scope;
					}
					return *value;
				}
			}
		}
	}

	std::cerr << "[Interpreter] Variable unknown \"" << node->value.valueString << "\" (Line: " << node->token->line << ")" << std::endl;
	exit( RESULT_CODE_VARIABLE_UNKNOWN );
}

Value &Interpreter::get_or_create_value( std::vector<Value*> &values, i32 valueScope, Node *node )
{
	for ( i32 i = 0, count = ( valueScope + 1 ) - static_cast<i32>( values.size() ); i < count; ++i )
		values.push_back( nullptr );
	Value *value = values[ valueScope ];
	if ( value )
		return *value;
	value = new Value;
	value->scope = valueScope;
	if ( valueScope > 0 )
		scopeWatch[ valueScope - 1 ].push_back( node->value.valueString );
	values[ valueScope ] = value;
	return *value;
}

Value &Interpreter::get_or_create_value( Node *node )
{
	Value *value = nullptr;

	std::vector<Value*> &values = data[ node->value.valueString ];

	// Check if its a local variable, will use the current scope
	if ( node->scope == -1 )
	{
		value = &get_or_create_value( values, scope, node );
	}
	else
	{
		bool found = false;

		for ( i32 scopeIdx = std::min( scope, static_cast<i32>( values.size() ) - 1 ); scopeIdx >= 0; --scopeIdx )
		{
			if ( values[ scopeIdx ] )
			{
				value = values[ scopeIdx ];
				found = true;
				break;
			}
		}

		if ( !found )
			value = &get_or_create_value( values, 0, node );
	}

	if ( value )
	{
		for ( auto &child : node->children )
		{
			value = &value->map[ child->value.valueString ];
			value->scope = -4;
		}
	}

	return *value;
}

void Interpreter::cleanup()
{
	data.clear();
}

void Interpreter::scope_push()
{
	scope += 1;
	scopeWatch.emplace_back();
}

void Interpreter::scope_pop()
{
	for ( auto &entry : scopeWatch.back() )
	{
		std::vector<Value*> &values = data[ entry ];
		delete values.back();
		values.pop_back();
	}

	scopeWatch.pop_back();
	scope -= 1;
}