
#pragma once

#include <vector>
#include <iosfwd>

#include "lexer.h"
#include "result_code.h"
#include "token.h"

struct NodeType
{
	NodeID id;
	const char *name;
};

constexpr NodeType NodeTypes[] =
{
	{
		.id = NodeID::Entry,
		.name = "Entry",
	},
	{
		.id = NodeID::Block,
		.name = "Block",
	},
	{
		.id = NodeID::Identifier,
		.name = "Identifier",
	},
	{
		.id = NodeID::CreateIdentifier,
		.name = "CreateIdentifier",
	},
	{
		.id = NodeID::StringLiteral,
		.name = "StringLiteral",
	},
	{
		.id = NodeID::Number,
		.name = "Number",
	},
	{
		.id = NodeID::CreateStruct,
		.name = "CreateStruct",
	},
	{
		.id = NodeID::CreateArray,
		.name = "CreateArray",
	},
	{
		.id = NodeID::ArrayAccess,
		.name = "ArrayAccess",
	},
	{
		.id = NodeID::Assignment,
		.name = "Assignment",
	},
	{
		.id = NodeID::Operation,
		.name = "Operation",
	},
	{
		.id = NodeID::AssignmentOp,
		.name = "AssignmentOp",
	},
	{
		.id = NodeID::Equal,
		.name = "Equal",
	},
	{
		.id = NodeID::NotEqual,
		.name = "NotEqual",
	},
	{
		.id = NodeID::DeclFunc,
		.name = "DeclFunc",
	},
	{
		.id = NodeID::FunctionArgs,
		.name = "FunctionArgs",
	},
	{
		.id = NodeID::FunctionCall,
		.name = "FunctionCall",
	},
	{
		.id = NodeID::If,
		.name = "If",
	},
	{
		.id = NodeID::Return,
		.name = "Return",
	},
	{
		.id = NodeID::Print,
		.name = "Print",
	},
	{
		.id = NodeID::Println,
		.name = "Println",
	},
	{
		.id = NodeID::Assert,
		.name = "Assert",
	},
};

std::ostream & operator << ( std::ostream &out, const NodeID &nodeID );

struct Node
{
	NodeID type;
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