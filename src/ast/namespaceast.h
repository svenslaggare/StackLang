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

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual void rewrite(Compiler& compiler) override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;
};

//Represents a using namespace expression AST
class UsingNamespaceExpressionAST : public ExpressionAST {
private:
	std::string mNamespace;
public:
	//Creates a new using namespace expression
	UsingNamespaceExpressionAST(std::string namespaceName);

	//Returns the name of the namespace
	std::string namespaceName() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;
};