
#pragma once

#include <string>
#include <vector>
#include <print>

struct Lexer
{
	void run( std::string filename, std::string data );
	void cleanup();

	std::vector<Token> tokens;
	const char *txt;
	std::string str;
	i32 line;
	i32 file;
	std::vector<std::string> filenames;
};