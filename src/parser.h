#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include "lexer.h"

class ProgramAST;
class FunctionAST;
class BlockAST;
class StatementAST;
class ExpressionAST;

//Represents a parser
class Parser {
private:
	std::vector<Token> tokens;
	Token currentToken;
	int tokenIndex;

	std::map<char, int> binOpPrecedence;
	std::map<std::pair<char, char>, int> twoCharOpPrecedence;
	std::set<char> assignmentOperators;

	//Signals that a compile error has occured
	void compileError(std::string message);

	//Advances to the next token
	Token& nextToken();

	//Returns the next token
	Token& peekToken();

	//Uses the current token as a char
	char currentTokenAsChar(std::string errorMessage = "Expected a single character.");

	//Checks if the current token is the given single character token
	bool isSingleCharToken(char character);

	//Asserts that the current token is the given character
	void assertCurrentTokenAsChar(char character, std::string errorMessage);

	//Returns the precedence for the current token or -1 if not a operator
	int getTokenPrecedence();

	//Parses an integer expression
	std::shared_ptr<ExpressionAST> parseIntegerExpression();

	//Parses an identifier expression
	std::shared_ptr<ExpressionAST> parseIdentifierExpression();

	//Parses a parenthesis expression
	std::shared_ptr<ExpressionAST> parseParenthesisExpression();

	//Parses a primary expression
	std::shared_ptr<ExpressionAST> parsePrimaryExpression();

	//Parses the right hand side of an binary op expression
	std::shared_ptr<ExpressionAST> parseBinaryOpRHS(int exprPrecedence, std::shared_ptr<ExpressionAST> lhs, bool allowEqualAssign = false);

	//Parses a unary expression
	std::shared_ptr<ExpressionAST> parseUnaryExpression();

	//Parses an expression
	std::shared_ptr<ExpressionAST> parseExpression(bool allowEqualAssign = false);

	//Parses a if & else statement
	std::shared_ptr<StatementAST> parseIfElseStatement();

	//Parses a for loop statement
	std::shared_ptr<StatementAST> parseForLoopStatement();

	//Parses a statement
	std::shared_ptr<StatementAST> parseStatement();

	//Parses a block
	std::shared_ptr<BlockAST> parseBlock();

	//Parses a function definition
	std::shared_ptr<FunctionAST> parseFunctionDef();
public:
	//Creates a new parser
	Parser(std::vector<Token> tokens);

	//Parses the tokens
	std::shared_ptr<ProgramAST> parse();
};