
#pragma once

#include <vector>
#include <unordered_map>

#include "parser.h"

struct Interpreter
{
	using ValueMap = std::unordered_map<std::string, std::vector<Value*>>;

	ValueMap data;
	std::vector<std::vector<std::string>> scopeWatch;
	int scope;
	Value *chainedDotAccess;
	std::vector<Value *> context;
	std::vector<std::string> filenames;

	Value run( std::vector<std::string> files, Node *node );
	Value run( Node *node );
	Value *chain_access( Node *node, Value *value );
	Value *get_value_if_exists( Node *node );
	Value &get_value( Node *node );
	Value &get_or_create_value( std::vector<Value*> &values, i32 valueScope, Node *node );
	Value &get_or_create_value( Node *node );

	void cleanup();
	void scope_push( Value *newContext = nullptr );
	void scope_pop();
	std::string fail_at( Node *node );
};