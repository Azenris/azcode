
#include <iostream>

#include "parser.h"

static Token *parser_consume( Parser *parser, TokenID tokenID )
{
	if ( parser->token->id != tokenID )
	{
		std::cerr << "Unexpected token( " << *parser->token << " )." << std::endl;
		exit( -3 );
	}

	parser->token = &parser->tokens[ ++parser->tokenIndex ];

	return parser->token;
}

AbstractSyntaxTree Parser::run( std::vector<Token> tokensIn )
{
	tokens = std::move( tokensIn );
	tokenIndex = 0;
	token = &tokens[ tokenIndex ];

	for ( auto &t : tokens )
		std::cout << t << std::endl;

	AbstractSyntaxTree ast;

	

	return ast;
}