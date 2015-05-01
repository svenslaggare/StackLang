#pragma once
#include "ast.h"

#include <memory>
#include <string>

class Compiler;
class TypeChecker;
class CodeGenerator;
class SymbolTable;
class VariableSymbol;
class TypeName;

//Represents a variable reference expression
class VariableReferenceExpressionAST : public ExpressionAST {
private:
	std::string mName;
public:
	//Creates a new variable reference expression
	VariableReferenceExpressionAST(std::string name);

	//Returns the name of the variable
	std::string name() const;

	std::string asString() const override;

	//Returns the symbol that the variable references to. Returns null if not bound.
	std::shared_ptr<VariableSymbol> symbol() const;

	virtual void visit(VisitFn visitFn) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a variable declaration expression
class VariableDeclarationExpressionAST : public ExpressionAST {
private:
	std::unique_ptr<TypeName> mType;
	std::string mName;
	bool mIsFunctionParameter;
public:
	//Creates a new variable declaration expression
	VariableDeclarationExpressionAST(std::string type, std::string name, bool isFunctionParameter = false);

	//Returns the type of the variable
	std::string type() const;

	//Returns the name of the variable
	std::string name() const;

	//Indicates if the decleration is of a function parameter
	bool isFunctionParameter() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};