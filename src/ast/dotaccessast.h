#pragma once
#include "ast.h"
#include <memory>

class Compiler;
class Binder;
class SymbolTable;

//Represents a dot access AST
class DotAccessAST : public ExpressionAST {
private:	
	std::shared_ptr<ExpressionAST> mLeftHandSide;
	std::shared_ptr<ExpressionAST> mRightHandSide;
public:
	//Creates a new dot access AST
	DotAccessAST(std::shared_ptr<ExpressionAST> leftHandSide, std::shared_ptr<ExpressionAST> rightHandSide);

	//Returns the left hand side
	std::shared_ptr<ExpressionAST> leftHandSide() const;

	//Returns the right hand side
	std::shared_ptr<ExpressionAST> rightHandSide() const;

	virtual std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual bool rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST, Compiler& compiler) const override; 

	virtual void rewrite(Compiler& compiler) override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;
};