#pragma once
#include "ast.h"

#include <memory>
#include <string>
#include <vector>

class Compiler;

//Represents a namespace declaration AST
class NamespaceDeclarationAST : public AbstractSyntaxTree {
private:
	std::string mName;
	std::vector<std::shared_ptr<AbstractSyntaxTree>> mMembers;
public:
	//Creates a new namespace with the given members
	NamespaceDeclarationAST(std::string mName, std::vector<std::shared_ptr<AbstractSyntaxTree>> members);

	//Returns the name of the namespace
	std::string name() const;

	//Returns the members
	const std::vector<std::shared_ptr<AbstractSyntaxTree>>& members() const;

	virtual std::string type() const override;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual void rewrite(Compiler& compiler) override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;
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

	virtual void visit(VisitFn visitFn) const override;
	
	virtual void rewrite(Compiler& compiler) override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void verify(SemanticVerifier& verifier) override;

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};