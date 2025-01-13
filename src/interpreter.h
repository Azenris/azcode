
#pragma once

#include <vector>
#include <unordered_map>

#include "parser.h"

struct Interpreter
{
	std::vector<std::unordered_map<std::string, Value>> data;

	Value run( Node *node );
	Value &get_value( Node *node );
	Value &get_or_create_value( Node *node );
	void cleanup();
};