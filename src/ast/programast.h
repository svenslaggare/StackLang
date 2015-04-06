#pragma once
#include "ast.h"

#include <memory>
#include <string>
#include <vector>
#include <functional>

class Compiler;
class Binder;
class SymbolTable;
class TypeChecker;
class NamespaceDeclarationAST;
class FunctionAST;
class ClassDefinitionAST;

using VisitFunctionsFn = std::function<void(std::shared_ptr<FunctionAST>)>;
using VisitClassesFn = std::function<void(std::shared_ptr<ClassDefinitionAST>)>;

//Represents a program AST
class ProgramAST : public AbstractSyntaxTree {
private:
	std::vector<std::shared_ptr<NamespaceDeclarationAST>> mNamespaces;

	//Visits all the functions in given namespace
	void visitFunctions(VisitFunctionsFn visitFn, std::shared_ptr<NamespaceDeclarationAST> currentNamespace) const;

	//Visits all the classes in the given namespace
	void visitClasses(VisitClassesFn visitFn, std::shared_ptr<NamespaceDeclarationAST> currentNamespace) const;
public:
	ProgramAST(const std::vector<std::shared_ptr<NamespaceDeclarationAST>>& namespaces);

	//Returns the namespaces
	const std::vector<std::shared_ptr<NamespaceDeclarationAST>>& namespaces() const;

	//Visits all the functions in the program
	void visitFunctions(VisitFunctionsFn visitFn) const;

	//Visits all the classes in the program
	void visitClasses(VisitClassesFn visitFn) const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual void rewrite(Compiler& compiler) override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;
};