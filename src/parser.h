
#pragma once

#include <vector>

#include "lexer.h"
#include "result_code.h"
#include "token.h"

enum class NodeType
{
	Entry,
	Block,
	Identifier,
	StringLiteral,
	Number,
	Assignment,
	AssignmentOp,
	Operation,
	Equal,
	NotEqual,
	DeclFunc,
	FunctionArgs,
	FunctionCall,
	If,
	Return,
	Print,
	Println,
};

struct Node
{
	NodeType type;
	Token *token;
	Node *left;
	Node *right;
	Value value;
	std::vector<Node*> children;
	i32 scope;
};

struct Parser
{
	void run( std::vector<Token> tokens );

	void cleanup();

	std::vector<Token> tokens;
	Token *token;
	i32 tokenIndex;
	i32 scope;
	Node *root;
};