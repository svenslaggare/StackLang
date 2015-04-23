#include "dotaccessast.h"

DotAccessAST::DotAccessAST(std::shared_ptr<ExpressionAST> leftHandSide, std::shared_ptr<ExpressionAST> rightHandSide)
	: mLeftHandSide(leftHandSide), mRightHandSide(rightHandSide) {

}

std::shared_ptr<ExpressionAST> DotAccessAST::leftHandSide() const {
	return mLeftHandSide;
}

std::shared_ptr<ExpressionAST> DotAccessAST::rightHandSide() const {
	return mRightHandSide;
}

std::string DotAccessAST::asString() const {
	return mLeftHandSide->asString() + "." + mRightHandSide->asString();
}

void DotAccessAST::visit(VisitFn visitFn) const {
	mLeftHandSide->visit(visitFn);
	mRightHandSide->visit(visitFn);
	visitFn(this);
}

bool DotAccessAST::rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST, Compiler& compiler) const {
	if (mSymbolTable != nullptr) {

		return true;
	} else {
		return false;
	}
}

void DotAccessAST::rewrite(Compiler& compiler) {
	std::shared_ptr<AbstractSyntaxTree> newLHS;

	while (mLeftHandSide->rewriteAST(newLHS, compiler)) {
		mLeftHandSide = std::dynamic_pointer_cast<ExpressionAST>(newLHS);
	}

	std::shared_ptr<AbstractSyntaxTree> newRHS;

	while (mRightHandSide->rewriteAST(newRHS, compiler)) {
		mRightHandSide = std::dynamic_pointer_cast<ExpressionAST>(newRHS);
	}

	mLeftHandSide->rewrite(compiler);
	mRightHandSide->rewrite(compiler);
}

void DotAccessAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
}