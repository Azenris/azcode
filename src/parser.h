
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
		.id = NodeID::Import,
		.name = "Import",
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
	{
		.id = NodeID::ForNumberRange,
		.name = "ForNumberRange",
	},
	{
		.id = NodeID::ForOfIdentifier,
		.name = "ForOfIdentifier",
	},
	{
		.id = NodeID::ForOfIdentifierRange,
		.name = "ForOfIdentifierRange",
	},
	{
		.id = NodeID::ForOfIdentifierRangeCount,
		.name = "ForOfIdentifierRangeCount",
	},
	{
		.id = NodeID::While,
		.name = "While",
	},
	{
		.id = NodeID::Continue,
		.name = "Continue",
	},
	{
		.id = NodeID::Break,
		.name = "Break",
	},
	{
		.id = NodeID::Exit,
		.name = "Exit",
	},
};

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

struct Arg
{
	Node **args;

	inline Arg( Node **children )
		: args( children )
	{
	}

	inline Node *next()
	{
		Node *ret = *args;
		args++;
		return ret;
	}

	inline operator Node * () const
	{
		return *args;
	}
};

// --------------------------------------------------------------------

template <>
struct std::formatter<NodeID>
{
	constexpr auto parse( std::format_parse_context &ctx )
	{
		return ctx.begin();
	}

	std::format_context::iterator format( const NodeID &nodeID, std::format_context &ctx ) const
	{
		return std::format_to( ctx.out(), "NodeType: {}", NodeTypes[ static_cast<i32>( nodeID ) ].name );
	}
};