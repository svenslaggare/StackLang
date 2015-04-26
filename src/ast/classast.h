#pragma once
#include "ast.h"
#include "../symbol.h"
#include <vector>
#include <memory>

class FunctionAST;
class Type;
class Compiler;
class Binder;
class SymbolTable;
class TypeChecker;
class CodeGenerator;
class GeneratedFunction;
class Symbol;
class FunctionSignatureSymbol;

//Represents a field declaration expression AST
class FieldDeclarationExpressionAST : public ExpressionAST {
private:
	std::string mFieldType;
	std::string mFieldName;
public:
	//Creates a new field declaration expression
	FieldDeclarationExpressionAST( std::string fieldType, std::string fieldName);

	//Returns the type of the field
	std::string fieldType() const;

	//Returns the name of the field
	std::string fieldName() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a class definition AST
class ClassDefinitionAST : public AbstractSyntaxTree {
private:
	std::string mName;
	std::vector<std::shared_ptr<FieldDeclarationExpressionAST>> mFields;
	std::vector<std::shared_ptr<FunctionAST>> mFunctions;

	//Finds the namespace name for the current class
	std::string findNamespaceName(std::shared_ptr<SymbolTable> symbolTable, std::string sep) const;
public:
	//Creates a new class definition using the given fields and functions
	ClassDefinitionAST(
		std::string name,
		std::vector<std::shared_ptr<FieldDeclarationExpressionAST>> fields,
		std::vector<std::shared_ptr<FunctionAST>> functions);

	//Returns the name of the class
	std::string name() const;

	//Returns the fields
	const std::vector<std::shared_ptr<FieldDeclarationExpressionAST>>& fields() const;

	//Returns the member functions
	const std::vector<std::shared_ptr<FunctionAST>>& functions() const;

	//Returns the full name
	std::string fullName(std::string namespaceSep = "::") const;

	virtual std::string asString() const override;

	//Adds the class definition to the given type checker
	void addClassDefinition(TypeChecker& checker) const;

	virtual void rewrite(Compiler& compiler) override;

	virtual void visit(VisitFn visitFn) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a new class expression AST
class NewClassExpressionAST : public ExpressionAST {
private:
	std::string mTypeName;
	std::vector<std::shared_ptr<ExpressionAST>> mConstructorArguments;
	std::shared_ptr<ClassSymbol> mClassSymbol;
public:
	//Creates a new new class expression AST
	NewClassExpressionAST(std::string typeName, std::vector<std::shared_ptr<ExpressionAST>> constructorArguments);

	//Returns the type name
	std::string typeName() const;

	//Returns the constructor arguments
	const std::vector<std::shared_ptr<ExpressionAST>>& constructorArguments() const;

	virtual std::string asString() const override;
	
	//Finds the symbol for the constructor
	std::shared_ptr<Symbol> constructorSymbol(std::shared_ptr<SymbolTable> symbolTable) const;

	//Returns the signature for the constructor
	std::shared_ptr<FunctionSignatureSymbol> constructorSignature(const TypeChecker& typeChecker) const;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};