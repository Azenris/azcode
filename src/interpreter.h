
#pragma once

#include <vector>
#include <unordered_map>
#include <functional>

#include "parser.h"

struct Interpreter
{
	using ValueMap = std::unordered_map<std::string, std::vector<Value*>>;

	ValueMap data;
	std::vector<std::vector<std::string>> scopeWatch;
	i32 scope;
	Value *chainedDotAccess;
	std::vector<Value *> context;
	std::vector<std::string> filenames;
	std::unordered_map<std::string, std::function<void()>> builtInImports;
	std::vector<std::string> programArgs;

	void set_args( i32 argc, char *argv[] );
	Value run( std::vector<std::string> files, Node *node );
	Value run( Node *node );
	Value *chain_access( Node *node, Value *value );
	Value *get_value_if_exists( Node *node );
	Value &get_value( Node *node );
	Value &get_or_create_value( const char *name );
	Value &get_or_create_value( std::vector<Value*> &values, i32 valueScope, Node *node );
	Value &get_or_create_value( Node *node );
	Value &get_or_create_global( const char *name );

	void cleanup();

	void scope_push( Value *newContext = nullptr );
	void scope_pop();

	void expect_arg( const char *name, Node *args, i32 expect );
	std::string fail_at( Node *node ) const;

	[[noreturn]] void fatal( RESULT_CODE resultCode, const char *message ) const
	{
		std::println( stderr, "[Interpreter] {}", message );
		exit( resultCode );
	}

	template <typename... T>
	[[noreturn]] void fatal( RESULT_CODE resultCode, std::format_string<T...> fmt, T&&... args ) const
	{
		std::println( stderr, "[Interpreter] {}", std::format( fmt, std::forward<T>( args )...) );
		exit( resultCode );
	}

	[[noreturn]] void fatal( RESULT_CODE resultCode, Node *node, const char *message ) const
	{
		std::println( stderr, "[Interpreter] {} {}", message, fail_at( node ) );
		exit( resultCode );
	}

	template <typename... T>
	[[noreturn]] void fatal( RESULT_CODE resultCode, Node *node, std::format_string<T...> fmt, T&&... args ) const
	{
		std::println( stderr, "[Interpreter] {} {}", std::format( fmt, std::forward<T>( args )...), fail_at( node ) );
		exit( resultCode );
	}
};