
#include <iostream>
#include <print>

#include "interpreter.h"
#include "os.h"
#include "net.h"

static void breakable_codeblock( Interpreter *interpreter, Node *node, bool *flagBreak, bool *flagContinue, bool *flagReturn, Value *ret )
{
	*flagContinue = false;
	*flagBreak = false;
	*flagReturn = false;

	for ( auto child : node->children )
	{
		switch ( child->type )
		{
		case NodeID::Continue:
			*flagContinue = true;
			return;

		case NodeID::Break:
			*flagBreak = true;
			return;
		}

		*ret = interpreter->run( child );

		if ( ret->type == ValueType::Command )
		{
			switch ( ret->keywordID )
			{
			case KeywordID::Continue:
				*flagContinue = true;
				return;

			case KeywordID::Break:
				*flagBreak = true;
				return;
			}
		}

		switch ( child->type )
		{
		case NodeID::Return:
			*flagReturn = true;
			if ( ret->deref().scope == interpreter->scope )
				ret->unfold();
			interpreter->scope_pop();
		}
	}
}

static Value process_codeblock( Interpreter *interpreter, Node *node )
{
	interpreter->scope_push();

	Value value;

	for ( auto child : node->children )
	{
		switch ( child->type )
		{
		case NodeID::Continue:
			return KeywordID::Continue;

		case NodeID::Break:
			return KeywordID::Break;
		}

		value = interpreter->run( child );

		if ( value.type == ValueType::Command )
		{
			switch ( value.keywordID )
			{
			case KeywordID::Continue:
			case KeywordID::Break:
				return value;
			}
		}

		switch ( child->type )
		{
		case NodeID::Return:
			// check if the value will go out of scope with the return
			// it will have to pass-by-value
			if ( value.deref().scope == interpreter->scope )
				value.unfold();
			interpreter->scope_pop();
			return value;
		}
	}

	interpreter->scope_pop();

	return value;
}

static std::string build_string( Interpreter *interpreter, Node *node, Node *stringNode, Node *argNodes, bool addNewline )
{
	Value value = interpreter->run( stringNode );
	Value &str = value.deref();

	if ( str.type != ValueType::StringLiteral || str.valueString.empty() )
		interpreter->fatal( RESULT_CODE_PRINT_FORMAT_UNEXPECTED, node, "Println format expected as a string." );

	std::string ret;
	std::vector<Value> args;

	ret.reserve( str.valueString.size() + argNodes->children.size() * 32 );
	args.reserve( argNodes->children.size() );

	for ( auto argNode : argNodes->children )
		args.push_back( interpreter->run( argNode ) );

	const char *fmt = str.valueString.c_str();
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
					std::format_to( std::back_inserter( ret ), "{}{}", std::string_view( start, fmt - ( start + 2 ) ), args[ num ] );
					start = fmt;
				}
				else
				{
					interpreter->fatal( RESULT_CODE_PRINT_FORMAT_TOKEN_ID_UNEXPECTED, "Println format token id unexpected." );
				}
			}
			else
			{
				fmt += 1;
				std::format_to( std::back_inserter( ret ), "{}", std::string_view( start, fmt - start ) );
				fmt += 1;
				start = fmt;
			}
		}
		else
		{
			fmt += 1;
		}
	}

	std::format_to( std::back_inserter( ret ), "{}", std::string_view( start, fmt - start ) );

	if ( addNewline )
		std::format_to( std::back_inserter( ret ), "\n" );

	return ret;
}

void Interpreter::set_args( i32 argc, char *argv[] )
{
	for ( i32 i = 0; i < argc; ++i )
		programArgs.push_back( argv[ i ] );
}

Value Interpreter::run( std::vector<std::string> files, Node *node )
{
	filenames = std::move( files );

	builtInImports[ "args" ] = [this]()
	{
		Value arr( ValueType::Arr );
		for ( auto arg : programArgs )
			arr.arr.push_back( arg );
		get_or_create_global( "args" ) = arr;
	};

	builtInImports[ "os" ] = [this]()
	{
		Value lwo( ValueType::Struct );
		lwo.map[ "name" ] = OS_NAME;
		lwo.map[ "mkdir" ] = BuiltInNode_Struct_OS_MkDir;
		lwo.map[ "rm" ] = BuiltInNode_Struct_OS_Rm;
		lwo.map[ "open" ] = BuiltInNode_Struct_OS_Open;
		lwo.map[ "close" ] = BuiltInNode_Struct_OS_Close;
		lwo.map[ "exists" ] = BuiltInNode_Struct_OS_Exists;
		lwo.map[ "file_size" ] = BuiltInNode_Struct_OS_FileSize;
		lwo.map[ "read" ] = BuiltInNode_Struct_OS_Read;
		lwo.map[ "read_file" ] = BuiltInNode_Struct_OS_ReadFile;
		lwo.map[ "write" ] = BuiltInNode_Struct_OS_Write;
		get_or_create_global( "os" ) = lwo;
	};

	builtInImports[ "net" ] = [this]()
	{
		Value lwo( ValueType::Struct );
		lwo.map[ "init" ] = BuiltInNode_Struct_NET_Init;
		lwo.map[ "quit" ] = BuiltInNode_Struct_NET_Quit;
		lwo.map[ "sockopen" ] = BuiltInNode_Struct_NET_SockOpen;
		lwo.map[ "sockclose" ] = BuiltInNode_Struct_NET_SockClose;
		lwo.map[ "socktimeout" ] = BuiltInNode_Struct_NET_SockTimeout;
		lwo.map[ "sockname" ] = BuiltInNode_Struct_NET_SockName;
		lwo.map[ "bind" ] = BuiltInNode_Struct_NET_Bind;
		lwo.map[ "listen" ] = BuiltInNode_Struct_NET_Listen;
		lwo.map[ "connect" ] = BuiltInNode_Struct_NET_Connect;
		lwo.map[ "send" ] = BuiltInNode_Struct_NET_Send;
		lwo.map[ "recv" ] = BuiltInNode_Struct_NET_Recv;
		get_or_create_global( "net" ) = lwo;
	};

	return run( node );
}

Value Interpreter::run( Node *node )
{
	switch ( node->type )
	{
	case NodeID::Entry:
		{
			scope = SCOPE_GLOBAL;
			data.clear();
			Value value;
			for ( auto child : node->children )
			{
				value = run( child );
				if ( child->type == NodeID::Return )
					return value.get_as_i64( this, child );
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

	case NodeID::CreateStruct:
		{
			Value lwo( ValueType::Struct );
			// provided initialisation data
			for ( auto child : node->children )
			{
				switch ( child->type )
				{
				case NodeID::DeclFunc:
					lwo.map[ child->left->value.valueString ] = child;
					break;

				case NodeID::Assignment:
					lwo.map[ child->left->value.valueString ] = run( child->right );
					break;

				default:
					fatal( RESULT_CODE_UNEXPECTED_STRUCT_ASSIGNMENT_TYPE, child, "Unexpected struct assignment type \"{}\"", child->type );
				}
			}
			return lwo;
		}

	case NodeID::CreateArray:
		{
			Value arr( ValueType::Arr );
			// provided initialisation data
			for ( auto child : node->children )
				arr.arr.push_back( run( child ) );
			return arr;
		}
		break;

	case NodeID::ArrayAccess:
		return run( node->left )[ run( node->right ).get_as_i64( this, node ) ];

	case NodeID::Assignment:
		return run( node->left ) = run( node->right );

	case NodeID::Operation:
		switch ( node->token->id )
		{
		case TokenID::Minus:				return run( node->left ) - run( node->right );
		case TokenID::Plus:					return run( node->left ) + run( node->right );
		case TokenID::Divide:				return run( node->left ) / run( node->right );
		case TokenID::Asterisk:				return run( node->left ) * run( node->right );
		case TokenID::Amp:					return run( node->left ) & run( node->right );
		case TokenID::Pipe:					return run( node->left ) | run( node->right );
		case TokenID::Hat:					return run( node->left ) ^ run( node->right );
		case TokenID::Percent:				return run( node->left ) % run( node->right );
		case TokenID::DoubleAssign:			return run( node->left ) == run( node->right );
		case TokenID::ExclamationAssign:	return run( node->left ) != run( node->right );
		case TokenID::GreaterThan:			return run( node->left ) > run( node->right );
		case TokenID::GreaterOrEqual:		return run( node->left ) >= run( node->right );
		case TokenID::LesserThan:			return run( node->left ) < run( node->right );
		case TokenID::LesserOrEqual:		return run( node->left ) <= run( node->right );
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

	case NodeID::DeclFunc:
		get_or_create_value( node->left ) = node;
		break;

	case NodeID::FunctionArgs:
		break;

	case NodeID::FunctionCall:
		{
			chainedDotAccess = nullptr;

			Value ret = run( node->left );
			Value &call = ret.deref();

			if ( call.type == ValueType::Node )
			{
				Node *funcNode = call.valueNode;
				if ( funcNode )
				{
					if ( ( funcNode->right ? funcNode->right->children.size() : 0 ) != node->children.size() )
					{
						u64 wanted = ( funcNode->right ? funcNode->right->children.size() : 0 );
						fatal( RESULT_CODE_FUNCTION_ARG_COUNT, node, "Function wants {} args, but was given {} args.", wanted, node->children.size() );
					}

					scope_push( chainedDotAccess );

					// -- setup arguments --
					for ( u64 argIdx = 0, argCount = node->children.size(); argIdx < argCount; ++argIdx )
					{
						Node *argNode = funcNode->right->children[ argIdx ];
						get_or_create_value( data[ argNode->value.valueString ], scope, argNode ) = run( node->children[ argIdx ] );
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
								value.unfold();
							scope_pop();
							return value;
						}
					}

					scope_pop();
				}
			}
			else if ( call.type == ValueType::InbuiltFunc )
			{
				scope_push( chainedDotAccess );
				Value inbuiltReturn = call.valueInbuiltFunc( this, *context.back(), node );
				scope_pop();
				return inbuiltReturn;
			}
			else
			{
				fatal( RESULT_CODE_NOT_CALLABLE, node, "Not callable \"{}\"", node->left->value.valueString );
			}
		}

	case NodeID::If:
		{
			if ( run( node->left ).get_as_bool( this, node ) )
			{
				return process_codeblock( this, node );
			}
			else if ( node->right )
			{
				return run( node->right );
			}
		}
		break;

	case NodeID::Import:
		{
			std::function func = builtInImports[ node->left->value.valueString ];
			if ( func )
			{
				func();
			}
			else
			{
				fatal( RESULT_CODE_INVALID_IMPORT, node, "Invalid Import \"{}\"", node->left->value.valueString );
			}
		}
		break;

	case NodeID::Return:
		if ( node->left )
			return run( node->left );
		return node->value;

	case NodeID::Print:
		{
			if ( node->children.empty() )
			{
				std::print( "{}", run( node->left ) );
			}
			else
			{
				std::print( "{}", build_string( this, node, node->left, node, false ) );
			}
		}
		break;

	case NodeID::Println:
		{
			if ( node->children.empty() )
			{
				if ( node->left )
					std::println( "{}", run( node->left ) );
				else
					std::println( "" );
			}
			else
			{
				std::println( "{}", build_string( this, node, node->left, node, false ) );
			}
		}
		break;

	case NodeID::Assert:
		{
			if ( !run( node->left ).get_as_bool( this, node ) )
			{
				std::string temp;

				if ( node->right )
				{
					if ( node->children.empty() )
					{
						std::format_to( std::back_inserter( temp ), "{}\n", run( node->right ) );
					}
					else
					{
						std::format_to( std::back_inserter( temp ), "{}\n", build_string( this, node, node->right, node, true ) );
					}
				}

				fatal( RESULT_CODE_ASSERT_FAILED, node, "(ASSERT){}", temp );
			}
		}
		break;

	case NodeID::ForNumberRange:
		{
			scope_push();

			Value &v = get_or_create_value( node->left );

			i64 start = run( node->right ).get_as_i64( this, node->right );
			i64 end = run( node->right->right ).get_as_i64( this, node->right->right );
			i64 dir = ( end - start );

			dir = ( dir < 0 ? -1 : ( dir > 0 ? 1 : 0 ) );

			Value ret;
			bool flagBreak;
			bool flagContinue;
			bool flagReturn;

			for ( i64 i = start; true; i += dir )
			{
				v = i;

				breakable_codeblock( this, node, &flagBreak, &flagContinue, &flagReturn, &ret );

				if ( i == end )		break;
				if ( flagBreak )	break;
				if ( flagContinue )	continue;
				if ( flagReturn )	return ret;
			}

			scope_pop();
		}
		break;

	case NodeID::ForOfIdentifier:
		{
			scope_push();

			Value &v = get_or_create_value( node->left );
			Value &idx = get_or_create_value( "lidx" );
			Value identifier = run( node->right );
			Value &id = identifier.deref();

			Value ret;
			bool flagBreak;
			bool flagContinue;
			bool flagReturn;

			if ( id.type == ValueType::Arr )
			{
				i64 index = 0;

				for ( auto &entry : id.arr )
				{
					v = entry;
					idx = index;

					breakable_codeblock( this, node, &flagBreak, &flagContinue, &flagReturn, &ret );

					if ( flagBreak )	break;
					if ( flagContinue )	continue;
					if ( flagReturn )	return ret;

					index += 1;
				}
			}
			else if ( id.type == ValueType::Struct )
			{
				i64 index = 0;

				v.type = ValueType::Struct;
				Value &key = v.map[ "key" ];
				Value &value = v.map[ "value" ];

				for ( auto &entry : id.map )
				{
					key = entry.first;
					value = entry.second;
					idx = index;

					breakable_codeblock( this, node, &flagBreak, &flagContinue, &flagReturn, &ret );

					if ( flagBreak )	break;
					if ( flagContinue )	continue;
					if ( flagReturn )	return ret;

					index += 1;
				}
			}
			else
			{
				fatal( RESULT_CODE_VARIABLE_UNKNOWN, "Unexpected loop on variable type. {}", fail_at( node ) );
			}

			scope_pop();
		}
		break;

	case NodeID::ForOfIdentifierRange:
		{
			scope_push();

			Value &v = get_or_create_value( node->left );
			Value &idx = get_or_create_value( "lidx" );
			Value identifier = run( node->right );
			Value &id = identifier.deref();

			Node *startNode = node->right->right;
			Node *endNode = node->right->right->right;

			i64 start = run( startNode ).get_as_i64( this, startNode );
			i64 end = run( endNode ).get_as_i64( this, endNode );
			i64 dir = ( end - start );

			dir = ( dir < 0 ? -1 : ( dir > 0 ? 1 : 0 ) );

			Value ret;
			bool flagBreak;
			bool flagContinue;
			bool flagReturn;

			if ( id.type == ValueType::Arr )
			{
				for ( i64 i = start; true; i += dir )
				{
					if ( i < 0 || i >= static_cast<i64>( id.arr.size() ) )
					{
						fatal( RESULT_CODE_VARIABLE_UNKNOWN, "Loop index out of bounds. {}", fail_at( node ) );
					}

					v = id.arr[ i ];
					idx = i;

					breakable_codeblock( this, node, &flagBreak, &flagContinue, &flagReturn, &ret );

					if ( i == end )		break;
					if ( flagBreak )	break;
					if ( flagContinue )	continue;
					if ( flagReturn )	return ret;
				}
			}
			else
			{
				fatal( RESULT_CODE_VARIABLE_UNKNOWN, node, "Unexpected loop on variable type." );
			}

			scope_pop();
		}
		break;

	case NodeID::ForOfIdentifierRangeCount:
		{
			scope_push();

			Value &v = get_or_create_value( node->left );
			Value &idx = get_or_create_value( "lidx" );
			Value identifier = run( node->right );
			Value &id = identifier.deref();

			Node *startNode = node->right->right;
			Node *countNode = node->right->right->right;

			i64 start = run( startNode ).get_as_i64( this, startNode );
			i64 end = start + run( countNode ).get_as_i64( this, countNode );

			Value ret;
			bool flagBreak;
			bool flagContinue;
			bool flagReturn;

			if ( id.type == ValueType::Arr )
			{
				for ( i64 i = start; i < end; ++i )
				{
					if ( i < 0 || i >= static_cast<i64>( id.arr.size() ) )
					{
						fatal( RESULT_CODE_VARIABLE_UNKNOWN, "Loop index out of bounds. {}", fail_at( node ) );
					}

					v = id.arr[ i ];
					idx = i;

					breakable_codeblock( this, node, &flagBreak, &flagContinue, &flagReturn, &ret );

					if ( flagBreak )	break;
					if ( flagContinue )	continue;
					if ( flagReturn )	return ret;
				}
			}
			else
			{
				fatal( RESULT_CODE_VARIABLE_UNKNOWN, "Unexpected loop on variable type. {}", fail_at( node ) );
			}

			scope_pop();
		}
		break;

	case NodeID::While:
		{
			scope_push();

			if ( run( node->left ).get_as_bool( this, node->left ) )
			{
				Value &idx = get_or_create_value( "lidx" );
				Value ret;
				bool flagBreak;
				bool flagContinue;
				bool flagReturn;
				i32 index = 0;

				while ( true )
				{
					idx = index++;

					breakable_codeblock( this, node, &flagBreak, &flagContinue, &flagReturn, &ret );

					if ( !run( node->left ).get_as_bool( this, node->left ) )
						break;

					if ( flagBreak )	break;
					if ( flagContinue )	continue;
					if ( flagReturn )	return ret;
				}

				scope_pop();
			}
		}
		break;

	case NodeID::Exit:
		exit( static_cast<i32>( run( node->left ).get_as_i64( this, node->left ) ) );
	}

	return Value();
}

Value *Interpreter::chain_access( Node *node, Value *value )
{
	if ( value->type == ValueType::Undefined )
		return nullptr;

	const char *from = node->value.valueString.c_str();

	for ( auto &child : node->children )
	{
		Value &l = value->deref();

		auto subIter = l.map.find( child->value.valueString );

		if ( subIter == l.map.end() )
		{
			fatal( RESULT_CODE_VARIABLE_UNKNOWN, "\"{}\" is not a variable in \"{}\". {}", child->value.valueString, from, fail_at( child ) );
		}

		chainedDotAccess = value;
		value = &subIter->second;
		from = child->value.valueString.c_str();
		value->scope = scope;
	}

	return value;
}

Value *Interpreter::get_value_if_exists( Node *node )
{
	if ( node->left )
	{
		Value value = run( node->left );
		return chain_access( node, &value );
	}

	auto iter = data.find( node->value.valueString );
	if ( iter != data.end() )
	{
		std::vector<Value*> &values = iter->second;
		for ( i32 scopeIdx = std::min( scope, static_cast<i32>( values.size() ) - 1 ); scopeIdx >= 0; --scopeIdx )
		{
			if ( values[ scopeIdx ] )
			{
				Value *value = chain_access( node, values[ scopeIdx ] );
				if ( value )
					return value;
			}
		}
	}

	return nullptr;
}

Value &Interpreter::get_value( Node *node )
{
	if ( node->value.valueString == "self" )
	{
		Value *chainedValue = chain_access( node, context.back() );
		if ( chainedValue )
			return *chainedValue;
		fatal( RESULT_CODE_VARIABLE_UNKNOWN, "Variable unknown \"self\" {}", fail_at( node ) );
	}

	if ( node->left )
	{
		Value value = run( node->left );
		Value *chainedValue = chain_access( node, &value );
		if ( chainedValue )
			return *chainedValue;
		fatal( RESULT_CODE_VARIABLE_UNKNOWN, "Variable unknown \"{}\" {}", node->left->value.valueString, fail_at( node->left ) );
	}

	auto iter = data.find( node->value.valueString );
	if ( iter != data.end() )
	{
		std::vector<Value*> &values = iter->second;
		for ( i32 scopeIdx = std::min( scope, static_cast<i32>( values.size() ) - 1 ); scopeIdx >= 0; --scopeIdx )
		{
			if ( values[ scopeIdx ] )
			{
				Value *value = chain_access( node, values[ scopeIdx ] );
				if ( value )
					return *value;
			}
		}
	}

	fatal( RESULT_CODE_VARIABLE_UNKNOWN, "Variable unknown \"{}\" {}", node->value.valueString, fail_at( node ) );
}

Value &Interpreter::get_or_create_value( const char *name )
{
	std::vector<Value*> &values = data[ name ];
	for ( i32 i = 0, count = ( scope + 1 ) - static_cast<i32>( values.size() ); i < count; ++i )
		values.push_back( nullptr );
	Value *value = values[ scope ];
	if ( value )
		return *value;
	value = new Value;
	value->scope = scope;
	if ( scope > 0 )
		scopeWatch[ scope - 1 ].push_back( name );
	values[ scope ] = value;
	return *value;
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

	if ( node->value.valueString == "self" )
	{
		value = context.back();
	}
	else
	{
		std::vector<Value*> &values = data[ node->value.valueString ];

		// Check if its a local variable, will use the current scope
		if ( node->scope == SCOPE_LOCAL )
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
	}

	if ( value )
	{
		Value *lastValue = value;
		for ( auto &child : node->children )
		{
			value = &value->deref().map[ child->value.valueString ];
			if ( value->type == ValueType::Undefined )
				*value = Value( ValueType::Struct );
			value->scope = SCOPE_STRUCT;
			lastValue = value;
		}
	}

	return *value;
}

Value &Interpreter::get_or_create_global( const char *name )
{
	std::vector<Value*> &values = data[ name ];

	if ( values.empty() )
		values.push_back( nullptr );
	else if ( values[ SCOPE_GLOBAL ] )
		return *values[ SCOPE_GLOBAL ];

	Value *value = new Value;
	value->scope = SCOPE_GLOBAL;
	values[ SCOPE_GLOBAL ] = value;

	return *value;
}

void Interpreter::cleanup()
{
	data.clear();
}

void Interpreter::scope_push( Value *newContext )
{
	context.push_back( newContext );
	scope += 1;
	scopeWatch.emplace_back();
}

void Interpreter::scope_pop()
{
	context.pop_back();

	for ( auto &entry : scopeWatch.back() )
	{
		std::vector<Value*> &values = data[ entry ];
		delete values.back();
		values.pop_back();
	}

	scopeWatch.pop_back();
	scope -= 1;
}

std::string Interpreter::fail_at( Node *node ) const
{
	return "(Line: " + std::to_string( node->token->line ) + ") (File: " + filenames[ node->token->file ] + ")";
}

void Interpreter::expect_arg( const char *name, Node *args, i32 expect )
{
	if ( args->children.size() != expect )
		fatal( RESULT_CODE_INVALID_ARGS_BUILTIN_FUNC, args, "{} received incorrect arguments. Got () expected ().", name, args->children.size(), expect );
}