#include "statementast.h"
#include "blockast.h"
#include "typechecker.h"
#include "symboltable.h"
#include "asthelpers.h"
#include "codegenerator.h"

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

void ExpressionStatementAST::typeCheck(TypeChecker& checker) {
	mExpression->typeCheck(checker);
}

std::shared_ptr<AbstractSyntaxTree> ExpressionStatementAST::findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const {
	if (predicate(mExpression)) {
		return mExpression;
	}

	return mExpression->findAST(predicate);
}

void ExpressionStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mExpression->generateCode(codeGen, func);
}

//Return statement AST
ReturnStatementAST::ReturnStatementAST(std::shared_ptr<ExpressionAST> returnExpression)
	: mReturnExpression(returnExpression) {

}

ReturnStatementAST::ReturnStatementAST()
	: mReturnExpression(nullptr) {

}

std::shared_ptr<ExpressionAST> ReturnStatementAST::returnExpression() const {
	return mReturnExpression;
}

std::string ReturnStatementAST::asString() const {
	return "return" + (mReturnExpression == nullptr ? "" : " " + mReturnExpression->asString()) + ";";
}

std::shared_ptr<AbstractSyntaxTree> ReturnStatementAST::findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const {
	if (mReturnExpression == nullptr) {
		return nullptr;
	}

	if (predicate(mReturnExpression)) {
		return mReturnExpression;
	}

	return mReturnExpression->findAST(predicate);
}

void ReturnStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	if (mReturnExpression != nullptr) {
		mReturnExpression->generateSymbols(binder, symbolTable);
	}
}

void ReturnStatementAST::typeCheck(TypeChecker& checker) {
	if (mReturnExpression != nullptr) {
		mReturnExpression->typeCheck(checker);
	}
}

void ReturnStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mReturnExpression->generateCode(codeGen, func);

	func.addInstruction("RET");
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

std::shared_ptr<AbstractSyntaxTree> IfElseStatementAST::findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const {
	std::shared_ptr<AbstractSyntaxTree> result;

	if (ASTHelpers::findAST(mConditionExpression, predicate, result)
		|| ASTHelpers::findAST(mThenBlock, predicate, result)
		|| (mElseBlock != nullptr && ASTHelpers::findAST(mElseBlock, predicate, result))) {
		return result;
	}

	return nullptr;
}

void IfElseStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	mConditionExpression->generateSymbols(binder, symbolTable);
	mThenBlock->generateSymbols(binder, symbolTable);

	if (mElseBlock != nullptr) {
		mElseBlock->generateSymbols(binder, symbolTable);
	}
}

void IfElseStatementAST::typeCheck(TypeChecker& checker) {
	mConditionExpression->typeCheck(checker);

	checker.assertSameType(*checker.getType("Bool"), *mConditionExpression->expressionType(checker), mConditionExpression->asString());

	mThenBlock->typeCheck(checker);

	if (mElseBlock != nullptr) {
		mElseBlock->typeCheck(checker);
	}
}

void IfElseStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
}

//While loop statement AST
WhileLoopStatementAST::WhileLoopStatementAST(std::shared_ptr<ExpressionAST> conditionExpression, std::shared_ptr<BlockAST> bodyBlock)
	: mConditionExpression(conditionExpression), mBodyBlock(bodyBlock) {

}

std::shared_ptr<ExpressionAST> WhileLoopStatementAST::conditionExpression() const {
	return mConditionExpression;
}

std::shared_ptr<BlockAST> WhileLoopStatementAST::bodyBlock() const {
	return mBodyBlock;
}

std::string WhileLoopStatementAST::asString() const {
	return "while (" + mConditionExpression->asString() + ") " + mBodyBlock->asString();
}

std::shared_ptr<AbstractSyntaxTree> WhileLoopStatementAST::findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const {
	if (predicate(mConditionExpression)) {
		return mConditionExpression;
	} else {
		if (auto ast = mConditionExpression->findAST(predicate)) {
			return ast;
		}
	}

	if (predicate(mBodyBlock)) {
		return mBodyBlock;
	} else {
		if (auto ast = mBodyBlock->findAST(predicate)) {
			return ast;
		}
	}

	return nullptr;
}

void WhileLoopStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	mConditionExpression->generateSymbols(binder, symbolTable);;
	mBodyBlock->generateSymbols(binder, symbolTable);
}

void WhileLoopStatementAST::typeCheck(TypeChecker& checker) {
	mConditionExpression->typeCheck(checker);
	mBodyBlock->typeCheck(checker);
}

void WhileLoopStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {

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

std::shared_ptr<AbstractSyntaxTree> ForLoopStatementAST::findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const {
	std::shared_ptr<AbstractSyntaxTree> result;

	if (ASTHelpers::findAST(mInitExpression, predicate, result)
		|| ASTHelpers::findAST(mConditionExpression, predicate, result)
		|| ASTHelpers::findAST(mChangeExpression, predicate, result)
		|| ASTHelpers::findAST(mBodyBlock, predicate, result)) {
		return result;
	}

	return nullptr;
}

void ForLoopStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	auto inner = SymbolTable::newInner(symbolTable);

	mInitExpression->generateSymbols(binder, inner);
	mConditionExpression->generateSymbols(binder, inner);
	mChangeExpression->generateSymbols(binder, inner);
	mBodyBlock->generateSymbols(binder, inner);
}

void ForLoopStatementAST::typeCheck(TypeChecker& checker) {
	mInitExpression->typeCheck(checker);
	mConditionExpression->typeCheck(checker);
	mChangeExpression->typeCheck(checker);
	mBodyBlock->typeCheck(checker);
}

void ForLoopStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	
}