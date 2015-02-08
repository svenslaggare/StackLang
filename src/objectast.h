#pragma once
#include "ast.h"
#include <memory>

class Binder;
class SymbolTable;
class TypeChecker;
class CodeGenerator;
class GeneratedFunction;

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

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a namespace access AST
class NamespaceAccessAST : public ExpressionAST {
private:	
	std::shared_ptr<ExpressionAST> mNamespaceExpression;
	std::shared_ptr<ExpressionAST> mMemberExpression;

	//Finds the namespace symbol table
	std::shared_ptr<SymbolTable> findNamespaceTable(Binder& binder, std::shared_ptr<SymbolTable> symbolTable, std::shared_ptr<ExpressionAST> namespaceExpression);

	//Finds the namespace name
	std::string namespaceName(std::shared_ptr<SymbolTable> symbolTable, std::shared_ptr<ExpressionAST> namespaceExpression) const;
public:
	//Creates a new namespace access AST
	NamespaceAccessAST(std::shared_ptr<ExpressionAST> namespaceExpression, std::shared_ptr<ExpressionAST> memberExpression);

	//Returns the namespace expression
	std::shared_ptr<ExpressionAST> namespaceExpression() const;

	//Returns the member expression
	std::shared_ptr<ExpressionAST> memberExpression() const;

	virtual std::string asString() const override;

	virtual void rewrite() override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};