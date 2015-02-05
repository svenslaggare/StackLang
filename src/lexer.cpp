#include "lexer.h"

Token::Token(TokenType type): mType(type) {

}

Token::Token(): mType(TokenType::NoToken) {

}

TokenType Token::type() const {
	return mType;
}

std::string Token::asString() const {
	switch (type()) {
		case TokenType::NoToken:
			return "";
		case TokenType::LineBreak:
			return "\\n";
		case TokenType::SingleChar:
			return std::string({ charValue });
		case TokenType::TwoChars:
			return std::string({ charValue, charValue2 });
		case TokenType::Identifier:
			return strValue;
		case TokenType::Func:
			return "func";
		case TokenType::Integer:
			return std::to_string(intValue);
		case TokenType::Float:
			return std::to_string(floatValue);
		case TokenType::True:
			return "true";
		case TokenType::False:
			return "false";
		case TokenType::If:
			return "if";
		case TokenType::Else:
			return"else";
		case TokenType::For:
			return "for";
		case TokenType::While:
			return "while";
		case TokenType::Break:
			return "break";
		case TokenType::Return:
			return "return";
		case TokenType::New:
			return "new";
		case TokenType::Null:
			return "null";
		case TokenType::EndOfFile:
			return "EOF";
	} 
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
	os << token.asString();
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
		//The line break token only used for error messages
		if (currentChar == '\n') {
			tokens.push_back(Token(TokenType::LineBreak));
		}

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
				} else if (identStr == "null") {
					tokens.push_back(Token(TokenType::Null));
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