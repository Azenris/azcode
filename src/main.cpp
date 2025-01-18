
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;

static_assert( sizeof( i8 ) == 1 );
static_assert( sizeof( i16 ) == 2 );
static_assert( sizeof( i32 ) == 4 );
static_assert( sizeof( i64 ) == 8 );
static_assert( sizeof( u8 ) == 1 );
static_assert( sizeof( u16 ) == 2 );
static_assert( sizeof( u32 ) == 4 );
static_assert( sizeof( u64 ) == 8 );
static_assert( sizeof( f32 ) == 4 );
static_assert( sizeof( f64 ) == 8 );

#include "value.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "result_code.h"

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

	Lexer lexer;
	Parser parser;
	Interpreter interpreter;

	lexer.run( filename, std::move( data ) );
	//std::cout << "lexer finished." << std::endl;

	parser.run( std::move( lexer.tokens ) );
	//std::cout << "Parser finished." << std::endl;

	i32 ret = interpreter.run( parser.root ).valueI32;
	//std::cout << "Interpreter finished." << std::endl;

	lexer.cleanup();
	parser.cleanup();
	interpreter.cleanup();

	if ( ret != 0 )
		std::cerr << "ReturnCode (" << ret << ")." << std::endl;

	return ret;
}

// -- Unity Build --
#include "value.cpp"
#include "token.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#include "interpreter.cpp"