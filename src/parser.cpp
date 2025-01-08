
#include <iostream>
#include <list>

#include "parser.h"

// Forward Decl
static Node *parser_parse( Parser *parser );

static inline Node *new_node( NodeType type )
{
	Node *node = new Node;
	node->type = type;
	node->value = nullptr;
	return node;
}

static Token *parser_consume( Parser *parser, TokenID tokenID )
{
	if ( parser->token->id != tokenID )
	{
		std::cerr << "Unexpected token( " << *parser->token << " )." << std::endl;
		exit( RESULT_CODE_UNEXPECTED_TOKEN );
	}
	Token *token = &parser->tokens[ parser->tokenIndex++ ];
	parser->token = &parser->tokens[ parser->tokenIndex ];
	return token;
}

static Node *parser_parse_keyword( Parser *parser )
{
	

	std::cerr << "Unexpected keyword token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_identifier( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::Identifier );

	switch ( parser->token->id )
	{
	case TokenID::Assign:
		{
			parser_consume( parser, TokenID::Assign );
			Node *node = new_node( NodeType::Assignment );
			node->token = token;
			node->value = parser_parse( parser );
			return node;
		}
		break;
	}

	std::cerr << "Unexpected identifier token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_stringliteral( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::StringLiteral );
	Node *node = new_node( NodeType::StringLiteral );
	node->token = token;
	node->valueString = token->valueString.c_str();
	return node;
}

static Node *parser_parse_number( Parser *parser )
{
	Token *token = parser_consume( parser, TokenID::Number );
	Node *node = new_node( NodeType::Number );
	node->token = token;
	node->valueInt = token->valueNumber;
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
	parser_consume( parser, TokenID::ParenOpen );
	Node *node = new_node( NodeType::Block );
	node->children.push_back( parser_parse( parser ) );
	while ( parser->token->id == TokenID::Comma )
	{
		parser_consume( parser, TokenID::Comma );
		node->children.push_back( parser_parse( parser ) );
	}
	parser_consume( parser, TokenID::ParenClose );
	return node;
}

static Node *parser_parse_parenclose( Parser *parser )
{
	std::cerr << "Unexpected parenclose token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_blockopen( Parser *parser )
{
	

	std::cerr << "Unexpected blockopen token( " << *parser->token << " )." << std::endl;
	exit( RESULT_CODE_UNHANDLED_TOKEN_PARSING );
}

static Node *parser_parse_blockclose( Parser *parser )
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
	Node *node = new_node( NodeType::EndStatement );
	node->token = parser_consume( parser, TokenID::NewLine );
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
	case TokenID::BlockOpen: return parser_parse_blockopen( parser );
	case TokenID::BlockClose: return parser_parse_blockclose( parser );
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
	root = new_node( NodeType::Block );

	std::cout << "[Tokens]" << std::endl;
	for ( auto &t : tokens )
		std::cout << " " << t << std::endl;
	std::cout << std::endl;

	while ( token->id != TokenID::EndOfFile )
		root->children.push_back( parser_parse( this ) );
}