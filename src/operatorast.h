#pragma once
#include <memory>
#include <set>
#include <map>

#include "ast.h"
#include "operators.h"

class TypeChecker;
class Type;
class CodeGenerator;

//Represents a binary operation expression
class BinaryOpExpressionAST : public ExpressionAST {
private:
	std::shared_ptr<ExpressionAST> mLeftHandSide;
	std::shared_ptr<ExpressionAST> mRightHandSide;
	Operator mOp;

	static std::set<std::string> arithmeticTypes;
	static std::set<std::string> equalityTypes;
	static std::set<std::string> comparableTypes;
	static std::set<std::string> logicalTypes;

	//Indicates if the left hand side is convertable to float
	bool lhsFloatConvertable(const TypeChecker& typeChecker) const;

	//Indicates if the right hand side is convertable to float
	bool rhsFloatConvertable(const TypeChecker& typeChecker) const;

	//Generates code for the rhs
	void generateRHSCode(CodeGenerator& codeGen, GeneratedFunction& func);

	//Generates the code for lhs and rhs
	void generateSidesCode(CodeGenerator& codeGen, GeneratedFunction& func);
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

	virtual void rewrite() override;

	virtual bool rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;

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
	
	virtual bool rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};