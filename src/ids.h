
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
	DoublePeriod,
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
	Assert,
	For,
	While,
};

enum class NodeID
{
	Entry,
	Block,
	Identifier,
	CreateIdentifier,
	StringLiteral,
	Number,
	CreateStruct,
	CreateArray,
	ArrayAccess,
	Assignment,
	Operation,
	AssignmentOp,
	Equal,
	NotEqual,
	DeclFunc,
	FunctionArgs,
	FunctionCall,
	If,
	Return,
	Print,
	Println,
	Assert,
	ForNumberRange,
	While,
};

enum class ValueType
{
	Undefined,
	NumberI32,
	NumberI64,
	StringLiteral,
	Struct,
	Arr,
	TokenID,
	KeywordID,
	Node,
	InbuiltFunc,
	Reference,
};