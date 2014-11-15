#include "lexer.h"

Token::Token(TokenType type): mType(type) {

}

Token::Token() {

}

TokenType Token::type() const {
	return mType;
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
	switch (token.type()) {
		case TokenType::SingleChar:
			os << token.charValue;
			break;
		case TokenType::Identifier:
			os << token.strValue;
			break;
		case TokenType::Func:
			os << "func";
			break;
		case TokenType::Integer:
			os << token.intValue;
			break;
		case TokenType::If:
			os << "if";
			break;
		case TokenType::Else:
			os << "else";
			break;
		case TokenType::For:
			os << "for";
			break;
		case TokenType::While:
			os << "while";
			break;
		case TokenType::Return:
			os << "return";
			break;
		case TokenType::EndOfFile:
			os << "EOF";
			break;
	} 

	return os;
}

std::vector<Token> Lexer::tokenize(std::istream& stream) const {
	std::vector<Token> tokens;

	char currentChar;
	bool isComment = false;

	while (stream.get(currentChar)) {
		if (!isComment && currentChar == '#') {
            isComment = true;
            continue;
        }

        if (isComment && currentChar == '\n') {
            isComment = false;
            continue;
        }

        if (!isComment) {
			//Skip whitespace
			if (isspace(currentChar)) {
				continue;
			}

			//identifier: [a-zA-Z_][a-zA-Z0-9_]*
			if (isalpha(currentChar) || currentChar == '_') {
				std::string identStr = "";
				identStr += currentChar;

				while ((currentChar = stream.peek()) && (isalnum(currentChar) || currentChar == '_')) {
					identStr += currentChar;
					currentChar = stream.peek();
					stream.get(currentChar);
				}

				if (identStr == "func") {
					tokens.push_back(Token(TokenType::Func));
				} else if (identStr == "if") {
					tokens.push_back(Token(TokenType::If));
				} else if (identStr == "else") {
					tokens.push_back(Token(TokenType::Else));
				} else if (identStr == "for") {
					tokens.push_back(Token(TokenType::For));
				} else if (identStr == "while") {
					tokens.push_back(Token(TokenType::While));
				} else if (identStr == "return") {
					tokens.push_back(Token(TokenType::Return));
				} else {
					auto newToken = Token(TokenType::Identifier);
					newToken.strValue = identStr;
					tokens.push_back(newToken);
				}

				continue;
			}

			//number: [0-9.]+
			if (isdigit(currentChar)) {
				std::string numStr = "";
				numStr += currentChar;

				while ((currentChar = stream.peek()) && isdigit(currentChar)) {
					numStr += currentChar;
					currentChar = stream.peek();
					stream.get(currentChar);
				}

				auto newToken = Token(TokenType::Integer);
				newToken.intValue = std::stoi(numStr);
				tokens.push_back(newToken);
				continue;
			}

			auto newToken = Token(TokenType::SingleChar);
			newToken.charValue = currentChar;
			tokens.push_back(newToken);	
		}
	}

	tokens.push_back(Token(TokenType::EndOfFile));

	return tokens;
}