
#include <filesystem>

#include "os.h"

Value BuiltInNode_Struct_OS_MkDir( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "mkdir", args, 1 );
	Arg arg = &args->children[ 0 ];
	std::string path = interpreter->run( arg ).get_as_string( interpreter, arg );
	if ( path == "" )
		return false;
	return std::filesystem::create_directories( path );
}

Value BuiltInNode_Struct_OS_Rm( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "rm", args, 1 );
	Arg arg = &args->children[ 0 ];
	std::string path = interpreter->run( arg ).get_as_string( interpreter, arg );
	if ( path == "" )
		return false;
	std::filesystem::remove_all( path );
	return true;
}

Value BuiltInNode_Struct_OS_Open( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;

	std::string filename;
	std::ios_base::openmode mode;

	switch ( args->children.size() )
	{
	case 1:
		{
			Arg arg = &args->children[ 0 ];
			filename = interpreter->run( arg ).get_as_string( interpreter, arg );
			if ( std::filesystem::exists( filename ) )
				mode = std::ios_base::in | std::ios_base::out;
			else
				mode = std::ios_base::out;
		}
		break;

	case 2:
		{
			Arg arg = &args->children[ 0 ];
			filename = interpreter->run( arg ).get_as_string( interpreter, arg.next() );
			mode = (std::ios_base::openmode)interpreter->run( arg ).get_as_i32( interpreter, arg );
		}
		break;

	default:
		interpreter->fatal( RESULT_CODE_INVALID_ARGS_BUILTIN_FUNC, args, "open received incorrect arguments." );
	}

	std::filesystem::create_directories( std::filesystem::path( filename ).parent_path().string() );

	std::fstream file( filename, mode );

	if ( !file.is_open() )
		return nullptr;

	return { filename, file };
}

Value BuiltInNode_Struct_OS_Close( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "close", args, 1 );
	Arg arg = &args->children[ 0 ];
	Value &value = interpreter->run( arg ).deref();
	if ( value.type != ValueType::File )
		interpreter->fatal( RESULT_CODE_VALUE_NOT_A_FILE, args, "close called on a value that is not a file." );
	value.file->close();
	value.file = nullptr;
	return true;
}

Value BuiltInNode_Struct_OS_Exists( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "exists", args, 1 );
	Arg arg = &args->children[ 0 ];
	std::string path = interpreter->run( arg ).get_as_string( interpreter, arg );
	if ( path == "" )
		return false;
	return std::filesystem::exists( path );
}

Value BuiltInNode_Struct_OS_FileSize( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "file_size", args, 1 );
	Arg arg = &args->children[ 0 ];
	Value &value = interpreter->run( arg ).deref();
	if ( value.type == ValueType::File )
	{
		std::streampos current = value.file->tellg();
		value.file->seekg( 0, std::ios::end );
		std::streampos filesize = value.file->tellg();
		value.file->seekg( current, std::ios::beg );
		return static_cast<i64>( filesize );
	}
	return static_cast<i64>( std::filesystem::file_size( value.get_as_string( interpreter, arg ) ) );
}

Value BuiltInNode_Struct_OS_Read( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "read", args, 1 );
	Arg arg = &args->children[ 0 ];
	Value &value = interpreter->run( arg ).deref();
	if ( value.type != ValueType::File )
		interpreter->fatal( RESULT_CODE_VALUE_NOT_A_FILE, args, "read called on a value that is not a file." );
	// TODO : 
	// what should read be done in
	// bytes
	// lines
	// custom delim?
	return 0;
}

Value BuiltInNode_Struct_OS_ReadFile( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "read_file", args, 1 );
	Arg arg = &args->children[ 0 ];
	std::fstream file( interpreter->run( arg ).get_as_string( interpreter, arg ), std::ios_base::in | std::ios_base::binary );
	if ( !file.is_open() )
		return nullptr;
	std::string ret = std::string( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );
	file.close();
	return ret;
}

Value BuiltInNode_Struct_OS_Write( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "write", args, 2 );
	Arg arg = &args->children[ 0 ];
	Value &value = interpreter->run( arg.next() ).deref();
	if ( value.type != ValueType::File )
		interpreter->fatal( RESULT_CODE_VALUE_NOT_A_FILE, args, "write called on a value that is not a file." );
	std::string data = interpreter->run( arg ).get_as_string( interpreter, arg );
	i64 size = static_cast<i64>( data.size() );
	value.file->write( data.data(), data.size() );
	return size;
}