
#include "lexer.h"

// Note: \n is not skipped, it is used to break statements up
static void skip_whitespace( Lexer *lexer )
{
	char c = *lexer->txt;

	while ( c != '\0' )
	{
		switch ( c )
		{
		case '\t':
		case '\v':
		case '\f':
		case '\r':
		case ' ':
			break;

		default:
			return;
		}

		c = *(++lexer->txt);
	}
}

static bool is_digit( char c )
{
	switch ( c )
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return true;
	}
	return false;
}

constexpr bool IdentiferCharLUT[ 123 ] =
{
	false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true, false, false,
	false, false, false, false, false,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true, false, false, false, false,  true, false,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,
};

static bool is_identifier_start( char c )
{
	if ( c > 122 || is_digit( c ) )
		return false;
	return IdentiferCharLUT[ c ];
}

static bool is_identifier( char c )
{
	if ( c > 122 )
		return false;
	return IdentiferCharLUT[ c ];
}

static Token next_token( Lexer *lexer, Token *lastToken )
{
	skip_whitespace( lexer );

	char c = *lexer->txt;

	while ( c != '\0' )
	{
		if ( is_identifier_start( c ) )
		{
			const char *start = lexer->txt;
			u64 len = 0;

			do
			{
				len += 1;
				c = *(++lexer->txt);
			} while ( is_identifier( c ) );

			lexer->str.assign( start, len );

			const KeywordType *kw = get_keyword( lexer->str );
			if ( kw )
				return { .id = TokenID::Keyword, .value = { static_cast<i32>( kw->id ), lexer->str.c_str() } };

			return { .id = TokenID::Identifier, .value = lexer->str };
		}

		if ( is_digit( c ) )
		{
			i64 number;
			char *end;

			if ( to_int( &number, lexer->txt, &end ) == ToIntResult::Success )
			{
				lexer->txt = end;
				return { .id = TokenID::Number, .value = number };
			}

			std::cerr << "[Lexer] Could not convert value to int ( " << lexer->txt << " )." << std::endl;
			exit( RESULT_CODE_STRING_LITERAL_NOT_CLOSED );
		}

		switch ( c )
		{
		case '-':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { TokenID::MinusAssign };
			}
			else
			{
				lexer->txt += 1;
				return { TokenID::Minus };
			}

		case '+':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { TokenID::PlusAssign };
			}
			else
			{
				lexer->txt += 1;
				return { TokenID::Plus };
			}

		case '*':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { TokenID::AsteriskAssign };
			}
			else
			{
				lexer->txt += 1;
				return { TokenID::Asterisk };
			}

		case '=':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { TokenID::Equal };
			}
			lexer->txt += 1;
			return { TokenID::Assign };

		case '>':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { TokenID::GreaterOrEqual };
			}
			lexer->txt += 1;
			return { TokenID::GreaterThan };

		case '<':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { TokenID::LesserOrEqual };
			}
			lexer->txt += 1;
			return { TokenID::LesserThan };

		case '&':
			if ( *( lexer->txt + 1 ) == '&' )
			{
				lexer->txt += 2;
				return { TokenID::LogicalAnd };
			}
			lexer->txt += 1;
			return { TokenID::BitwiseAnd };

		case '|':
			if ( *( lexer->txt + 1 ) == '|' )
			{
				lexer->txt += 2;
				return { TokenID::LogicalOr };
			}
			lexer->txt += 1;
			return { TokenID::BitwiseOr };

		case '~':
			lexer->txt += 1;
			return { TokenID::BitwiseNot };

		case '!':
			lexer->txt += 1;
			return { TokenID::LogicalNot };

		case '(':
			lexer->txt += 1;
			return { TokenID::ParenOpen };

		case ')':
			lexer->txt += 1;
			return { TokenID::ParenClose };

		case '{':
			lexer->txt += 1;
			return { TokenID::BraceOpen };

		case '}':
			lexer->txt += 1;
			return { TokenID::BraceClose };

		case '.':
			lexer->txt += 1;
			return { TokenID::Period };

		case ',':
			lexer->txt += 1;
			return { TokenID::Comma };

		case ':':
			lexer->txt += 1;
			return { TokenID::Colon };

		case ';':
			lexer->txt += 1;
			return { TokenID::SemiColon };

		case '\n':
			if ( lastToken->id != TokenID::NewLine && lastToken->id != TokenID::EndOfFile )
			{
				lexer->txt += 1;
				return { TokenID::NewLine };
			}
			break;

		case '"':
			{
				const char *start = ++lexer->txt;
				u64 len = 0;
				char p;

				// TODO : do in a loop and concat any strings place next to each other

				lexer->str.clear();

				do
				{
					p = c;
					len += 1;
					c = *(++lexer->txt);
					if ( c == '\0' )
					{
						std::cerr << "[Lexer] String literal not closed." << std::endl;
						exit( RESULT_CODE_STRING_LITERAL_NOT_CLOSED );
					}

					if ( p == '\\' && c == '"' )
					{
						lexer->str.append( start, len - 1 );
						lexer->str.append( "\"" );
						start = ++lexer->txt;
						len = 0;
						c = *start;
					}

				} while ( c != '"' );

				lexer->str.append( start, len );

				lexer->txt += 1;

				return { .id = TokenID::StringLiteral, .value = lexer->str };
			}
			break;

		case '/':
			{
				switch ( *( lexer->txt + 1 ) )
				{
				case '*':
					{
						lexer->txt += 2;

						i32 level = 1;
						char n;

						while ( level > 0 )
						{
							while ( true )
							{
								c = *(++lexer->txt);
								if ( c == '\0' )
									return { TokenID::EndOfFile };
								n = *( lexer->txt + 1 );
								if ( c == '/' && n == '*' )
								{
									lexer->txt += 2;
									level += 1;
									break;
								}
								else if ( c == '*' && n == '/' )
								{
									lexer->txt += 2;
									level -= 1;
									break;
								}
							}
						}
					}
					break;

				case '/':
					do
					{
						c = *(++lexer->txt);
						if ( c == '\0' )
							return { TokenID::EndOfFile };
					} while ( c != '\n' );
					break;

				case '=':
					lexer->txt += 2;
					return { TokenID::DivideAssign };

				default:
					lexer->txt += 1;
					return { TokenID::Divide };
				}
			}
		}

		c = *(++lexer->txt);
	}

	return { TokenID::EndOfFile };
}

void Lexer::run( std::string data )
{
	tokens.clear();
	tokens.reserve( 65536 );
	txt = data.c_str();
	str.reserve( 512 );

	Token token;
	token.id = TokenID::EndOfFile;

	do
	{
		token = next_token( this, &token );
		tokens.push_back( token );

	} while ( token.id != TokenID::EndOfFile );

	txt = nullptr;
	str.clear();
}