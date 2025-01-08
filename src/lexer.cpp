
#include <iostream>
#include <unordered_map>

#include "lexer.h"

constexpr static KeywordType Keywords[] =
{
	{
		.id = KeywordID::False,
		.name = "False",
	},
	{
		.id = KeywordID::True,
		.name = "True",
	},
};

std::ostream & operator << ( std::ostream &out, const KeywordType &keywordType )
{
	return out << "Keyword: " << Keywords[ static_cast<int>( keywordType.id ) ].name;
}

std::ostream & operator << ( std::ostream &out, const KeywordID &keywordID )
{
	return out << "Keyword: " << Keywords[ static_cast<int>( keywordID ) ].name;
}

static std::unordered_map<std::string, KeywordID> KeywordsMap =
{
	{ "false", KeywordID::False },
	{ "true", KeywordID::True },
};

constexpr static TokenType TokenTypes[] =
{
	{
		.id = TokenID::Keyword,
		.name = "Keyword",
	},
	{
		.id = TokenID::Identifier,
		.name = "Identifier",
	},
	{
		.id = TokenID::StringLiteral,
		.name = "StringLiteral",
	},
	{
		.id = TokenID::Number,
		.name = "Number",
	},
	{
		.id = TokenID::Minus,
		.name = "Minus",
	},
	{
		.id = TokenID::MinusAssign,
		.name = "MinusAssign",
	},
	{
		.id = TokenID::Plus,
		.name = "Plus",
	},
	{
		.id = TokenID::PlusAssign,
		.name = "PlusAssign",
	},
	{
		.id = TokenID::Divide,
		.name = "Divide",
	},
	{
		.id = TokenID::DivideAssign,
		.name = "DivideAssign",
	},
	{
		.id = TokenID::Asterisk,
		.name = "Asterisk",
	},
	{
		.id = TokenID::AsteriskAssign,
		.name = "AsteriskAssign",
	},
	{
		.id = TokenID::Assign,
		.name = "Assign",
	},
	{
		.id = TokenID::Equal,
		.name = "Equal",
	},
	{
		.id = TokenID::GreaterThan,
		.name = "GreaterThan",
	},
	{
		.id = TokenID::GreaterOrEqual,
		.name = "GreaterOrEqual",
	},
	{
		.id = TokenID::LesserThan,
		.name = "LesserThan",
	},
	{
		.id = TokenID::LesserOrEqual,
		.name = "LesserOrEqual",
	},
	{
		.id = TokenID::BitwiseAnd,
		.name = "BitwiseAnd",
	},
	{
		.id = TokenID::LogicalAnd,
		.name = "LogicalAnd",
	},
	{
		.id = TokenID::BitwiseOr,
		.name = "BitwiseOr",
	},
	{
		.id = TokenID::LogicalOr,
		.name = "LogicalOr",
	},
	{
		.id = TokenID::LogicalNot,
		.name = "LogicalNot",
	},
	{
		.id = TokenID::BitwiseNot,
		.name = "BitwiseNot",
	},
	{
		.id = TokenID::ParenOpen,
		.name = "ParenOpen",
	},
	{
		.id = TokenID::ParenClose,
		.name = "ParenClose",
	},
	{
		.id = TokenID::BlockOpen,
		.name = "BlockOpen",
	},
	{
		.id = TokenID::BlockClose,
		.name = "BlockClose",
	},
	{
		.id = TokenID::Period,
		.name = "Period",
	},
	{
		.id = TokenID::Comma,
		.name = "Comma",
	},
	{
		.id = TokenID::Colon,
		.name = "Colon",
	},
	{
		.id = TokenID::SemiColon,
		.name = "SemiColon",
	},
	{
		.id = TokenID::NewLine,
		.name = "NewLine",
	},
	{
		.id = TokenID::EndOfFile,
		.name = "EndOfFile",
	},
};

std::ostream & operator << ( std::ostream &out, const Token &token )
{
	out << "Token: " << TokenTypes[ static_cast<int>( token.id ) ].name;

	switch ( token.id )
	{
	case TokenID::Keyword:
		out << " ( " << token.valueString << " ) ";
		break;

	case TokenID::Identifier:
		out << " ( " << token.valueString << " ) ";
		break;

	case TokenID::StringLiteral:
		out << " ( " << token.valueString << " ) ";
		break;

	case TokenID::Number:
		out << " ( " << token.valueNumber << " ) ";
		break;

	case TokenID::Minus:
		out << " ( - ) ";
		break;

	case TokenID::MinusAssign:
		out << " ( -= ) ";
		break;

	case TokenID::Plus:
		out << " ( + ) ";
		break;

	case TokenID::PlusAssign:
		out << " ( ++ ) ";
		break;

	case TokenID::Divide:
		out << " ( / ) ";
		break;

	case TokenID::DivideAssign:
		out << " ( /= ) ";
		break;

	case TokenID::Asterisk:
		out << " ( * ) ";
		break;

	case TokenID::AsteriskAssign:
		out << " ( *= ) ";
		break;

	case TokenID::Assign:
		out << " ( = ) ";
		break;

	case TokenID::Equal:
		out << " ( == ) ";
		break;

	case TokenID::GreaterThan:
		out << " ( > ) ";
		break;

	case TokenID::GreaterOrEqual:
		out << " ( >= ) ";
		break;

	case TokenID::LesserThan:
		out << " ( < ) ";
		break;

	case TokenID::LesserOrEqual:
		out << " ( <= ) ";
		break;

	case TokenID::BitwiseAnd:
		out << " ( & ) ";
		break;

	case TokenID::LogicalAnd:
		out << " ( && ) ";
		break;

	case TokenID::BitwiseOr:
		out << " ( | ) ";
		break;

	case TokenID::LogicalOr:
		out << " ( || ) ";
		break;

	case TokenID::BitwiseNot:
		out << " ( ~ ) ";
		break;

	case TokenID::LogicalNot:
		out << " ( ! ) ";
		break;

	case TokenID::ParenOpen:
		out << " ( ( ) ";
		break;

	case TokenID::ParenClose:
		out << " ( ) ) ";
		break;

	case TokenID::BlockOpen:
		out << " ( { ) ";
		break;

	case TokenID::BlockClose:
		out << " ( } ) ";
		break;

	case TokenID::Period:
		out << " ( . ) ";
		break;

	case TokenID::Comma:
		out << " ( , ) ";
		break;

	case TokenID::Colon:
		out << " ( : ) ";
		break;

	case TokenID::SemiColon:
		out << " ( ; ) ";
		break;

	case TokenID::NewLine:
		break;

	case TokenID::EndOfFile:
		break;
	}

	return out;
}

struct InternalLexer
{
	std::vector<Token> tokens;
	const char *txt;
	std::string stringValue;
};

// Note: \n is not skipped, it is used to break statements up
static void skip_whitespace( InternalLexer *lexer )
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

static Token next_token( InternalLexer *lexer, Token *lastToken )
{
	skip_whitespace( lexer );

	char c = *lexer->txt;

	while ( c != '\0' )
	{
		if ( is_identifier_start( c ) )
		{
			const char *start = lexer->txt;
			size_t len = 0;

			do
			{
				len += 1;
				c = *(++lexer->txt);
			} while ( is_identifier( c ) );

			lexer->stringValue.assign( start, len );

			if ( KeywordsMap.find( lexer->stringValue ) != KeywordsMap.end() )
				return { .id = TokenID::Keyword, .valueString = lexer->stringValue };

			return { .id = TokenID::Identifier, .valueString = lexer->stringValue };
		}

		if ( is_digit( c ) )
		{
			size_t len = 0;
			int64_t value = std::stoll( lexer->txt, &len );
			lexer->txt += len;
			return { .id = TokenID::Number, .valueNumber = value };
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
			return { TokenID::BlockOpen };

		case '}':
			lexer->txt += 1;
			return { TokenID::BlockClose };

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
				size_t len = 0;
				char p;

				lexer->stringValue.clear();

				do
				{
					p = c;
					len += 1;
					c = *(++lexer->txt);
					if ( c == '\0' )
					{
						std::cerr << "String literal not closed." << std::endl;
						exit( RESULT_CODE_STRING_LITERAL_NOT_CLOSED );
					}

					if ( p == '\\' && c == '"' )
					{
						lexer->stringValue.append( start, len - 1 );
						lexer->stringValue.append( "\"" );
						start = ++lexer->txt;
						len = 0;
						c = *start;
					}

				} while ( c != '"' );

				lexer->stringValue.append( start, len );

				lexer->txt += 1;

				return { .id = TokenID::StringLiteral, .valueString = lexer->stringValue };
			}
			break;

		case '/':
			{
				switch ( *( lexer->txt + 1 ) )
				{
				case '*':
					{
						lexer->txt += 2;

						int level = 1;
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

std::vector<Token> Lexer::run( std::string data )
{
	InternalLexer lex;

	lex.tokens.reserve( 65536 );
	lex.txt = data.c_str();
	lex.stringValue.reserve( 512 );

	Token token;
	token.id = TokenID::EndOfFile;

	do
	{
		token = next_token( &lex, &token );
		lex.tokens.push_back( token );

	} while ( token.id != TokenID::EndOfFile );

	return lex.tokens;
}