#include "statementast.h"
#include "blockast.h"

//Expresssion statement AST
ExpressionStatementAST::ExpressionStatementAST(std::shared_ptr<ExpressionAST> expression)
	: mExpression(expression) {

}

std::shared_ptr<ExpressionAST> ExpressionStatementAST::expression() const {
	return mExpression;
}

std::string ExpressionStatementAST::asString() const {
	return mExpression->asString() + ";";
}

void ExpressionStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mExpression->generateSymbols(binder, symbolTable);
}

//Return statement AST
ReturnStatementAST::ReturnStatementAST(std::shared_ptr<ExpressionAST> returnExpression)
	: mReturnExpression(returnExpression) {

}

std::shared_ptr<ExpressionAST> ReturnStatementAST::returnExpression() const {
	return mReturnExpression;
}

std::string ReturnStatementAST::asString() const {
	return "return" + (mReturnExpression == nullptr ? "" : " " + mReturnExpression->asString()) + ";";
}

void ReturnStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mReturnExpression->generateSymbols(binder, symbolTable);
}

//If & else statement AST
IfElseStatementAST::IfElseStatementAST(std::shared_ptr<ExpressionAST> conditionExpression, std::shared_ptr<BlockAST> thenBlock, std::shared_ptr<BlockAST> elseBlock)
	: mConditionExpression(conditionExpression), mThenBlock(thenBlock), mElseBlock(elseBlock) {

}

std::shared_ptr<ExpressionAST> IfElseStatementAST::conditionExpression() const {
	return mConditionExpression;
}

std::shared_ptr<BlockAST> IfElseStatementAST::thenBlock() const {
	return mThenBlock;
}

std::shared_ptr<BlockAST> IfElseStatementAST::elseBlock() const {
	return mElseBlock;
}

std::string IfElseStatementAST::asString() const {
	return "if (" + mConditionExpression->asString() + ") " + mThenBlock->asString() + (mElseBlock != nullptr ? " else " + mElseBlock->asString() : "");
}

void IfElseStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	mConditionExpression->generateSymbols(binder, symbolTable);
	mThenBlock->generateSymbols(binder, symbolTable);

	if (mElseBlock != nullptr) {
		mElseBlock->generateSymbols(binder, symbolTable);
	}
}

//For loop statement AST
ForLoopStatementAST::ForLoopStatementAST(
	std::shared_ptr<ExpressionAST> initExpression, std::shared_ptr<ExpressionAST> conditionExpression,
	std::shared_ptr<ExpressionAST> changeExpression, std::shared_ptr<BlockAST> bodyBlock)
	: mConditionExpression(conditionExpression), mInitExpression(initExpression), mChangeExpression(changeExpression), mBodyBlock(bodyBlock) {

}

std::shared_ptr<ExpressionAST> ForLoopStatementAST::initExpression() const {
	return mInitExpression;
}

std::shared_ptr<ExpressionAST> ForLoopStatementAST::conditionExpression() const {
	return mConditionExpression;
}

std::shared_ptr<ExpressionAST> ForLoopStatementAST::changeExpression() const {
	return mChangeExpression;
}

std::shared_ptr<BlockAST> ForLoopStatementAST::bodyBlock() const {
	return mBodyBlock;
}

std::string ForLoopStatementAST::asString() const {
	return "for (" + mInitExpression->asString() + "; " + mConditionExpression->asString() + "; " + mChangeExpression->asString() + ") " + mBodyBlock->asString();
}

void ForLoopStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	mInitExpression->generateSymbols(binder, symbolTable);
	mConditionExpression->generateSymbols(binder, symbolTable);
	mChangeExpression->generateSymbols(binder, symbolTable);
	mBodyBlock->generateSymbols(binder, symbolTable);
}