
#pragma once

#include <unordered_map>

#include "parser.h"

struct Interpreter
{
	std::unordered_map<std::string, Value> data;

	Value run( Node *node );

	void cleanup();
};