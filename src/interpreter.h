
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

	Value run( Node *node );
	Value &get_or_create_value( std::vector<Value*> &values, i32 valueScope, Node *node );
	Value &get_value( Node *node );
	Value &get_or_create_value( Node *node );
	void cleanup();
	void scope_push();
	void scope_pop();
};