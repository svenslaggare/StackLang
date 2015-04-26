#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>

//The type of a token
enum class TokenType : unsigned char {
	NoToken,
	LineBreak,
	SingleChar,
	TwoChars,
	Identifier,
	Func,
	Integer,
	Float,
	True,
	False,
	Char,
	String,
	If,
	Else,
	For,
	While,
	Break,
	Return,
	New,
	Null,
	Namespace,
	Cast,
	Class,
	Using,
	EndOfFile
};

//Represents a token
class Token {
private:
	TokenType mType;
public:
	//Creates a new token
	Token(TokenType type);
	Token();

	//The type of the token
	TokenType type() const;

	//Returns the current token as a string
	std::string asString() const;

	//The values
	char charValue;
	char charValue2;

	std::string strValue;

	int intValue;
	float floatValue;
};

std::ostream& operator<<(std::ostream& os, const Token& token);

//Represents a lexer
class Lexer {
private:
	const std::unordered_set<char>& mOpTable;
	const std::unordered_set<char>& mTwoOpTable;

	//Signals that an error has occurred
	void error(std::string message) const;
public:
	//Creates a new lexer
	Lexer(const std::unordered_set<char>& opTable, const std::unordered_set<char>& twoOpTable);

	//Tokenizes the given input streaqm
	std::vector<Token> tokenize(std::istream& stream) const;
};