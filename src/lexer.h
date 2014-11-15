#pragma once
#include <string>
#include <iostream>
#include <vector>

//The type of a token
enum class TokenType : unsigned char {
	SingleChar,
	Identifier,
	Func,
	Integer,
	If,
	Else,
	For,
	While,
	Return,
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
	std::string strValue;
	int intValue;
};

std::ostream& operator<<(std::ostream& os, const Token& token);

//Represents a lexer
class Lexer {
public:
	//Tokenizes the given input streaqm
	std::vector<Token> tokenize(std::istream& stream) const;
};