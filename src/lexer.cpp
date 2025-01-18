
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
				return { .id = TokenID::Keyword, .value = { kw->id, lexer->str.c_str() }, .line = lexer->line };

			return { .id = TokenID::Identifier, .value = lexer->str, .line = lexer->line };
		}

		if ( is_digit( c ) )
		{
			i64 number;
			char *end;

			if ( to_int( &number, lexer->txt, &end ) == ToIntResult::Success )
			{
				lexer->txt = end;
				return { .id = TokenID::Number, .value = number, .line = lexer->line };
			}

			std::cerr << "[Lexer] Could not convert value to int ( " << lexer->txt << " )." << std::endl;
			exit( RESULT_CODE_CANNOT_CONVERT_TO_INT );
		}

		switch ( c )
		{
		case '-':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::MinusAssign, .line = lexer->line };
			}
			else
			{
				lexer->txt += 1;
				return { .id = TokenID::Minus, .line = lexer->line };
			}

		case '+':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::PlusAssign, .line = lexer->line };
			}
			else
			{
				lexer->txt += 1;
				return { .id = TokenID::Plus, .line = lexer->line };
			}

		case '*':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::AsteriskAssign, .line = lexer->line };
			}
			else
			{
				lexer->txt += 1;
				return { .id = TokenID::Asterisk, .line = lexer->line };
			}

		case '=':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::DoubleAssign, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::Assign, .line = lexer->line };

		case '^':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::HatAssign, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::Hat, .line = lexer->line };

		case '%':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::PercentAssign, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::Percent, .line = lexer->line };

		case '~':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::TildeAssign, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::Tilde, .line = lexer->line };

		case '>':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::GreaterOrEqual, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::GreaterThan, .line = lexer->line };

		case '<':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::LesserOrEqual, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::LesserThan, .line = lexer->line };

		case '&':
			if ( *( lexer->txt + 1 ) == '&' )
			{
				lexer->txt += 2;
				return { .id = TokenID::DoubleAmp, .line = lexer->line };
			}
			else if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::AmpAssign, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::Amp, .line = lexer->line };

		case '|':
			if ( *( lexer->txt + 1 ) == '|' )
			{
				lexer->txt += 2;
				return { .id = TokenID::DoublePipe, .line = lexer->line };
			}
			else if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::PipeAssign, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::Pipe, .line = lexer->line };

		case '!':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::ExclamationAssign, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::Exclamation, .line = lexer->line };

		case '(':
			lexer->txt += 1;
			return { .id = TokenID::ParenOpen, .line = lexer->line };

		case ')':
			lexer->txt += 1;
			return { .id = TokenID::ParenClose, .line = lexer->line };

		case '{':
			lexer->txt += 1;
			return { .id = TokenID::BraceOpen, .line = lexer->line };

		case '}':
			lexer->txt += 1;
			return { .id = TokenID::BraceClose, .line = lexer->line };

		case '[':
			lexer->txt += 1;
			return { .id = TokenID::SquareOpen, .line = lexer->line };

		case ']':
			lexer->txt += 1;
			return { .id = TokenID::SquareClose, .line = lexer->line };

		case '.':
			if ( *( lexer->txt + 1 ) == '.' )
			{
				lexer->txt += 2;
				return { .id = TokenID::DoublePeriod, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::Period, .line = lexer->line };

		case ',':
			lexer->txt += 1;
			return { .id = TokenID::Comma, .line = lexer->line };

		case ':':
			if ( *( lexer->txt + 1 ) == '=' )
			{
				lexer->txt += 2;
				return { .id = TokenID::ColonAssign, .line = lexer->line };
			}
			lexer->txt += 1;
			return { .id = TokenID::Colon, .line = lexer->line };

		case ';':
			lexer->txt += 1;
			return { .id = TokenID::SemiColon, .line = lexer->line };

		case '\n':
			lexer->line += 1;
			if ( lastToken->id != TokenID::NewLine && lastToken->id != TokenID::EndOfFile )
			{
				lexer->txt += 1;
				return { .id = TokenID::NewLine, .line = lexer->line - 1 };
			}
			break;

		case '"':
			{
				// TODO : do in a loop and concat any strings placed next to each other
				lexer->str.clear();

				if ( *++lexer->txt != '"' )
				{
					const char *start = lexer->txt;
					u64 len = 0;
					char p;
					c = *start;

					while ( c != '"' )
					{
						p = c;
						len += 1;
						c = *(++lexer->txt);
						if ( c == '\0' )
						{
							std::cerr << "[Lexer] String literal not closed." << std::endl;
							exit( RESULT_CODE_STRING_LITERAL_NOT_CLOSED );
						}
						else if ( c == '\n' )
						{
							lexer->line += 1;
						}
						else if ( p == '\\' && c == '"' )
						{
							lexer->str.append( start, len - 1 );
							lexer->str.append( "\"" );
							start = ++lexer->txt;
							len = 0;
							c = *start;
						}
					}

					lexer->str.append( start, len );
				}

				lexer->txt += 1;

				return { .id = TokenID::StringLiteral, .value = lexer->str, .line = lexer->line };
			}
			break;

		case '/':
			{
				switch ( *( lexer->txt + 1 ) )
				{
				case '*':
					{
						lexer->txt += 1;

						i32 level = 1;
						char n;

						while ( level > 0 )
						{
							while ( true )
							{
								c = *(++lexer->txt);
								if ( c == '\0' )
								{
									return { .id = TokenID::EndOfFile, .line = lexer->line };
								}
								else if ( c == '\n' )
								{
									lexer->line += 1;
								}
								else
								{
									n = *( lexer->txt + 1 );
									if ( c == '/' && n == '*' )
									{
										lexer->txt += 1;
										level += 1;
										break;
									}
									else if ( c == '*' && n == '/' )
									{
										lexer->txt += 1;
										level -= 1;
										break;
									}
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
							return { .id = TokenID::EndOfFile, .line = lexer->line };
					} while ( c != '\n' );
					lexer->line += 1;
					break;

				case '=':
					lexer->txt += 2;
					return { .id = TokenID::DivideAssign, .line = lexer->line };

				default:
					lexer->txt += 1;
					return { .id = TokenID::Divide, .line = lexer->line };
				}
			}
		}

		c = *(++lexer->txt);
	}

	return { .id = TokenID::EndOfFile, .line = lexer->line };
}

void Lexer::run( std::string data )
{
	tokens.clear();
	tokens.reserve( 65536 );
	txt = data.c_str();
	str.reserve( 512 );
	line = 1;

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

void Lexer::cleanup()
{
	tokens.clear();
	txt = nullptr;
	str.clear();
	line = 0;
}