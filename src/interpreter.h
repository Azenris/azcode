
#pragma once

#include "parser.h"

struct Interpreter
{
	Value run( Node *node );

	Value process_node( Node *node );
	Value &get_or_create_variable( Node *node );
	Value *get_variable( Node *node );
};