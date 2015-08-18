#pragma once
#include "ast.h"
#include <memory>
#include <string>

class Compiler;
class TypeChecker;
class CodeGenerator;
class Symbol;
class SymbolTable;
class VariableSymbol;
class FunctionSignatureSymbol;
class FunctionSymbol;
class ClassType;

//Represents a call expression
class CallExpressionAST : public ExpressionAST {
private:
	std::string mFunctionName;
	std::vector<std::shared_ptr<ExpressionAST>> mArguments;
	std::shared_ptr<FunctionSymbol> mFuncSymbol;

	std::shared_ptr<SymbolTable> mCallTable;

	//Returns the call table
	std::shared_ptr<SymbolTable> callTable() const;
public:
	//Creates a new function call expression
	CallExpressionAST(std::string functionName, std::vector<std::shared_ptr<ExpressionAST>> arguments);

	//Returns the name of the function to call
	std::string functionName() const;

	//Returns the arguments to call with
	const std::vector<std::shared_ptr<ExpressionAST>>& arguments() const;

	//Sets the call table where to look for functions. The default is the symbol table for the tree.
	void setCallTable(std::shared_ptr<SymbolTable> callTable);

	//Finds the func signature symbol
	std::shared_ptr<FunctionSignatureSymbol> funcSignature(const TypeChecker& typeChecker) const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;
	
	virtual void rewrite(Compiler& compiler) override;

	virtual bool rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST, Compiler& compiler) const override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	//Generates code in the given namespace
	void generateCode(CodeGenerator& codeGen, GeneratedFunction& func, std::string scopeName);

	//Generates code for a member call
	void generateMemberCallCode(CodeGenerator& codeGen, GeneratedFunction& func, std::shared_ptr<ClassType> classType);

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};