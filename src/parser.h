#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include "lexer.h"

class OperatorContainer;
class ProgramAST;
class FunctionAST;
class BlockAST;
class StatementAST;
class ExpressionAST;
class NamespaceDeclarationAST;

//Represents a parser
class Parser {
private:
	std::vector<Token> tokens;
	Token currentToken;
	int tokenIndex;

	int mCurrentLineNumber = 1;

	const OperatorContainer& operators;

	//Signals that a compile error has occured
	void compileError(std::string message);

	//Advances to the next token
	Token& nextToken();

	//Returns the next token
	Token& peekToken(int delta = 1);

	//Uses the current token as a char
	char currentTokenAsChar(std::string errorMessage = "Expected a single character.");

	//Checks if the current token is the given single character token
	bool isSingleCharToken(char character);

	//Asserts that the current token is the given character
	void assertCurrentTokenAsChar(char character, std::string errorMessage);

	//Returns the precedence for the current token or -1 if not a operator
	int getTokenPrecedence();

	//Parses a type name
	std::string parseTypeName();

	//Parses an integer expression
	std::shared_ptr<ExpressionAST> parseIntegerExpression();

	//Parses a bool expression
	std::shared_ptr<ExpressionAST> parseBoolExpression();

	//Parses a float expression
	std::shared_ptr<ExpressionAST> parseFloatExpression();

	//Parses a null ref expression
	std::shared_ptr<ExpressionAST> parseNullRefExpression();

	//Parses a cast expression
	std::shared_ptr<ExpressionAST> parseCastExpression();

	//Parses an identifier expression
	std::shared_ptr<ExpressionAST> parseIdentifierExpression(bool allowDeclaration = false);

	//Parses a parenthesis expression
	std::shared_ptr<ExpressionAST> parseParenthesisExpression();

	//Parses an array declaration
	std::shared_ptr<ExpressionAST> parseArrayDeclaration();

	//Parses a primary expression
	std::shared_ptr<ExpressionAST> parsePrimaryExpression(bool allowDeclaration = false);

	//Parses the right hand side of an binary op expression
	std::shared_ptr<ExpressionAST> parseBinaryOpRHS(int exprPrecedence, std::shared_ptr<ExpressionAST> lhs, bool allowEqualAssign = false);

	//Parses a unary expression
	std::shared_ptr<ExpressionAST> parseUnaryExpression(bool allowDeclaration = false);

	//Parses an expression
	std::shared_ptr<ExpressionAST> parseExpression(bool allowEqualAssign = false);

	//Parses a if & else statement
	std::shared_ptr<StatementAST> parseIfElseStatement();

	//Parses a while loop statement
	std::shared_ptr<StatementAST> parseWhileLoopStatement();

	//Parses a for loop statement
	std::shared_ptr<StatementAST> parseForLoopStatement();

	//Parses a statement
	std::shared_ptr<StatementAST> parseStatement();

	//Parses a block
	std::shared_ptr<BlockAST> parseBlock();

	//Parses a function definition
	std::shared_ptr<FunctionAST> parseFunctionDef();

	//Parses a namespace declaration
	std::shared_ptr<NamespaceDeclarationAST> parseNamespaceDef();
public:
	//Creates a new parser
	Parser(const OperatorContainer& operators, std::vector<Token> tokens);

	//Parses the tokens
	std::shared_ptr<ProgramAST> parse();
};