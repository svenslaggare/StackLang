#pragma once
#include <memory>
#include <string>

#include "ast.h"

class TypeChecker;
class CodeGenerator;

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

//Represents a call expression
class CallExpressionAST : public ExpressionAST {
private:
	std::string mFunctionName;
	std::vector<std::shared_ptr<ExpressionAST>> mArguments;
public:
	//Creates a new function call expression
	CallExpressionAST(std::string functionName, std::vector<std::shared_ptr<ExpressionAST>> arguments);

	//Returns the name of the function to call
	std::string functionName() const;

	//Returns the arguments to call with
	const std::vector<std::shared_ptr<ExpressionAST>>& arguments() const;

	std::string asString() const override;

	virtual void rewrite() override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};