
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <print>

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

i32 main( i32 argc, char *argv[] )
{
	if ( argc < 2 )
	{
		std::println( stderr, "Missing file to run." );
		return RESULT_CODE_NO_FILE_INPUT_TO_PROCESS;
	}

	std::string filename = argv[ 1 ];
	std::string data;

	{
		std::ifstream file( filename );

		if ( !file.is_open() )
		{
			std::println( stderr, "Unable to open file: {}", filename );
			return RESULT_CODE_FAILED_TO_OPEN_INPUT_FILE;
		}

		data = std::string( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );
	}

	Lexer lexer;
	Parser parser;
	Interpreter interpreter;

	interpreter.set_args( argc - 1, &argv[ 1 ] );

	lexer.run( filename, std::move( data ) );
	parser.run( std::move( lexer.tokens ) );
	i32 ret = interpreter.run( std::move( lexer.filenames ), parser.root ).valueI32;

	lexer.cleanup();
	parser.cleanup();
	interpreter.cleanup();

	if ( ret != 0 )
		std::println( stderr, "ReturnCode ( {} ).", ret );

	return ret;
}

// -- Unity Build --
#include "value.cpp"
#include "token.cpp"
#include "enums.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#include "interpreter.cpp"
#include "os.cpp"
#include "net.cpp"