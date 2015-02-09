#pragma once
#include "ast.h"

#include <memory>
#include <string>
#include <vector>
#include <functional>

class Binder;
class SymbolTable;
class TypeChecker;
class NamespaceDeclarationAST;
class FunctionAST;

using VisitFn = std::function<void(std::string, std::shared_ptr<FunctionAST>)>;

//Represents a program AST
class ProgramAST : public AbstractSyntaxTree {
private:
	std::vector<std::shared_ptr<NamespaceDeclarationAST>> mNamespaces;

	//Visits all the functions in given namespace
	void visitFunctions(VisitFn visitFn, std::shared_ptr<NamespaceDeclarationAST> currentNamespace, std::string outerNamespaceName = "") const;
public:
	ProgramAST(const std::vector<std::shared_ptr<NamespaceDeclarationAST>>& namespaces);

	//Returns the namespaces
	const std::vector<std::shared_ptr<NamespaceDeclarationAST>>& namespaces() const;

	//Visits all the functions in the program
	void visitFunctions(VisitFn visitFn) const;

	virtual std::string type() const override;

	std::string asString() const override;

	virtual void rewrite() override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;
};