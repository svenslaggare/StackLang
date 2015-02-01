#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>

//The type of a token
enum class TokenType : unsigned char {
	NoToken,
	SingleChar,
	TwoChars,
	Identifier,
	Func,
	Integer,
	Float,
	True,
	False,
	If,
	Else,
	For,
	While,
	Break,
	Return,
	New,
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

	//Signals that an error has occurred
	void error(std::string message) const;
public:
	//Creates a new lexer
	Lexer(const std::unordered_set<char>& opTable);

	//Tokenizes the given input streaqm
	std::vector<Token> tokenize(std::istream& stream) const;
};