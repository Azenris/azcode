
#include <iostream>

#include "parser.h"

// Forward Decl
static Node *parser_parse( Parser *parser );
static Node *parser_parse_check_op( Parser *parser, Node *left );

static Node *new_node( NodeType type, Token *token )
{
	Node *node = new Node;
	node->type = type;
	node->token = token;
	node->value = nullptr;
	node->left = nullptr;
	node->right = nullptr;
	return node;
}

static Token *parser_consume( Parser *parser, TokenID tokenID )
{
	if ( parser->token->id != tokenID )
	{
		std::cerr << "Unexpected token( " << *parser->token << " ) wanted ( " << tokenID << " )." << std::endl;
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

static Node *parser_parse_keyword( Parser *parser )
{
	std::cerr << "Unexpected keyword token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_identifier( Parser *parser )
{
	Node *node = new_node( NodeType::Identifier, parser->token );

	parser_consume( parser, TokenID::Identifier );

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
	}

	Token *token = parser->token;

	Node *op = parser_parse_check_op( parser, node );

	if ( node == op )
	{
		std::cerr << "Unexpected identifier token( " << *token << " )." << std::endl;
		exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
	}

	return op;
}

static Node *parser_parse_stringliteral( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::StringLiteral );
	Node *node = new_node( NodeType::StringLiteral, token );
	node->value = token->value.valueString;
	return node;
}

static Node *parser_parse_number( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::Number );
	Node *node = new_node( NodeType::Number, token );
	node->value = token->value.valueI64;
	node = parser_parse_check_op( parser, node );
	return node;
}

static Node *parser_parse_check_op( Parser *parser, Node *left )
{
	Node *node = left;

	switch ( parser->token->id )
	{
	case TokenID::Minus:
	case TokenID::Plus:
	case TokenID::Divide:
	case TokenID::Asterisk:
	case TokenID::MinusAssign:
	case TokenID::PlusAssign:
	case TokenID::DivideAssign:
	case TokenID::AsteriskAssign:
		{
			Token *token = parser_consume( parser, parser->token->id );
			node = new_node( NodeType::Operation, token );
			node->left = left;
			node->right = parser_parse( parser );
		}
		break;
	}

	return node;
}

static Node *parser_parse_minus( Parser *parser )
{
	std::cerr << "Unexpected minus token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_minusassign( Parser *parser )
{
	std::cerr << "Unexpected minusassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_plus( Parser *parser )
{
	std::cerr << "Unexpected plus token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_plusassign( Parser *parser )
{
	std::cerr << "Unexpected plusassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_divide( Parser *parser )
{
	std::cerr << "Unexpected divide token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_divideassign( Parser *parser )
{
	std::cerr << "Unexpected divideassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_asterisk( Parser *parser )
{
	std::cerr << "Unexpected asterisk token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_asteriskassign( Parser *parser )
{
	std::cerr << "Unexpected asteriskassign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_assign( Parser *parser )
{
	std::cerr << "Unexpected assign token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_equal( Parser *parser )
{
	std::cerr << "Unexpected equal token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_greaterthan( Parser *parser )
{
	std::cerr << "Unexpected greaterthan token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_greaterorequal( Parser *parser )
{
	std::cerr << "Unexpected greaterorequal token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_lesserthan( Parser *parser )
{
	std::cerr << "Unexpected lesserthan token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_lesserorequal( Parser *parser )
{
	std::cerr << "Unexpected lesserorequal token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_bitwiseand( Parser *parser )
{
	std::cerr << "Unexpected bitwiseand token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_logicaland( Parser *parser )
{
	std::cerr << "Unexpected logicaland token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_bitwiseor( Parser *parser )
{
	std::cerr << "Unexpected bitwiseor token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_logicalor( Parser *parser )
{
	std::cerr << "Unexpected logicalor token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_bitwisenot( Parser *parser )
{
	std::cerr << "Unexpected bitwisenot token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_logicalnot( Parser *parser )
{
	std::cerr << "Unexpected logicalnot token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_parenopen( Parser *parser )
{
	std::cerr << "Unexpected parenopen token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_parenclose( Parser *parser )
{
	std::cerr << "Unexpected parenclose token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_braceopen( Parser *parser )
{
	parser_consume( parser, TokenID::BraceOpen );
	Node *node = new_node( NodeType::Block, parser->token );
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
	std::cerr << "Unexpected blockclose token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_period( Parser *parser )
{
	std::cerr << "Unexpected period token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_comma( Parser *parser )
{
	std::cerr << "Unexpected comma token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_colon( Parser *parser )
{
	std::cerr << "Unexpected colon token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_semicolon( Parser *parser )
{
	std::cerr << "Unexpected semicolon token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_newline( Parser *parser )
{
	Node *node = new_node( NodeType::EndStatement, parser->token );
	return node;
}

static Node *parser_parse_endoffile( Parser *parser )
{
	std::cerr << "Unexpected endoffile token( " << *parser->token << " )." << std::endl;
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

	std::cerr << "Unexpected parse token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

void Parser::run( std::vector<Token> tokensIn )
{
	tokens = std::move( tokensIn );
	tokenIndex = 0;
	token = &tokens[ tokenIndex ];
	root = new_node( NodeType::Block, token );

	std::cout << "[Tokens]" << std::endl;
	for ( Token &t : tokens )
		std::cout << " " << t << std::endl;
	std::cout << std::endl;

	while ( token->id != TokenID::EndOfFile )
		root->children.push_back( parser_parse( this ) );
}