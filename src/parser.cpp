#include <stdexcept>
#include "parser.h"
#include "lexer.h"
#include "ast.h"

Parser::Parser(std::vector<Token> tokens)
	: tokens(tokens), tokenIndex(-1) {
	binOpPrecedence['<'] = 10;
	binOpPrecedence['>'] = 10;
	binOpPrecedence['+'] = 20;
	binOpPrecedence['-'] = 20;
	binOpPrecedence['*'] = 40;
	binOpPrecedence['/'] = 40;
	binOpPrecedence['='] = 100;

	twoCharOpPrecedence[std::make_pair('=', '=')] = 10;
	twoCharOpPrecedence[std::make_pair('!', '=')] = 10;
	twoCharOpPrecedence[std::make_pair('<', '=')] = 10;
	twoCharOpPrecedence[std::make_pair('>', '=')] = 10;

	twoCharOpPrecedence[std::make_pair('+', '=')] = 100;
	twoCharOpPrecedence[std::make_pair('-', '=')] = 100;
	twoCharOpPrecedence[std::make_pair('*', '=')] = 100;
	twoCharOpPrecedence[std::make_pair('-', '=')] = 100;

	assignmentOperators = { '+', '-', '*', '/' };
}

void Parser::compileError(std::string message) {
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

Token& Parser::peekToken() {
	int nextTokenIndex = tokenIndex + 1;

	if (nextTokenIndex >= tokens.size()) {
		compileError("Reached end of tokens");
	}

	currentToken = tokens[nextTokenIndex];
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
	if (!(currentToken.type() == TokenType::SingleChar || currentToken.type() == TokenType::TwoChars)) {
		return -1;
	}

	auto twoCharsKey = std::make_pair(currentToken.charValue, currentToken.charValue2);

	if (!(binOpPrecedence.count(currentToken.charValue) > 0 || twoCharOpPrecedence.count(twoCharsKey) > 0)) {
		return -1;
	}

	if (currentToken.type() == TokenType::SingleChar) {
		return binOpPrecedence[currentToken.charValue];
	} else {
		return twoCharOpPrecedence[twoCharsKey];
	}
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

	//Variable decleration/reference
	if (!isSingleCharToken('(')) {
		if (currentToken.type() == TokenType::Identifier) {
			std::string varName = currentToken.strValue;
			nextToken(); //Eat the identifier

			//Decleration
			return std::make_shared<VariableDeclerationExpressionAST>(VariableDeclerationExpressionAST(identifier, varName));
		} else {
			//Reference
			return std::make_shared<VariableReferenceExpressionAST>(VariableReferenceExpressionAST(identifier));
		}
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

std::shared_ptr<ExpressionAST> Parser::parseBinaryOpRHS(int exprPrecedence, std::shared_ptr<ExpressionAST> lhs, bool allowEqualAssign) {
	while (true) {
		//If this is a bin op, find its precedence
		int tokPrec = getTokenPrecedence();

		//If this is a bin op  that binds as least as tightly as the current bin op, consume it, otherwise we are done.
		if (tokPrec < exprPrecedence) {
			return lhs;
		}

		Operator op = currentToken.type() == TokenType::SingleChar ?
			Operator(currentToken.charValue) :
			Operator(currentToken.charValue, currentToken.charValue2);

		if (!op.isTwoChars()) {
			if (!allowEqualAssign && op.op1() == '=') {
				compileError("Operator '=' not allowed in this expression.");
			}
		} else {
			if (!allowEqualAssign && assignmentOperators.count(op.op1()) > 0 && op.op2() == '=') {
				compileError("Operator '=' not allowed in this expression.");
			}
		}

		nextToken(); //Eat the operator

		//Parse the unary expression after the binary operator
		auto rhs = parseUnaryExpression();

		if (rhs == nullptr) {
			return rhs;
		}

		//If binOp binds less tightly with RHS than the operator after RHS, let the pending operator take RHS as its LHS
		int nextPrec = getTokenPrecedence();
		if (tokPrec < nextPrec) {
			rhs = parseBinaryOpRHS(tokPrec + 1, rhs, allowEqualAssign);

			if (rhs == nullptr) {
				return rhs;
			}
		}

		//Merge LHS/RHS
		lhs = std::make_shared<BinaryOpExpressionAST>(lhs, rhs, op);
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
		return std::make_shared<UnaryOpExpressionAST>(UnaryOpExpressionAST(operand, Operator(opChar)));
	} 

	return operand;
}

std::shared_ptr<ExpressionAST> Parser::parseExpression(bool allowEqualAssign) {
	auto lhs = parseUnaryExpression();

	if (lhs == nullptr) {
		return lhs;
	}

	return parseBinaryOpRHS(0, lhs, allowEqualAssign);
}

std::shared_ptr<StatementAST> Parser::parseIfElseStatement() {
	nextToken(); //Eat the 'if'

	assertCurrentTokenAsChar('(', "Expected '('.");
	nextToken(); //Eat the '('

	auto condExpr = parseExpression();

	assertCurrentTokenAsChar(')', "Expected ')'.");
	nextToken(); //Eat the ')'

	//Parse the then body
	auto thenBody = parseBlock();

	//Check if else
	//nextToken();
	std::shared_ptr<BlockAST> elseBody = nullptr;

	if (peekToken().type() == TokenType::Else) {
		nextToken(); //Eat the 'else'.

		nextToken(); 
		elseBody = parseBlock();
	}

	return std::make_shared<IfElseStatementAST>(IfElseStatementAST(condExpr, thenBody, elseBody));
}

std::shared_ptr<StatementAST> Parser::parseForLoopStatement() {
	nextToken(); //Eat the 'for'

	assertCurrentTokenAsChar('(', "Expected '('.");
	nextToken(); //Eat the '('

	auto initExpr = parseExpression(true);
	assertCurrentTokenAsChar(';', "Expected ';' after statement.");
	nextToken(); //Eat the ';'

	auto condExpr = parseExpression();
	assertCurrentTokenAsChar(';', "Expected ';' after statement.");
	nextToken(); //Eat the ';'

	auto changeExpr = parseExpression(true);

	assertCurrentTokenAsChar(')', "Expected ')'.");
	nextToken(); //Eat the ')'

	// auto changeExprBin = std::dynamic_pointer_cast<BinaryOpExpressionAST>(changeExpr);

	// std::cout << *changeExprBin->leftHandSide() << std::endl;
	// std::cout << *changeExprBin->rightHandSide() << std::endl;

	//Parse the body
	auto bodyBlock = parseBlock();

	return std::make_shared<ForLoopStatementAST>(ForLoopStatementAST(initExpr, condExpr, changeExpr, bodyBlock));
}

std::shared_ptr<StatementAST> Parser::parseStatement() {
	std::shared_ptr<StatementAST> statement;

	switch (currentToken.type()) {
	case TokenType::Return:
		{
			nextToken(); //Eat the 'return'
			auto returnExpr = parseExpression();
			statement = std::make_shared<ReturnStatementAST>(ReturnStatementAST(returnExpr));
			assertCurrentTokenAsChar(';', "Expected ';' after statement.");
		}
		break;
	case TokenType::If:
		statement = parseIfElseStatement();
		break;
	case TokenType::For:
		statement = parseForLoopStatement();
		break;
	default:
		//Simple statement, one expression.
		auto expr = parseExpression(true);
		statement = std::make_shared<ExpressionStatementAST>(ExpressionStatementAST(expr));
		assertCurrentTokenAsChar(';', "Expected ';' after statement.");
		break;
	}

	return statement;
}

std::shared_ptr<BlockAST> Parser::parseBlock() {
	assertCurrentTokenAsChar('{', "Expected '{'.");

	std::vector<std::shared_ptr<StatementAST>> statements;

	nextToken(); //Eat the '{'

	while (true) {
		if (isSingleCharToken('}')) {
			break;
		}

		auto statement = parseStatement();

		if (statement != nullptr) {
			statements.push_back(statement);
		}

		nextToken();
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
	nextToken(); //Eat the name
	if (currentTokenAsChar() != '(')  {
		compileError("Expected ')' in prototype.");
	}

	nextToken(); //Eat the '('

	std::vector<std::shared_ptr<VariableDeclerationExpressionAST>> arguments;
	if (!(currentToken.type() == TokenType::SingleChar && currentToken.charValue == ')')) {
		while (true) {
			if (currentToken.type() == TokenType::Identifier) {
				std::string varType = currentToken.strValue;
				nextToken(); //Eat the type

				if (currentToken.type() == TokenType::Identifier) {
					arguments.push_back(std::make_shared<VariableDeclerationExpressionAST>(VariableDeclerationExpressionAST(varType, currentToken.strValue)));
				}
			}

			nextToken();

			if (isSingleCharToken(')')) {
				break;
			}

			if (!isSingleCharToken(',')) {
				compileError("Expected ',' or ')' in argument list.");
			}

			nextToken();
		}
	}

	//Return type
	nextToken();
	if (currentTokenAsChar() != ':')  {
		compileError("Expected ':' after arguments.");
	}

	nextToken();

	if (currentToken.type() != TokenType::Identifier) {
		compileError("Expected identifier.");
	}

	std::string returnType = currentToken.strValue;

	//Body
	nextToken(); //Eat the return type

	auto body = parseBlock();

	return std::make_shared<FunctionAST>(FunctionAST(name, arguments, returnType, body));
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