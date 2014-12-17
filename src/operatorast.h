#pragma once
#include <memory>
#include <set>
#include <map>

#include "ast.h"

class TypeChecker;
class Type;
class CodeGenerator;

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

	bool operator<(const Operator& rhs) const;
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
	static const std::map<Operator, std::shared_ptr<Type>> mBoolTypes;
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

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
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
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};