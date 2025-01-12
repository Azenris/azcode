
#include <iostream>

#include "parser.h"

// Forward Decl
static Node *parser_parse( Parser *parser );

static Node *new_node( NodeType type, Token *token )
{
	Node *node = new Node;
	node->type = type;
	node->token = token;
	node->value = token ? token->value : nullptr;
	node->left = nullptr;
	node->right = nullptr;
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

static i32 get_operator_precedence( TokenID tokenID )
{
	switch ( tokenID )
	{
	case TokenID::Minus: return 1;
	case TokenID::Plus: return 1;
	case TokenID::Divide: return 0;
	case TokenID::Asterisk: return 0;
	}
	std::cerr << "[Parser] Unexpected operator precendence token( " << tokenID << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_operator( Parser *parser, Node *node )
{
	switch ( parser->token->id )
	{
	case TokenID::Minus:
	case TokenID::Plus:
	case TokenID::Divide:
	case TokenID::Asterisk:
		{
			Token *token = parser_consume( parser, parser->token->id );
			Node *op = new_node( NodeType::Operation, token );
			op->left = node;
			Node *right = parser_parse( parser );
			op->right = right;
			if ( op->right->type == NodeType::Operation && get_operator_precedence( right->token->id ) > get_operator_precedence( op->token->id ) )
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
	}

	return node;
}

static Node *parser_parse_keyword( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::Keyword );

	switch ( static_cast<KeywordID>( token->value.valueI32 ) )
	{
	case KeywordID::Return:
		{
			Node *node = new_node( NodeType::Return, token );
			if ( parser->token->id == TokenID::NewLine )
			{
				Node *returnNodeValue = new_node( NodeType::Number, token );
				returnNodeValue->value = 0;
				node->left = 0;
			}
			else
			{
				node->left = parser_parse( parser );
			}
			return node;
		}

	case KeywordID::False:
		{
			Node *node = new_node( NodeType::Number, token );
			node->value = 0;
			return node;
		}

	case KeywordID::True:
		{
			Node *node = new_node( NodeType::Number, token );
			node->value = 1;
			return node;
		}
	}

	std::cerr << "[Parser] Unexpected keyword token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_identifier( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::Identifier );
	Node *node = new_node( NodeType::Identifier, token );

	switch ( parser->token->id )
	{
	case TokenID::Assign:
		{
			parser_consume( parser, TokenID::Assign );
			Node *assignment = new_node( NodeType::Assignment, parser->token );
			assignment->left = node;
			assignment->right = parser_parse( parser );
			return assignment;
		}
		break;

	case TokenID::ParenOpen:
		{
			parser_consume( parser, TokenID::ParenOpen );
			Node *func = new_node( NodeType::FunctionCall, parser->token );
			func->left = node;
			if ( parser->token->id != TokenID::ParenClose )
			{
				func->children.push_back( parser_parse( parser ) );
				while ( parser->token->id == TokenID::Comma )
				{
					parser_consume( parser, TokenID::Comma );
					func->children.push_back( parser_parse( parser ) );
				}
			}
			parser_consume( parser, TokenID::ParenClose );
			return func;
		}
		break;

	case TokenID::MinusAssign:
	case TokenID::PlusAssign:
	case TokenID::DivideAssign:
	case TokenID::AsteriskAssign:
		{
			parser_consume( parser, parser->token->id );
			Node *assignment = new_node( NodeType::Assignment, parser->token );
			assignment->left = node;
			Node *op = new_node( NodeType::Operation, parser->token );
			op->left = node;
			op->right = parser_parse( parser );
			assignment->right = op;
			return assignment;
		}
		break;
	}

	node = parser_parse_operator( parser, node );

	return node;
}

static Node *parser_parse_stringliteral( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::StringLiteral );
	Node *node = new_node( NodeType::StringLiteral, token );
	return node;
}

static Node *parser_parse_number( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::Number );
	Node *node = new_node( NodeType::Number, token );
	node = parser_parse_operator( parser, node );
	return node;
}

static Node *parser_parse_minus( Parser *parser )
{
	std::cerr << "[Parser] Unexpected minus token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_minusassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected minusassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_plus( Parser *parser )
{
	std::cerr << "[Parser] Unexpected plus token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_plusassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected plusassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_divide( Parser *parser )
{
	std::cerr << "[Parser] Unexpected divide token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_divideassign( Parser *parser )
{
	std::cerr << "[Parser] Unexpected divideassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_asterisk( Parser *parser )
{
	std::cerr << "[Parser] Unexpected asterisk token( " << *parser->token << " )." << std::endl;
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

static Node *parser_parse_equal( Parser *parser )
{
	std::cerr << "[Parser] Unexpected equal token( " << *parser->token << " )." << std::endl;
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

static Node *parser_parse_bitwiseand( Parser *parser )
{
	std::cerr << "[Parser] Unexpected bitwiseand token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_logicaland( Parser *parser )
{
	std::cerr << "[Parser] Unexpected logicaland token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_bitwiseor( Parser *parser )
{
	std::cerr << "[Parser] Unexpected bitwiseor token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_logicalor( Parser *parser )
{
	std::cerr << "[Parser] Unexpected logicalor token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_bitwisenot( Parser *parser )
{
	std::cerr << "[Parser] Unexpected bitwisenot token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_logicalnot( Parser *parser )
{
	std::cerr << "[Parser] Unexpected logicalnot token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_parenopen( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::ParenOpen );
	Node *node = new_node( NodeType::Block, token );
	node->children.push_back( parser_parse( parser ) );
	while ( parser->token->id != TokenID::ParenClose )
		node->children.push_back( parser_parse( parser ) );
	parser_consume( parser, TokenID::ParenClose );
	node = parser_parse_operator( parser, node );
	return node;
}

static Node *parser_parse_parenclose( Parser *parser )
{
	std::cerr << "[Parser] Unexpected parenclose token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_braceopen( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::BraceOpen );
	Node *node = new_node( NodeType::Block, token );
	node->children.push_back( parser_parse( parser ) );
	while ( parser->token->id != TokenID::BraceClose )
	{
		parser_ignore( parser, TokenID::NewLine );
		node->children.push_back( parser_parse( parser ) );
	}
	parser_consume( parser, TokenID::BraceClose );
	return node;
}

static Node *parser_parse_braceclose( Parser *parser )
{
	std::cerr << "[Parser] Unexpected blockclose token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_period( Parser *parser )
{
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

static Node *parser_parse_semicolon( Parser *parser )
{
	std::cerr << "[Parser] Unexpected semicolon token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_newline( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::NewLine );
	Node *node = new_node( NodeType::EndStatement, token );
	return node;
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
	case TokenID::MinusAssign: return parser_parse_minusassign( parser );
	case TokenID::Plus: return parser_parse_plus( parser );
	case TokenID::PlusAssign: return parser_parse_plusassign( parser );
	case TokenID::Divide: return parser_parse_divide( parser );
	case TokenID::DivideAssign: return parser_parse_divideassign( parser );
	case TokenID::Asterisk: return parser_parse_asterisk( parser );
	case TokenID::AsteriskAssign: return parser_parse_asteriskassign( parser );
	case TokenID::Assign: return parser_parse_assign( parser );
	case TokenID::Equal: return parser_parse_equal( parser );
	case TokenID::GreaterThan: return parser_parse_greaterthan( parser );
	case TokenID::GreaterOrEqual: return parser_parse_greaterorequal( parser );
	case TokenID::LesserThan: return parser_parse_lesserthan( parser );
	case TokenID::LesserOrEqual: return parser_parse_lesserorequal( parser );
	case TokenID::BitwiseAnd: return parser_parse_bitwiseand( parser );
	case TokenID::LogicalAnd: return parser_parse_logicaland( parser );
	case TokenID::BitwiseOr: return parser_parse_bitwiseor( parser );
	case TokenID::LogicalOr: return parser_parse_logicalor( parser );
	case TokenID::BitwiseNot: return parser_parse_bitwisenot( parser );
	case TokenID::LogicalNot: return parser_parse_logicalnot( parser );
	case TokenID::ParenOpen: return parser_parse_parenopen( parser );
	case TokenID::ParenClose: return parser_parse_parenclose( parser );
	case TokenID::BraceOpen: return parser_parse_braceopen( parser );
	case TokenID::BraceClose: return parser_parse_braceclose( parser );
	case TokenID::Period: return parser_parse_period( parser );
	case TokenID::Comma: return parser_parse_comma( parser );
	case TokenID::Colon: return parser_parse_colon( parser );
	case TokenID::SemiColon: return parser_parse_semicolon( parser );
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
	token = &tokens[ tokenIndex ];
	root = new_node( NodeType::Block, nullptr );

	// std::cout << "[Tokens]" << std::endl;
	// for ( Token &t : tokens )
		// std::cout << " " << t << std::endl;
	// std::cout << std::endl;

	while ( token->id != TokenID::EndOfFile )
		root->children.push_back( parser_parse( this ) );

	root->children.pop_back();

	// -- If the program doesn't have a final return, add one --
	bool autoAddReturn = false;

	if ( root->children.empty() )
	{
		std::cerr << "[Parser] Script is empty." << std::endl;
		exit( RESULT_CODE_EMPTY_SCRIPT );
	}

	// TODO : functions will need a version of this too, but not all blocks will
	Node *lastNode = root->children.back();

	if ( lastNode->token->id != TokenID::Keyword || lastNode->value.valueString != "return" )
	{
		Node *returnNode = new_node( NodeType::Number, nullptr );
		returnNode->value = static_cast<i32>( 0 );
		root->children.push_back( returnNode );
	}
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