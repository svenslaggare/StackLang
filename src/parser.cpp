#include <stdexcept>
#include "parser.h"
#include "lexer.h"
#include "ast.h"

Parser::Parser(std::vector<Token> tokens)
	: tokens(tokens), tokenIndex(-1) {
	binOpPrecedence['+'] = 20;
	binOpPrecedence['-'] = 20;
	binOpPrecedence['*'] = 40;
	binOpPrecedence['/'] = 40;
	binOpPrecedence['='] = 100;
}

void compileError(std::string message) {
	throw std::runtime_error("Error: " + message);
}

Token& Parser::nextToken() {
	tokenIndex++;

	if (tokenIndex >= tokens.size()) {
		compileError("Reached end of tokens");
	}

	currentToken = tokens[tokenIndex];
	return currentToken;
}

char Parser::currentTokenAsChar(std::string errorMessage) {
	if (currentToken.type() != TokenType::SingleChar) {
		compileError(errorMessage);
	}

	return currentToken.charValue;
}

bool Parser::isSingleCharToken(char character) {
	return currentToken.type() == TokenType::SingleChar && currentToken.charValue == character;
}

void Parser::assertCurrentTokenAsChar(char character, std::string errorMessage) {
	if (!isSingleCharToken(character)) {
		compileError(errorMessage);
	}
}

int Parser::getTokenPrecedence() {
	if (currentToken.type() != TokenType::SingleChar || binOpPrecedence.count(currentToken.charValue) == 0) {
		return -1;
	}

	return binOpPrecedence[currentToken.charValue];
}

std::shared_ptr<ExpressionAST> Parser::parseIntegerExpression() {
	auto intAst = std::make_shared<IntegerExpressionAST>(IntegerExpressionAST(currentToken.intValue));
	nextToken(); //Consume the int
	return intAst;
}

std::shared_ptr<ExpressionAST> Parser::parseIdentifierExpression() {
	std::string identifier = currentToken.strValue;

	//Eat the identifier.
	nextToken();

	//Simple variable reference
	if (!isSingleCharToken('(')) {
		return std::make_shared<VariableReferenceExpressionAST>(VariableReferenceExpressionAST(identifier));
	}

	//Function call
	nextToken(); //Eat the '('
	std::vector<std::shared_ptr<ExpressionAST>> arguments;

	if (!(currentToken.type() == TokenType::SingleChar && currentToken.charValue == ')')) {
		while (true) {
			auto arg = parseExpression();

			if (arg == nullptr) {
				return arg;
			}

			arguments.push_back(arg);

			if (isSingleCharToken(')')) {
				break;
			}

			if (!isSingleCharToken(',')) {
				compileError("Expected ',' or ')' in argument list.");
			}

			nextToken();
		}
	}

	//Eat the ')'
	nextToken();

	return std::make_shared<CallExpressionAST>(CallExpressionAST(identifier, arguments));
}

std::shared_ptr<ExpressionAST> Parser::parseParenthesisExpression() {
	nextToken(); //Eat the '('

	auto expr = parseExpression();

	if (expr == nullptr) {
		return expr;
	}

	assertCurrentTokenAsChar(')', "Expected ')'.");

	nextToken(); //Eat the ')'

	return expr;
}

std::shared_ptr<ExpressionAST> Parser::parsePrimaryExpression() {
	switch (currentToken.type()) {
	case TokenType::Integer:
		return parseIntegerExpression();
	case TokenType::Identifier:
		return parseIdentifierExpression();
	case TokenType::SingleChar:
		if (currentToken.charValue == '(') {
			return parseParenthesisExpression();
		}
		break;
	default:
		break;
	}	

	return std::shared_ptr<ExpressionAST>();
}

std::shared_ptr<ExpressionAST> Parser::parseBinaryOpRHS(int exprPrecedence, std::shared_ptr<ExpressionAST> lhs) {
	while (true) {
		//If this is a bin op, find its precedence
		int tokPrec = getTokenPrecedence();

		//If this is a bin op  that binds as least as tightly as the current bin op, consume it, otherwise we are done.
		if (tokPrec < exprPrecedence) {
			return lhs;
		}

		char binOp = currentToken.charValue;
		nextToken(); //Eat the operator

		//Parse the unary expression after the binary operator
		auto rhs = parseUnaryExpression();

		if (rhs == nullptr) {
			return rhs;
		}

		//If binOp binds less tightly with RHS than the operator after RHS, let the pending operator take RHS as its LHS
		int nextPrec = getTokenPrecedence();
		if (tokPrec < nextPrec) {
			rhs = parseBinaryOpRHS(tokPrec + 1, rhs);

			if (rhs == nullptr) {
				return rhs;
			}
		}

		//Merge LHS/RHS
		lhs = std::make_shared<BinaryOpExpressionAST>(lhs, rhs, binOp);
	}
}

std::shared_ptr<ExpressionAST> Parser::parseUnaryExpression() {
	//If the current token isn't an operator, is must be a primary expression
	if (currentToken.type() != TokenType::SingleChar || (isSingleCharToken('(') || isSingleCharToken(','))) {
		return parsePrimaryExpression();
	}

	//If this is a unary operator, read it.
	int opChar = currentToken.charValue;
	nextToken(); //Eat the operator

	auto operand = parseUnaryExpression();

	if (operand != nullptr) {
		return std::make_shared<UnaryOpExpressionAST>(UnaryOpExpressionAST(operand, opChar));
	} 

	return operand;
}

std::shared_ptr<ExpressionAST> Parser::parseExpression() {
	auto lhs = parseUnaryExpression();

	if (lhs == nullptr) {
		return lhs;
	}

	return parseBinaryOpRHS(0, lhs);
}

std::shared_ptr<StatementAST> Parser::parseStatement() {
	std::shared_ptr<StatementAST> statement;

	switch (currentToken.type()) {
	case TokenType::Return:
		{
			nextToken();
			auto returnExpr = parseExpression();
			statement = std::make_shared<ReturnStatementAST>(ReturnStatementAST(returnExpr));
			assertCurrentTokenAsChar(';', "Expected ';' after statement.");
		}
		break;
	default:
		//Simple statement, one expression.
		auto expr = parseExpression();
		statement = std::make_shared<ExpressionStatementAST>(ExpressionStatementAST(expr));
		assertCurrentTokenAsChar(';', "Expected ';' after statement.");
		break;
	}

	return statement;
}

std::shared_ptr<BlockAST> Parser::parseBlock() {
	nextToken();
	assertCurrentTokenAsChar('{', "Expected '{'.");

	std::vector<std::shared_ptr<StatementAST>> statements;

	nextToken(); //Eat the '{'

	while (true) {
		auto statement = parseStatement();

		if (statement != nullptr) {
			statements.push_back(statement);
		}

		nextToken();

		if (isSingleCharToken('}')) {
			break;
		}
	}

	return std::make_shared<BlockAST>(BlockAST(statements));
}

std::shared_ptr<FunctionAST> Parser::parseFunctionDef() {
	//Eat the 'func'
	nextToken();

	//The name
	if (currentToken.type() != TokenType::Identifier) {
		compileError("Expected identifier.");
	}

	std::string name = currentToken.strValue;

	//Arguments
	nextToken();
	if (currentTokenAsChar() != '(')  {
		compileError("Expected ')' in prototype.");
	}

	std::vector<std::string> arguments;

	bool isSep = false;
	while (true) {
		nextToken();
		
		if (isSep) {
			if (currentToken.type() == TokenType::SingleChar && currentToken.charValue == ')') {
				break;
			}

			if (currentToken.type() == TokenType::SingleChar && currentToken.charValue == ',') {
				isSep = false;
			} else {
				compileError("Expected ',' between arguments.");
			}
		} else {
			if (currentToken.type() == TokenType::Identifier) {
				arguments.push_back(currentToken.strValue);
				isSep = true;
			} else {
				compileError("Expected identifier.");
			}
		}
	}

	//Body
	nextToken();
	if (currentTokenAsChar() != '=')  {
		compileError("Expected '=' after prototype.");
	}

	auto body = parseBlock();

	return std::make_shared<FunctionAST>(FunctionAST(name, arguments, body));
}

std::shared_ptr<ProgramAST> Parser::parse() {
	nextToken();
	std::vector<std::shared_ptr<FunctionAST>> functions;

	while (true) {
		switch (currentToken.type()) {
			case TokenType::EndOfFile:
				return std::make_shared<ProgramAST>(ProgramAST(functions));
			case TokenType::Func:
				{
					auto funcDef = parseFunctionDef();
					functions.push_back(funcDef);
					nextToken();
				}
				break;
			default:
				break;
		}
	}
}