#include "lexer.h"

Token::Token(TokenType type): mType(type) {

}

Token::Token(): mType(TokenType::NoToken) {

}

TokenType Token::type() const {
	return mType;
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
	switch (token.type()) {
		case TokenType::NoToken:
			break;
		case TokenType::SingleChar:
			os << token.charValue;
			break;
		case TokenType::TwoChars:
			os << token.charValue << token.charValue2;
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
		case TokenType::Float:
			os << token.floatValue;
			break;
		case TokenType::True:
			os << "true";
			break;
		case TokenType::False:
			os << "false";
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
		case TokenType::Break:
			os << "break";
			break;
		case TokenType::Return:
			os << "return";
			break;
		case TokenType::New:
			os << "new";
			break;
		case TokenType::EndOfFile:
			os << "EOF";
			break;
	} 

	return os;
}

Lexer::Lexer(const std::unordered_set<char>& opTable)
	: mOpTable(opTable) {

}

void Lexer::error(std::string message) const {
	throw std::runtime_error(message);
}

std::vector<Token> Lexer::tokenize(std::istream& stream) const {
	std::vector<Token> tokens;

	char currentChar;
	bool isComment = false;
	Token prevToken;

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
				} else if (identStr == "true") {
					tokens.push_back(Token(TokenType::True));
				} else if (identStr == "false") {
					tokens.push_back(Token(TokenType::False));
				} else if (identStr == "if") {
					tokens.push_back(Token(TokenType::If));
				} else if (identStr == "else") {
					tokens.push_back(Token(TokenType::Else));
				} else if (identStr == "for") {
					tokens.push_back(Token(TokenType::For));
				} else if (identStr == "while") {
					tokens.push_back(Token(TokenType::While));
				} else if (identStr == "break") {
					tokens.push_back(Token(TokenType::Break));
				} else if (identStr == "return") {
					tokens.push_back(Token(TokenType::Return));
				} else if (identStr == "new") {
					tokens.push_back(Token(TokenType::New));
				} else {
					auto newToken = Token(TokenType::Identifier);
					newToken.strValue = identStr;
					tokens.push_back(newToken);
				}

				prevToken = tokens[tokens.size() - 1];
				continue;
			}

			//number: [0-9.]+
			if (isdigit(currentChar)) {
				std::string numStr = "";
				numStr += currentChar;
				bool containsDecimalPoint = false;

				while ((currentChar = stream.peek()) && (isdigit(currentChar) || currentChar == '.')) {
					if (currentChar == '.') {
						if (!containsDecimalPoint) {
							containsDecimalPoint = true;
						} else {
							error("The current number already contains a decimal point.");
						}
					}
					numStr += currentChar;
					currentChar = stream.peek();
					stream.get(currentChar);
				}

				auto tokenType = TokenType::Integer;

				if (containsDecimalPoint) {
					tokenType = TokenType::Float;
				}

				auto newToken = Token(tokenType);

				if (!containsDecimalPoint) {
					newToken.intValue = std::stoi(numStr);
				} else {
					newToken.floatValue = std::stof(numStr);
				}

				tokens.push_back(newToken);
				prevToken = newToken;
				continue;
			}

			Token newToken;

			//Merge two single chars to the 'TwoChars' type
			if (prevToken.type() == TokenType::SingleChar && (currentChar == '=' || currentChar == '&' || currentChar == '|') && mOpTable.count(prevToken.charValue) > 0) {
				newToken = Token(TokenType::TwoChars);
				newToken.charValue = prevToken.charValue;
				newToken.charValue2 = currentChar;
				tokens[tokens.size() - 1] = newToken;
			} else {
				newToken = Token(TokenType::SingleChar);
				newToken.charValue = currentChar;
				tokens.push_back(newToken);	
			}

			prevToken = newToken;
		}
	}

	tokens.push_back(Token(TokenType::EndOfFile));

	return tokens;
}