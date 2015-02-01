#pragma once
#include "ast.h"

#include <memory>

class TypeChecker;
class Type;

//Represents an array declaration AST
class ArrayDeclarationAST : public ExpressionAST {
private:
	std::string mElementType;
	std::shared_ptr<ExpressionAST> mLengthExpression;
public:
	//Creates a new array declaration AST
	ArrayDeclarationAST(std::string elementType, std::shared_ptr<ExpressionAST> lengthExpression);

	//Returns the element type
	std::string elementType() const;

	//Returns the length expression
	std::shared_ptr<ExpressionAST> lengthExpression() const;

	std::string asString() const override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents an array access AST
class ArrayAccessAST : public ExpressionAST {
private:
	std::string mArrayName;
	std::shared_ptr<ExpressionAST> mAccessExpression;
public:
	//Creates a new array access AST
	ArrayAccessAST(std::string arrayName, std::shared_ptr<ExpressionAST> accessExpression);

	//Returns the array name
	std::string arrayName() const;

	//Returns the access expression
	std::shared_ptr<ExpressionAST> accessExpression() const;

	std::string asString() const override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};