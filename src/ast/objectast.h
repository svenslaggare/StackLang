#pragma once
#include "ast.h"
#include <memory>

class Binder;
class SymbolTable;
class TypeChecker;
class CodeGenerator;
class GeneratedFunction;
class SemanticVerifier;

//Represents a member access AST
class MemberAccessAST : public ExpressionAST {
private:	
	std::shared_ptr<ExpressionAST> mAccessExpression;
	std::shared_ptr<ExpressionAST> mMemberExpression;
public:
	//Creates a new member access AST
	MemberAccessAST(std::shared_ptr<ExpressionAST> accessExpression, std::shared_ptr<ExpressionAST> memberExpression);

	//Returns the access expression
	std::shared_ptr<ExpressionAST> accessExpression() const;

	//Returns the member expression
	std::shared_ptr<ExpressionAST> memberExpression() const;

	virtual std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual void rewrite() override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a set field value AST
class SetFieldValueAST : public ExpressionAST {
private:	
	std::shared_ptr<ExpressionAST> mObjectRefExpression;
	std::shared_ptr<ExpressionAST> mMemberExpression;
	std::shared_ptr<ExpressionAST> mRightHandSide;
public:
	//Creates a new set field value AST
	SetFieldValueAST(
		std::shared_ptr<ExpressionAST> objectRefExpression,
		std::shared_ptr<ExpressionAST> memberExpression,
		std::shared_ptr<ExpressionAST> rightHandSide);

	//Returns the object reference expression
	std::shared_ptr<ExpressionAST> objectRefExpression() const;

	//Returns the member expression
	std::shared_ptr<ExpressionAST> memberExpression() const;

	//Returns the right hand side
	std::shared_ptr<ExpressionAST> rightHandSide() const;

	virtual std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual void rewrite() override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};