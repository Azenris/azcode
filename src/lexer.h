
#pragma once

#include <string>
#include <vector>

struct Lexer
{
	void run( std::string data );

	std::vector<Token> tokens;
	const char *txt;
	std::string str;
};