
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "lexer.h"
#include "parser.h"
#include "result_code.h"

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
		return RESULT_CODE_NO_FILE_INPUT_TO_PROCESS;
	}

	std::string filename = argv[ 1 ];
	std::string data;

	{
		std::ifstream file( filename );

		if ( !file.is_open() )
		{
			std::cerr << "Unable to open file: " << filename << std::endl;
			return RESULT_CODE_FAILED_TO_OPEN_INPUT_FILE;
		}

		std::stringstream stream;
		stream << file.rdbuf();
		data = stream.str();
	}

	Parser parser;
	parser.run( Lexer{}.run( std::move( data ) ) );

	//return Interpreter{}.run( &parser );

	return RESULT_CODE_SUCCESS;
}

// -- Unity Build --
#include "lexer.cpp"
#include "parser.cpp"