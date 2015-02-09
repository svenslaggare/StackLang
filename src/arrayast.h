#pragma once
#include "ast.h"
#include "statementast.h"

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

//Represents a multidimensional array declaration AST
class MultiDimArrayDeclarationAST : public ExpressionAST {
private:
	std::string mElementType;
	std::vector<std::shared_ptr<ExpressionAST>> mLengthExpressions;

	//Returns the type string
	std::string typeString(int dim = -1) const;
public:
	//Creates a new multidim array declaration AST
	MultiDimArrayDeclarationAST(std::string elementType, std::vector<std::shared_ptr<ExpressionAST>> lengthExpressions);

	//Returns the element type
	std::string elementType() const;

	//Returns the length expressions
	const std::vector<std::shared_ptr<ExpressionAST>>& lengthExpressions() const;

	std::string asString() const override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	//Generates an array of the given dim
	void generateArrayDim(CodeGenerator& codeGen, GeneratedFunction& func, int dim);

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents an array access AST
class ArrayAccessAST : public ExpressionAST {
private:
	std::shared_ptr<ExpressionAST> mArrayRefExpression;
	std::shared_ptr<ExpressionAST> mAccessExpression;
public:
	//Creates a new array access AST
	ArrayAccessAST(std::shared_ptr<ExpressionAST> arrayRefExpression, std::shared_ptr<ExpressionAST> accessExpression);

	//Returns the array reference expression
	std::shared_ptr<ExpressionAST> arrayRefExpression() const;

	//Returns the access expression
	std::shared_ptr<ExpressionAST> accessExpression() const;

	std::string asString() const override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents an array set element AST
class ArraySetElementAST : public ExpressionAST {
private:
	std::shared_ptr<ExpressionAST> mArrayRefExpression;
	std::shared_ptr<ExpressionAST> mAccessExpression;
	std::shared_ptr<ExpressionAST> mRightHandSide;
public:
	//Creates a new array set element AST
	ArraySetElementAST(
		std::shared_ptr<ExpressionAST> arrayRefExpression,
		std::shared_ptr<ExpressionAST> accessExpression,
		std::shared_ptr<ExpressionAST> rightHandSide);

	//Returns the array reference expression
	std::shared_ptr<ExpressionAST> arrayRefExpression() const;

	//Returns the access expression
	std::shared_ptr<ExpressionAST> accessExpression() const;

	//Returns the right hand side expression
	std::shared_ptr<ExpressionAST> rightHandSide() const;

	std::string asString() const override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};