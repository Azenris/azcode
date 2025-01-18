
#include <iostream>

#include "parser.h"

std::ostream & operator << ( std::ostream &out, const NodeID &nodeID )
{
	return out << "NodeType: " << NodeTypes[ static_cast<i32>( nodeID ) ].name;
}

// Forward Decl
static Node *parser_parse( Parser *parser );
static Node *parser_parse_identifier_assign( Parser *parser );
static Node *parser_parse_identifier( Parser *parser, Node **identiferNode = nullptr );

static Node *new_node( Parser *parser, NodeID type, Token *token )
{
	Node *node = new Node;
	node->type = type;
	node->token = token;
	node->value = token ? token->value : nullptr;
	node->left = nullptr;
	node->right = nullptr;
	node->scope = parser->scope;
	return node;
}

static Token *parser_consume( Parser *parser, TokenID tokenID )
{
	if ( parser->token->id != tokenID )
	{
		std::cerr << "[Parser] Unexpected token( " << *parser->token << " ) wanted ( " << tokenID << " )." << std::endl;
		exit( RESULT_CODE_UNEXPECTED_TOKEN );
	}
	Token *token = &parser->tokens[ parser->tokenIndex++ ];
	parser->token = &parser->tokens[ parser->tokenIndex ];
	return token;
}

static Token *parser_ignore( Parser *parser, TokenID tokenID )
{
	while ( parser->token->id == tokenID )
		parser->token = &parser->tokens[ ++parser->tokenIndex ];
	return parser->token;
}

static void add_return_if_needed( Parser *parser, Node *node )
{
	if ( node->children.empty() || node->children.back()->token->id != TokenID::Keyword || node->children.back()->value.keywordID != KeywordID::Return )
	{
		// Implicit return will return the value 0
		Node *returnNode = new_node( parser, NodeID::Return, nullptr );
		returnNode->value = static_cast<i32>( 0 );
		node->children.push_back( returnNode );
	}
}

static i32 get_operator_precedence( TokenID tokenID )
{
	switch ( tokenID )
	{
	case TokenID::Minus: return 1;
	case TokenID::Plus: return 1;
	case TokenID::Divide: return 0;
	case TokenID::Asterisk: return 0;
	case TokenID::Amp: return 2;
	case TokenID::Pipe: return 4;
	case TokenID::Hat: return 3;
	case TokenID::Percent: return 0;
	}
	std::cerr << "[Parser] Unexpected operator precendence token( " << tokenID << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

// precendence
// 5 * 1 + 1
//
// evals as 10
// 5 * ( 1 + 1 )
// -------------
//    |
//   op*
//  /   \
// 5     op+
//      /   \
//     1     1
// -------------
// but wanted:
// evals as 6
// (5 * 1 ) + 1
//
//       |
//      op+
//     /   \
//   op*    1
//  /   \
// 5     1

static Node *parser_parse_operator( Parser *parser, Node *node )
{
	switch ( parser->token->id )
	{
	case TokenID::Minus:
	case TokenID::Plus:
	case TokenID::Divide:
	case TokenID::Asterisk:
	case TokenID::Amp:
	case TokenID::Pipe:
	case TokenID::Hat:
	case TokenID::Percent:
		{
			Token *token = parser_consume( parser, parser->token->id );
			Node *op = new_node( parser, NodeID::Operation, token );
			op->left = node;
			Node *right = parser_parse( parser );
			op->right = right;
			if ( op->right->type == NodeID::Operation && get_operator_precedence( right->token->id ) > get_operator_precedence( op->token->id ) )
			{
				op->right = right->left;
				right->left = op;
				node = right;
			}
			else
			{
				node = op;
			}
		}
		break;

	case TokenID::DoubleAssign:
		{
			Token *token = parser_consume( parser, TokenID::DoubleAssign );
			Node *equal = new_node( parser, NodeID::Equal, token );
			equal->left = node;
			equal->right = parser_parse( parser );
			return equal;
		}
		break;

	case TokenID::ExclamationAssign:
		{
			Token *token = parser_consume( parser, TokenID::ExclamationAssign );
			Node *notEqual = new_node( parser, NodeID::NotEqual, token );
			notEqual->left = node;
			notEqual->right = parser_parse( parser );
			return notEqual;
		}
		break;
	}

	return node;
}

static Node *parser_parse_func_decl_args( Parser *parser )
{
	parser->scope += 1;

	Node *args = new_node( parser, NodeID::FunctionArgs, nullptr );

	parser_ignore( parser, TokenID::NewLine );
	parser_consume( parser, TokenID::ParenOpen );
	parser_ignore( parser, TokenID::NewLine );

	if ( parser->token->id != TokenID::ParenClose )
	{
		args->children.push_back( parser_parse( parser ) );
		parser_ignore( parser, TokenID::NewLine );

		while ( parser->token->id == TokenID::Comma )
		{
			parser_consume( parser, TokenID::Comma );
			parser_ignore( parser, TokenID::NewLine );
			if ( parser->token->id == TokenID::ParenClose )
				break;
			args->children.push_back( parser_parse( parser ) );
			parser_ignore( parser, TokenID::NewLine );
		}
	}

	parser_consume( parser, TokenID::ParenClose );

	if ( args->children.empty() )
	{
		delete args;
		args = nullptr;
	}

	parser->scope -= 1;

	return args;
}

static void parser_parse_func_args( Parser *parser, Node *node )
{
	parser_ignore( parser, TokenID::NewLine );
	parser_consume( parser, TokenID::ParenOpen );
	parser_ignore( parser, TokenID::NewLine );

	if ( parser->token->id != TokenID::ParenClose )
	{
		node->children.push_back( parser_parse( parser ) );
		parser_ignore( parser, TokenID::NewLine );

		while ( parser->token->id == TokenID::Comma )
		{
			parser_consume( parser, TokenID::Comma );
			parser_ignore( parser, TokenID::NewLine );
			node->children.push_back( parser_parse( parser ) );
			parser_ignore( parser, TokenID::NewLine );
		}
	}

	parser_consume( parser, TokenID::ParenClose );
}

static void parser_parse_codeblock( Parser *parser, Node *node )
{
	parser->scope += 1;

	parser_ignore( parser, TokenID::NewLine );
	parser_consume( parser, TokenID::BraceOpen );
	parser_ignore( parser, TokenID::NewLine );

	while ( parser->token->id != TokenID::BraceClose )
	{
		node->children.push_back( parser_parse( parser ) );
		parser_ignore( parser, TokenID::NewLine );
	}

	parser_consume( parser, TokenID::BraceClose );

	parser->scope -= 1;
}

static Node *parser_parse_keyword( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::Keyword );

	switch ( token->value.keywordID )
	{
	case KeywordID::Return:
		{
			Node *node = new_node( parser, NodeID::Return, token );
			if ( parser->token->id == TokenID::NewLine )
			{
				Node *returnNodeValue = new_node( parser, NodeID::Number, nullptr );
				returnNodeValue->value = 0;
				node->left = returnNodeValue;
			}
			else
			{
				node->left = parser_parse( parser );
			}
			return node;
		}

	case KeywordID::False:
		{
			Node *node = new_node( parser, NodeID::Number, token );
			node->value = 0;
			return node;
		}

	case KeywordID::True:
		{
			Node *node = new_node( parser, NodeID::Number, token );
			node->value = 1;
			return node;
		}

	case KeywordID::If:
		{
			Node *node = new_node( parser, NodeID::If, token );
			parser_consume( parser, TokenID::ParenOpen );
			Node *top = node;
			while ( node )
			{
				// condition
				node->left = parser_parse( parser );
				parser_ignore( parser, TokenID::NewLine );
				parser_consume( parser, TokenID::ParenClose );
				// if
				parser_parse_codeblock( parser, node );
				parser_ignore( parser, TokenID::NewLine );
				if ( parser->token->id == TokenID::Keyword && parser->token->value.keywordID == KeywordID::Else )
				{
					parser_consume( parser, TokenID::Keyword );
					if ( parser->token->id == TokenID::Keyword && parser->token->value.keywordID == KeywordID::If )
					{
						// else if
						token = parser_consume( parser, TokenID::Keyword );
						node->right = new_node( parser, NodeID::If, token );
						parser_consume( parser, TokenID::ParenOpen );
						node = node->right;
					}
					else
					{
						// else
						node->right = new_node( parser, NodeID::Block, nullptr );
						parser_parse_codeblock( parser, node->right );
						node = nullptr;
					}
				}
			}
			return top;
		}

	case KeywordID::Print:
		{
			Node *node = new_node( parser, NodeID::Print, token );
			if ( parser->token->id != TokenID::NewLine )
			{
				bool paren = ( parser->token->id == TokenID::ParenOpen );
				if ( paren )
					parser_consume( parser, TokenID::ParenOpen );
				parser_ignore( parser, TokenID::NewLine );
				node->left = parser_parse( parser );
				while ( parser->token->id == TokenID::Comma )
				{
					parser_consume( parser, TokenID::Comma );
					node->children.push_back( parser_parse( parser ) );
					parser_ignore( parser, TokenID::NewLine );
				}
				parser_ignore( parser, TokenID::NewLine );
				if ( paren )
					parser_consume( parser, TokenID::ParenClose );
				return node;
			}
		}
		break;

	case KeywordID::Println:
		{
			Node *node = new_node( parser, NodeID::Println, token );
			if ( parser->token->id != TokenID::NewLine )
			{
				bool paren = ( parser->token->id == TokenID::ParenOpen );
				if ( paren )
					parser_consume( parser, TokenID::ParenOpen );
				parser_ignore( parser, TokenID::NewLine );
				node->left = parser_parse( parser );
				while ( parser->token->id == TokenID::Comma )
				{
					parser_consume( parser, TokenID::Comma );
					node->children.push_back( parser_parse( parser ) );
					parser_ignore( parser, TokenID::NewLine );
				}
				parser_ignore( parser, TokenID::NewLine );
				if ( paren )
					parser_consume( parser, TokenID::ParenClose );
				return node;
			}
			else
			{
				parser_consume( parser, TokenID::NewLine );
				return node;
			}
		}
		break;

	case KeywordID::Assert:
		{
			Node *node = new_node( parser, NodeID::Assert, token );
			parser_ignore( parser, TokenID::NewLine );
			node->left = parser_parse( parser );
			parser_ignore( parser, TokenID::NewLine );
			if ( parser->token->id == TokenID::Comma )
			{
				parser_consume( parser, TokenID::Comma );
				node->right = parser_parse( parser );
				while ( parser->token->id == TokenID::Comma )
				{
					parser_consume( parser, TokenID::Comma );
					node->children.push_back( parser_parse( parser ) );
				}
			}
			return node;
		}
		break;

	case KeywordID::For:
		{
			Node *node = new_node( parser, NodeID::ForNumberRange, token );
			parser_consume( parser, TokenID::ParenOpen );
			node->left = parser_parse_identifier( parser );
			parser_consume( parser, TokenID::Colon );

			Node *first = parser_parse( parser );

			if ( parser->token->id == TokenID::DoublePeriod )
			{
				parser_consume( parser, TokenID::DoublePeriod );
				node->type = NodeID::ForNumberRange;
				node->right = first;
				first->right = parser_parse( parser );
			}
			else
			{
				node->type = NodeID::ForOfIdentifier;
				node->right = first;
			}

			parser_consume( parser, TokenID::ParenClose );
			parser_parse_codeblock( parser, node );
			return node;
		}
		break;

	case KeywordID::While:
		{
			// TODO fff
		}
		break;
	}

	std::cerr << "[Parser] Unexpected keyword token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_struct_decl( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::BraceOpen );
	Node *node = new_node( parser, NodeID::CreateStruct, token );

	parser_ignore( parser, TokenID::NewLine );

	if ( parser->token->id != TokenID::BraceClose )
	{
		node->children.push_back( parser_parse_identifier_assign( parser ) );
		parser_ignore( parser, TokenID::NewLine );

		while ( parser->token->id == TokenID::Comma )
		{
			parser_consume( parser, TokenID::Comma );
			parser_ignore( parser, TokenID::NewLine );
			if ( parser->token->id == TokenID::BraceClose )
				break;
			node->children.push_back( parser_parse_identifier_assign( parser ) );
			parser_ignore( parser, TokenID::NewLine );
		}
	}

	parser_consume( parser, TokenID::BraceClose );

	return node;
}

static Node *parser_parse_identifier_assign( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::Identifier );
	Node *node = new_node( parser, NodeID::Identifier, token );

	switch ( parser->token->id )
	{
	case TokenID::Assign:
		{
			token = parser_consume( parser, TokenID::Assign );
			Node *assignment = new_node( parser, NodeID::Assignment, token );
			parser_ignore( parser, TokenID::NewLine );
			assignment->left = node;
			if ( parser->token->id == TokenID::BraceOpen )
				assignment->right = parser_parse_struct_decl( parser );
			else
				assignment->right = parser_parse( parser );
			return assignment;
		}
		break;

	case TokenID::ColonAssign:
		{
			token = parser_consume( parser, TokenID::ColonAssign );
			Node *declFunc = new_node( parser, NodeID::DeclFunc, token );
			declFunc->left = node;
			declFunc->right = parser_parse_func_decl_args( parser );
			parser_parse_codeblock( parser, declFunc );
			add_return_if_needed( parser, declFunc );
			return declFunc;
		}
		break;
	}

	std::cerr << "[Parser] Assign expected. Instead of token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parse_parse_dot_access( Parser *parser, Node *node )
{
	Token *token;
	Node *idNode = node;

	while ( parser->token->id == TokenID::Period )
	{
		parser_consume( parser, TokenID::Period );
		parser_ignore( parser, TokenID::NewLine );

		if ( parser->token->id == TokenID::Identifier )
		{
			token = parser_consume( parser, TokenID::Identifier );

			switch ( idNode->type )
			{
			case NodeID::Identifier:
			case NodeID::CreateIdentifier:
				idNode->children.push_back( new_node( parser, NodeID::Identifier, token ) );
				break;

			default:
				node = idNode;
				idNode = new_node( parser, NodeID::Identifier, token );
				idNode->left = node;
			}
		}
		else
		{
			std::cerr << "[Parser] Unexpected dot keyword token( " << *parser->token << " )." << std::endl;
			exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
		}
	}

	return idNode;
}

static Node *parser_parse_identifier_use( Parser *parser, Node *node )
{
	node = parse_parse_dot_access( parser, node );

	switch ( parser->token->id )
	{
	case TokenID::Assign:
		{
			if ( node->type == NodeID::Identifier )
				node->type = NodeID::CreateIdentifier;
			Token *token = parser_consume( parser, TokenID::Assign );
			Node *assignment = new_node( parser, NodeID::Assignment, token );
			parser_ignore( parser, TokenID::NewLine );
			assignment->left = node;
			if ( parser->token->id == TokenID::BraceOpen )
				assignment->right = parser_parse_struct_decl( parser );
			else
				assignment->right = parser_parse( parser );
			return assignment;
		}
		break;

	case TokenID::ColonAssign:
		{
			if ( node->type == NodeID::Identifier )
				node->type = NodeID::CreateIdentifier;
			Token *token = parser_consume( parser, TokenID::ColonAssign );
			Node *declFunc = new_node( parser, NodeID::DeclFunc, token );
			declFunc->left = node;
			declFunc->right = parser_parse_func_decl_args( parser );
			parser_parse_codeblock( parser, declFunc );
			add_return_if_needed( parser, declFunc );
			return declFunc;
		}
		break;

	case TokenID::ParenOpen:
		{
			Node *func = new_node( parser, NodeID::FunctionCall, parser->token );
			func->left = node;
			parser_parse_func_args( parser, func );
			return parser_parse_identifier_use( parser, func );
		}
		break;

	case TokenID::SquareOpen:
		{
			parser_consume( parser, TokenID::SquareOpen );
			parser_ignore( parser, TokenID::NewLine );
			Node *accessor = new_node( parser, NodeID::ArrayAccess, parser->token );
			accessor->left = node;
			accessor->right = parser_parse( parser );
			parser_ignore( parser, TokenID::NewLine );
			parser_consume( parser, TokenID::SquareClose );
			return parser_parse_identifier_use( parser, accessor );
		}
		break;

	case TokenID::MinusAssign:
	case TokenID::PlusAssign:
	case TokenID::DivideAssign:
	case TokenID::AsteriskAssign:
	case TokenID::AmpAssign:
	case TokenID::PipeAssign:
	case TokenID::HatAssign:
	case TokenID::PercentAssign:
		{
			Token *token = parser_consume( parser, parser->token->id );
			Node *op = new_node( parser, NodeID::AssignmentOp, token );
			op->left = node;
			op->right = parser_parse( parser );
			node = op;
		}
		break;
	}

	node = parser_parse_operator( parser, node );

	return node;
}

static Node *parser_parse_identifier( Parser *parser, Node **identiferNode )
{
	Token *token = parser_consume( parser, TokenID::Identifier );
	Node *node = new_node( parser, NodeID::Identifier, token );

	if ( identiferNode )
		*identiferNode = node;

	parser_ignore( parser, TokenID::NewLine );

	return parser_parse_identifier_use( parser, node );
}

static Node *parser_parse_stringliteral( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::StringLiteral );
	Node *node = new_node( parser, NodeID::StringLiteral, token );
	return node;
}

static Node *parser_parse_number( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::Number );
	Node *node = new_node( parser, NodeID::Number, token );
	node = parser_parse_operator( parser, node );
	return node;
}

static Node *parser_parse_minus( Parser *parser )
{
	std::cerr << "[Parser] Unexpected minus token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_plus( Parser *parser )
{
	std::cerr << "[Parser] Unexpected plus token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_divide( Parser *parser )
{
	std::cerr << "[Parser] Unexpected divide token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_asterisk( Parser *parser )
{
	std::cerr << "[Parser] Unexpected asterisk token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_tilde( Parser *parser )
{
	std::cerr << "[Parser] Unexpected tilde( token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_amp( Parser *parser )
{
	std::cerr << "[Parser] Unexpected amp token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_pipe( Parser *parser )
{
	std::cerr << "[Parser] Unexpected pipe token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_hat( Parser *parser )
{
	std::cerr << "[Parser] Unexpected hat token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_percent( Parser *parser )
{
	std::cerr << "[Parser] Unexpected percent token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_exclamation( Parser *parser )
{
	std::cerr << "[Parser] Unexpected exclamation token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_tildeassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected tildeassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_ampassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected ampassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_pipeassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected pipeassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_hatassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected hatassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_percentassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected percentassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_exclamationassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected exclamationassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_doubleamp( Parser *parser )
{
	std::cerr << "[Parser] Unexpected doubleamp token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_doublepipe( Parser *parser )
{
	std::cerr << "[Parser] Unexpected doublepipe token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_minusassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected minusassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_plusassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected plusassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_divideassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected divideassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_asteriskassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected asteriskassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_assign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected assign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_doubleequal( Parser *parser )
{
	std::cerr << "[Parser] Unexpected doubleequal token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_greaterthan( Parser *parser )
{
	std::cerr << "[Parser] Unexpected greaterthan token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_greaterorequal( Parser *parser )
{
	std::cerr << "[Parser] Unexpected greaterorequal token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_lesserthan( Parser *parser )
{
	std::cerr << "[Parser] Unexpected lesserthan token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_lesserorequal( Parser *parser )
{
	std::cerr << "[Parser] Unexpected lesserorequal token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_parenopen( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::ParenOpen );
	Node *node = new_node( parser, NodeID::Block, token );

	parser_ignore( parser, TokenID::NewLine );

	while ( parser->token->id != TokenID::ParenClose )
	{
		node->children.push_back( parser_parse( parser ) );
		parser_ignore( parser, TokenID::NewLine );
	}

	parser_consume( parser, TokenID::ParenClose );
	parser_ignore( parser, TokenID::NewLine );

	return parser_parse_operator( parser, node );
}

static Node *parser_parse_parenclose( Parser *parser )
{
	std::cerr << "[Parser] Unexpected parenclose token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_braceopen( Parser *parser )
{
	parser->scope += 1;

	Token *token = parser_consume( parser, TokenID::BraceOpen );
	Node *node = new_node( parser, NodeID::Block, token );

	parser_ignore( parser, TokenID::NewLine );

	while ( parser->token->id != TokenID::BraceClose )
	{
		node->children.push_back( parser_parse( parser ) );
		parser_ignore( parser, TokenID::NewLine );
	}

	parser_consume( parser, TokenID::BraceClose );

	parser->scope -= 1;

	return node;
}

static Node *parser_parse_braceclose( Parser *parser )
{
	std::cerr << "[Parser] Unexpected blockclose token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_squareopen( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::SquareOpen );
	Node *node = new_node( parser, NodeID::CreateArray, token );

	parser_ignore( parser, TokenID::NewLine );

	if ( parser->token->id != TokenID::SquareClose )
	{
		node->children.push_back( parser_parse( parser ) );
		parser_ignore( parser, TokenID::NewLine );

		while ( parser->token->id == TokenID::Comma )
		{
			parser_consume( parser, TokenID::Comma );
			parser_ignore( parser, TokenID::NewLine );
			if ( parser->token->id == TokenID::SquareClose )
				break;
			node->children.push_back( parser_parse( parser ) );
			parser_ignore( parser, TokenID::NewLine );
		}
	}

	parser_consume( parser, TokenID::SquareClose );

	return node;
}

static Node *parser_parse_squareclose( Parser *parser )
{
	std::cerr << "[Parser] Unexpected squareclose token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_period( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::Period );
	if ( parser->token->id == TokenID::Identifier )
	{
		Node *identiferNode;
		Node *node = parser_parse_identifier( parser, &identiferNode );
		identiferNode->scope = -1;
		return node;
	}
	std::cerr << "[Parser] Unexpected period token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_comma( Parser *parser )
{
	std::cerr << "[Parser] Unexpected comma token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_colon( Parser *parser )
{
	std::cerr << "[Parser] Unexpected colon token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_colonassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected colonassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_semicolon( Parser *parser )
{
	std::cerr << "[Parser] Unexpected semicolon token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_newline( Parser *parser )
{
	parser_consume( parser, TokenID::NewLine );
	return nullptr;
}

static Node *parser_parse_endoffile( Parser *parser )
{
	std::cerr << "[Parser] Unexpected endoffile token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse( Parser *parser )
{
	switch ( parser->token->id )
	{
	case TokenID::Keyword: return parser_parse_keyword( parser );
	case TokenID::Identifier: return parser_parse_identifier( parser );
	case TokenID::StringLiteral: return parser_parse_stringliteral( parser );
	case TokenID::Number: return parser_parse_number( parser );
	case TokenID::Minus: return parser_parse_minus( parser );
	case TokenID::Plus: return parser_parse_plus( parser );
	case TokenID::Divide: return parser_parse_divide( parser );
	case TokenID::Asterisk: return parser_parse_asterisk( parser );
	case TokenID::Tilde: return parser_parse_tilde( parser );
	case TokenID::Amp: return parser_parse_amp( parser );
	case TokenID::Pipe: return parser_parse_pipe( parser );
	case TokenID::Hat: return parser_parse_hat( parser );
	case TokenID::Percent: return parser_parse_percent( parser );
	case TokenID::Exclamation: return parser_parse_exclamation( parser );
	case TokenID::MinusAssign: return parser_parse_minusassign( parser );
	case TokenID::PlusAssign: return parser_parse_plusassign( parser );
	case TokenID::DivideAssign: return parser_parse_divideassign( parser );
	case TokenID::AsteriskAssign: return parser_parse_asteriskassign( parser );
	case TokenID::TildeAssign: return parser_parse_tildeassign( parser );
	case TokenID::AmpAssign: return parser_parse_ampassign( parser );
	case TokenID::PipeAssign: return parser_parse_pipeassign( parser );
	case TokenID::HatAssign: return parser_parse_hatassign( parser );
	case TokenID::PercentAssign: return parser_parse_percentassign( parser );
	case TokenID::ExclamationAssign: return parser_parse_exclamationassign( parser );
	case TokenID::DoubleAmp: return parser_parse_doubleamp( parser );
	case TokenID::DoublePipe: return parser_parse_doublepipe( parser );
	case TokenID::Assign: return parser_parse_assign( parser );
	case TokenID::DoubleAssign: return parser_parse_doubleequal( parser );
	case TokenID::GreaterThan: return parser_parse_greaterthan( parser );
	case TokenID::GreaterOrEqual: return parser_parse_greaterorequal( parser );
	case TokenID::LesserThan: return parser_parse_lesserthan( parser );
	case TokenID::LesserOrEqual: return parser_parse_lesserorequal( parser );
	case TokenID::ParenOpen: return parser_parse_parenopen( parser );
	case TokenID::ParenClose: return parser_parse_parenclose( parser );
	case TokenID::BraceOpen: return parser_parse_braceopen( parser );
	case TokenID::BraceClose: return parser_parse_braceclose( parser );
	case TokenID::SquareOpen: return parser_parse_squareopen( parser );
	case TokenID::SquareClose: return parser_parse_squareclose( parser );
	case TokenID::Period: return parser_parse_period( parser );
	case TokenID::Comma: return parser_parse_comma( parser );
	case TokenID::Colon: return parser_parse_colon( parser );
	case TokenID::ColonAssign: return parser_parse_colonassign( parser );
	case TokenID::SemiColon: return parser_parse_semicolon( parser );
	case TokenID::NewLine: return parser_parse_newline( parser );
	case TokenID::EndOfFile: return parser_parse_endoffile( parser );
	}

	std::cerr << "[Parser] Unexpected parse token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_top( Parser *parser )
{
	switch ( parser->token->id )
	{
	case TokenID::Keyword: return parser_parse_keyword( parser );
	case TokenID::Identifier: return parser_parse_identifier( parser );
	case TokenID::Minus: return parser_parse_minus( parser );
	case TokenID::Plus: return parser_parse_plus( parser );
	case TokenID::Divide: return parser_parse_divide( parser );
	case TokenID::Asterisk: return parser_parse_asterisk( parser );
	case TokenID::Tilde: return parser_parse_tilde( parser );
	case TokenID::Amp: return parser_parse_amp( parser );
	case TokenID::Pipe: return parser_parse_pipe( parser );
	case TokenID::Hat: return parser_parse_hat( parser );
	case TokenID::Percent: return parser_parse_percent( parser );
	case TokenID::Exclamation: return parser_parse_exclamation( parser );
	case TokenID::BraceOpen: return parser_parse_braceopen( parser );
	case TokenID::BraceClose: return parser_parse_braceclose( parser );
	case TokenID::NewLine: return parser_parse_newline( parser );
	case TokenID::EndOfFile: return parser_parse_endoffile( parser );
	}

	std::cerr << "[Parser] Unexpected parse token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

void Parser::run( std::vector<Token> tokensIn )
{
	tokens = std::move( tokensIn );
	tokenIndex = 0;
	scope = 0;
	token = &tokens[ tokenIndex ];
	root = new_node( this, NodeID::Entry, nullptr );

	// std::cout << "[Tokens]" << std::endl;
	// for ( Token &t : tokens )
		// std::cout << " " << t << std::endl;
	// std::cout << std::endl;

	Node *node;
	while ( token->id != TokenID::EndOfFile )
	{
		node = parser_parse_top( this );
		if ( node )
			root->children.push_back( node );
	}

	if ( root->children.empty() )
	{
		std::cerr << "[Parser] Script is empty." << std::endl;
		exit( RESULT_CODE_EMPTY_SCRIPT );
	}

	add_return_if_needed( this, root );
}

void Parser::cleanup()
{
	tokens.clear();
	token = nullptr;
	tokenIndex = 0;

	Node *node = root;
	std::vector<Node*> nodes;
	while ( node )
	{
		if ( node->left )
			nodes.push_back( node->left );
		if ( node->right )
			nodes.push_back( node->right );
		for ( auto child : node->children )
			nodes.push_back( child );
		delete node;
		if ( !nodes.empty() )
		{
			node = nodes.back();
			nodes.pop_back();
		}
		else
		{
			node = nullptr;
		}
	}
}