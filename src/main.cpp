
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "lexer.h"
#include "parser.h"

struct Interpreter
{
	int run()
	{
		

		return 0;
	}
};

int main( int argc, char *argv[] )
{
	if ( argc < 2 )
	{
		std::cerr << "Missing file to run." << std::endl;
		return -1;
	}

	std::string filename = argv[ 1 ];
	std::string data;

	{
		std::ifstream file( filename );

		if ( !file.is_open() )
		{
			std::cerr << "Unable to open file: " << filename << std::endl;
			return -2;
		}

		std::stringstream stream;
		stream << file.rdbuf();
		data = stream.str();
	}

	std::vector<Token> tokens = Lexer{}.run( std::move( data ) );
	AbstractSyntaxTree ast = Parser{}.run( std::move( tokens ) );

	//return Interpreter{}.run( std::move( ast ) );
	return 0;
}

// -- Unity Build --
#include "lexer.cpp"
#include "parser.cpp"