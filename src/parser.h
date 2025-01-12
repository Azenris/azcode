
#pragma once

#include <vector>

#include "lexer.h"
#include "result_code.h"

enum class NodeType
{
	Block,
	EndStatement,
	Identifier,
	StringLiteral,
	Number,
	Assignment,
	Operation,
	FunctionCall,
	Return,
};

struct Node
{
	NodeType type;
	Token *token;
	Node *left;
	Node *right;
	Value value;
	std::vector<Node*> children;
};

struct Parser
{
	void run( std::vector<Token> tokens );
	void cleanup();

	std::vector<Token> tokens;
	Token *token;
	i32 tokenIndex;
	Node *root;
};