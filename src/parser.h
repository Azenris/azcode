
#pragma once

#include <vector>

#include "lexer.h"
#include "result_code.h"

struct AbstractSyntaxTree
{
	
};

struct Parser
{
	AbstractSyntaxTree run( std::vector<Token> tokensIn );

	std::vector<Token> tokens;
	Token *token;
	int tokenIndex;
};