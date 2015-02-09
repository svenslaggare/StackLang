#pragma once
#include <memory>
#include <string>

#include "ast.h"

class TypeChecker;
class CodeGenerator;
class Symbol;
class SymbolTable;

//Represents an integer expression
class IntegerExpressionAST : public ExpressionAST {
private:
	int mValue;
public:
	//Creates a new integer expression
	IntegerExpressionAST(int value);

	//Returns the value
	int value() const;

	std::string asString() const override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a bool expression
class BoolExpressionAST : public ExpressionAST {
private:
	bool mValue;
public:
	//Creates a new integer expression
	BoolExpressionAST(bool value);

	//Returns the value
	bool value() const;

	std::string asString() const override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a float expression
class FloatExpressionAST : public ExpressionAST {
private:
	float mValue;
public:
	//Creates a new integer expression
	FloatExpressionAST(float value);

	//Returns the value
	float value() const;

	std::string asString() const override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a null reference expression
class NullRefExpressionAST : public ExpressionAST {
public:
	//Creates a new null ref expression
	NullRefExpressionAST();

	std::string asString() const override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a variable reference expression
class VariableReferenceExpressionAST : public ExpressionAST {
private:
	std::string mVarName;
public:
	//Creates a new variable reference expression
	VariableReferenceExpressionAST(std::string varName);

	//Returns the name of the varaible
	std::string varName() const;

	std::string asString() const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a variable declaration expression
class VariableDeclarationExpressionAST : public ExpressionAST, public std::enable_shared_from_this<VariableDeclarationExpressionAST> {
private:
	std::string mVarType;
	std::string mVarName;
	bool mIsFunctionParameter;
public:
	//Creates a new variable declaration expression
	VariableDeclarationExpressionAST(std::string varType, std::string varName, bool isFunctionParameter = false);

	//Returns the type of the varaible
	std::string varType() const;

	//Returns the name of the varaible
	std::string varName() const;

	//Indicates if the decleration is of a function parameter
	bool isFunctionParameter() const;

	virtual std::string type() const override;

	std::string asString() const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

class FunctionSignatureSymbol;

//Represents a call expression
class CallExpressionAST : public ExpressionAST {
private:
	std::string mFunctionName;
	std::vector<std::shared_ptr<ExpressionAST>> mArguments;

	std::shared_ptr<SymbolTable> mCallTable;

	//Returns the func symbol
	std::shared_ptr<Symbol> funcSymbol(std::shared_ptr<SymbolTable> symbolTable) const;

	//Finds the func signature symbol
	std::shared_ptr<FunctionSignatureSymbol> funcSignature(const TypeChecker& typeChecker) const;

	//Returns the call table
	std::shared_ptr<SymbolTable> callTable() const;
public:
	//Creates a new function call expression
	CallExpressionAST(std::string functionName, std::vector<std::shared_ptr<ExpressionAST>> arguments);

	//Returns the name of the function to call
	std::string functionName() const;

	//Returns the arguments to call with
	const std::vector<std::shared_ptr<ExpressionAST>>& arguments() const;

	//Sets the call table where to look for functions. The default is the symbol table.
	void setCallTable(std::shared_ptr<SymbolTable> callTable);

	std::string asString() const override;

	virtual void rewrite() override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	//Generates code in the given namespace
	void generateCode(CodeGenerator& codeGen, GeneratedFunction& func, std::string namespaceName);

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};