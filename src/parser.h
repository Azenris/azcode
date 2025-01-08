
#pragma once

#include <vector>

#include "lexer.h"
#include "result_code.h"

enum class NodeType
{
	Block,
	EndStatement,
	Assignment,
	StringLiteral,
	Number,
};

struct Node
{
	NodeType type;
	Token *token;
	Node *value;
	union
	{
		int64_t valueInt;
		const char *valueString;
	};
	std::vector<Node*> children;
};

struct Parser
{
	void run( std::vector<Token> tokens );

	std::vector<Token> tokens;
	Token *token;
	int tokenIndex;
	Node *root;
};