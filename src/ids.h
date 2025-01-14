
#pragma once

// TODO:
// BrokenBar
// Backtick

enum class TokenID
{
	Keyword,
	Identifier,
	StringLiteral,
	Number,
	Minus,
	Plus,
	Divide,
	Asterisk,
	Tilde,
	Amp,
	Pipe,
	Hat,
	Percent,
	Exclamation,
	MinusAssign,
	PlusAssign,
	DivideAssign,
	AsteriskAssign,
	TildeAssign,
	AmpAssign,
	PipeAssign,
	HatAssign,
	PercentAssign,
	ExclamationAssign,
	DoubleAmp,
	DoublePipe,
	Assign,
	DoubleAssign,
	GreaterThan,
	GreaterOrEqual,
	LesserThan,
	LesserOrEqual,
	ParenOpen,
	ParenClose,
	BraceOpen,
	BraceClose,
	SquareOpen,
	SquareClose,
	Period,
	Comma,
	Colon,
	ColonAssign,
	SemiColon,
	NewLine,
	EndOfFile,
};

enum class KeywordID
{
	Return,
	False,
	True,
	If,
	Else,
	Print,
	Println,
	Count,
};

enum class NodeType
{
	Entry,
	Block,
	Identifier,
	StringLiteral,
	Number,
	CreateArray,
	ArrayAccess,
	Count,
	Assignment,
	AssignmentOp,
	Operation,
	Equal,
	NotEqual,
	DeclFunc,
	FunctionArgs,
	FunctionCall,
	If,
	Return,
	Print,
	Println,
};

enum class ValueType
{
	Undefined,
	NumberI32,
	NumberI64,
	StringLiteral,
	Arr,
	TokenID,
	KeywordID,
	Node,
};