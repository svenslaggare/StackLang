#pragma once
#include <vector>
#include <memory>
#include <iostream>

//Represents an abstract syntax tree
class AbstractSyntaxTree {
public:
	virtual ~AbstractSyntaxTree() {};

	//Returns the current AST as a string
	virtual std::string asString() const = 0;
};

std::ostream& operator<<(std::ostream& os, const AbstractSyntaxTree& ast);

class FunctionAST;

//Represents an program AST
class ProgramAST : public AbstractSyntaxTree {
private:
	std::vector<std::shared_ptr<FunctionAST>> mFunctions;
public:
	ProgramAST(const std::vector<std::shared_ptr<FunctionAST>>& functions);

	//Returns the functions
	const std::vector<std::shared_ptr<FunctionAST>>& functions() const;

	std::string asString() const override;
};

//Represents an expression AST
class ExpressionAST : public AbstractSyntaxTree {
public:
	std::string asString() const override { return ""; }
};

//Represents a statement AST
class StatementAST : public AbstractSyntaxTree {
public:
	std::string asString() const override { return ""; }
};

//Represents a block AST
class BlockAST : public AbstractSyntaxTree {
private:
	std::vector<std::shared_ptr<StatementAST>> mStatements;
public:
	//Creates a new block
	BlockAST(const std::vector<std::shared_ptr<StatementAST>>& statements);

	//Returns the statements
	const std::vector<std::shared_ptr<StatementAST>>& statements() const;

	std::string asString() const override;
};

//Represents a function AST
class FunctionAST : public AbstractSyntaxTree {
private:
	std::string mName;
	std::vector<std::string> mArguments;
	std::shared_ptr<BlockAST> mBody;
public:
	//Creates a new function
	FunctionAST(std::string name, const std::vector<std::string>& arguments, std::shared_ptr<BlockAST> body);

	//Returns the name
	const std::string name() const;

	//Returns the arguments
	const std::vector<std::string>& arguments() const;

	//Returns the body
	std::shared_ptr<BlockAST> body() const;

	std::string asString() const override;
};

class BinaryOpExpressionAST;

//Represents an expression statement
class ExpressionStatementAST : public StatementAST {
private:
	std::shared_ptr<ExpressionAST> mExpression;
public:
	//Creates an expression statement
	ExpressionStatementAST(std::shared_ptr<ExpressionAST> expression);

	//Returns the expression
	std::shared_ptr<ExpressionAST> expression() const;

	std::string asString() const override;
};

//Represents a return statement
class ReturnStatementAST : public StatementAST {
private:
	std::shared_ptr<ExpressionAST> mReturnExpression;
public:
	//Creates a new return statement
	ReturnStatementAST(std::shared_ptr<ExpressionAST> returnExpression);

	//Returns the return expression
	std::shared_ptr<ExpressionAST> returnExpression() const;

	std::string asString() const override;
};

//Represents an integer expression
class IntegerExpressionAST : public ExpressionAST {
private:
	int mValue;
public:
	//Creates a new integer expression
	IntegerExpressionAST(int value);

	//Returns the value
	int value() const;

	std::string asString() const override;
};

//Represents a variable reference expression
class VariableReferenceExpressionAST : public ExpressionAST {
private:
	std::string mVarName;
public:
	//Creates a new variable reference expression
	VariableReferenceExpressionAST(std::string varName);

	//Returns the name of the varaible
	std::string varName() const;

	std::string asString() const override;
};

//Represents a call expression
class CallExpressionAST : public ExpressionAST {
private:
	std::string mFunctionName;
	std::vector<std::shared_ptr<ExpressionAST>> mArguments;
public:
	//Creates a new function call expression
	CallExpressionAST(std::string functionName, std::vector<std::shared_ptr<ExpressionAST>> arguments);

	//Returns the name of the function to call
	std::string functionName() const;

	//Returns the arguments to call with
	std::vector<std::shared_ptr<ExpressionAST>> arguments() const;

	std::string asString() const override;
};

//Represents a binary operation expression
class BinaryOpExpressionAST : public ExpressionAST {
private:
	std::shared_ptr<ExpressionAST> mLeftHandSide;
	std::shared_ptr<ExpressionAST> mRightHandSide;
	char mOpChar;
public:
	//Creates a new binary operator expression
	BinaryOpExpressionAST(std::shared_ptr<ExpressionAST> leftHandSide, std::shared_ptr<ExpressionAST> rightHandSide, char opChar);

	//Returns the left hand side
	std::shared_ptr<ExpressionAST> leftHandSide() const;

	//Returns the right hand side
	std::shared_ptr<ExpressionAST> rightHandSide() const;

	//Returns the operator character
	char opChar() const;

	std::string asString() const override;
};

//Represents a unary operator expression 
class UnaryOpExpressionAST : public ExpressionAST {
private:
	std::shared_ptr<ExpressionAST> mOperand;
	char mOpChar;
public:
	//Creates a new unary operator expression
	UnaryOpExpressionAST(std::shared_ptr<ExpressionAST> operand, char opChar);

	//Returns operand
	std::shared_ptr<ExpressionAST> operand() const;

	//Returns the operator character
	char opChar() const;

	std::string asString() const override;
};