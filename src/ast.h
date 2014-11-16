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

};

//Represents a statement AST
class StatementAST : public AbstractSyntaxTree {

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

class VariableDeclerationExpressionAST;

//Represents a function AST
class FunctionAST : public AbstractSyntaxTree {
private:
	std::string mName;
	std::vector<std::shared_ptr<VariableDeclerationExpressionAST>> mArguments;
	std::string mReturnType;
	std::shared_ptr<BlockAST> mBody;
public:
	//Creates a new function
	FunctionAST(std::string name, const std::vector<std::shared_ptr<VariableDeclerationExpressionAST>>& arguments, std::string returnType, std::shared_ptr<BlockAST> body);

	//Returns the name
	const std::string name() const;

	//Returns the arguments
	const std::vector<std::shared_ptr<VariableDeclerationExpressionAST>>& arguments() const;

	//Returns the type
	const std::string returnType() const;

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

//Represents an if and else statement
class IfElseStatementAST : public StatementAST {
private:
	std::shared_ptr<ExpressionAST> mConditionExpression;
	std::shared_ptr<BlockAST> mThenBlock;
	std::shared_ptr<BlockAST> mElseBlock;
public:
	//Creates a new if and else statement
	IfElseStatementAST(std::shared_ptr<ExpressionAST> conditionExpression, std::shared_ptr<BlockAST> thenBlock, std::shared_ptr<BlockAST> elseBlock);

	//Returns the condition expression
	std::shared_ptr<ExpressionAST> conditionExpression() const;

	//Returns the then block
	std::shared_ptr<BlockAST> thenBlock() const;

	//Returns the else block
	std::shared_ptr<BlockAST> elseBlock() const;

	std::string asString() const override;
};

//Represents a for loop statement
class ForLoopStatementAST : public StatementAST {
private:
	std::shared_ptr<ExpressionAST> mInitExpression;
	std::shared_ptr<ExpressionAST> mConditionExpression;
	std::shared_ptr<ExpressionAST> mChangeExpression;
	std::shared_ptr<BlockAST> mBodyBlock;
public:
	//Creates a new if and else statement
	ForLoopStatementAST(
		std::shared_ptr<ExpressionAST> initExpression, std::shared_ptr<ExpressionAST> conditionExpression,
		std::shared_ptr<ExpressionAST> changeExpression, std::shared_ptr<BlockAST> bodyBlock);

	//Returns the init expression
	std::shared_ptr<ExpressionAST> initExpression() const;

	//Returns the condition expression
	std::shared_ptr<ExpressionAST> conditionExpression() const;

	//Returns the change expression
	std::shared_ptr<ExpressionAST> changeExpression() const;

	//Returns the body block
	std::shared_ptr<BlockAST> bodyBlock() const;

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

//Represents a variable decleration expression
class VariableDeclerationExpressionAST : public ExpressionAST {
private:
	std::string mVarType;
	std::string mVarName;
public:
	//Creates a new variable decleration expression
	VariableDeclerationExpressionAST(std::string varType, std::string varName);

	//Returns the type of the varaible
	std::string varType() const;

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

//Represents an operator
class Operator {
private:
	bool mIsTwoChars;
	char mOp1;
	char mOp2;
public:
	//Creates a new single-character operator
	explicit Operator(char op1);

	//Creates a new two-characters operator
	Operator(char op1, char op2);

	//Indicates if the current operator consists of two characters
	bool isTwoChars() const;

	//Returns the first op character
	char op1() const;

	//Returns the second op character. If not a two-character operator the functions throws an exception.
	char op2() const;

	bool operator==(const Operator& rhs) const;
	bool operator!=(const Operator& rhs) const;

	//Returns the current operator as a string
	std::string asString() const;
};

//Represents a binary operation expression
class BinaryOpExpressionAST : public ExpressionAST {
private:
	std::shared_ptr<ExpressionAST> mLeftHandSide;
	std::shared_ptr<ExpressionAST> mRightHandSide;
	Operator mOp;
public:
	//Creates a new binary operator expression
	BinaryOpExpressionAST(std::shared_ptr<ExpressionAST> leftHandSide, std::shared_ptr<ExpressionAST> rightHandSide, Operator op);

	//Returns the left hand side
	std::shared_ptr<ExpressionAST> leftHandSide() const;

	//Returns the right hand side
	std::shared_ptr<ExpressionAST> rightHandSide() const;

	//Returns the operator
	Operator op() const;

	std::string asString() const override;
};

//Represents a unary operator expression 
class UnaryOpExpressionAST : public ExpressionAST {
private:
	std::shared_ptr<ExpressionAST> mOperand;
	Operator mOp;
public:
	//Creates a new unary operator expression
	UnaryOpExpressionAST(std::shared_ptr<ExpressionAST> operand, Operator op);

	//Returns operand
	std::shared_ptr<ExpressionAST> operand() const;

	//Returns the operator
	Operator op() const;

	std::string asString() const override;
};