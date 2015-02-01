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

	std::string asString() const override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};