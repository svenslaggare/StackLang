#include "statementast.h"
#include "blockast.h"
#include "operatorast.h"
#include "typechecker.h"
#include "symboltable.h"
#include "type.h"
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

void ExpressionStatementAST::visit(VisitFn visitFn) const {
	mExpression->visit(visitFn);
	visitFn(this);
}

void ExpressionStatementAST::rewrite() {
	std::shared_ptr<AbstractSyntaxTree> newAST;

	if (mExpression->rewriteAST(newAST)) {
		mExpression = std::dynamic_pointer_cast<ExpressionAST>(newAST);
	}

	mExpression->rewrite();
}

void ExpressionStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mExpression->generateSymbols(binder, symbolTable);
}

void ExpressionStatementAST::typeCheck(TypeChecker& checker) {
	mExpression->typeCheck(checker);
}

void ExpressionStatementAST::verify(SemanticVerifier& verifier) {
	mExpression->verify(verifier);
}

void ExpressionStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mExpression->generateCode(codeGen, func);

	//If the return type is not void, pop the top value
	if (mExpression->expressionType(codeGen.typeChecker())->name() != "Void") {
		func.addInstruction("POP");
	}
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

void ReturnStatementAST::visit(VisitFn visitFn) const {
	if (mReturnExpression != nullptr) {
		mReturnExpression->visit(visitFn);
	}

	visitFn(this);
}

void ReturnStatementAST::rewrite() {
	std::shared_ptr<AbstractSyntaxTree> newAST;

	if (mReturnExpression != nullptr && mReturnExpression->rewriteAST(newAST)) {
		mReturnExpression = std::dynamic_pointer_cast<ExpressionAST>(newAST);
	}

	if (mReturnExpression != nullptr) {
		mReturnExpression->rewrite();
	}
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

void ReturnStatementAST::verify(SemanticVerifier& verifier) {
	if (mReturnExpression != nullptr) {
		mReturnExpression->verify(verifier);
	}
}

void ReturnStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	if (mReturnExpression != nullptr) {
		mReturnExpression->generateCode(codeGen, func);
		func.addInstruction("STLOC " + std::to_string(func.getLocal(CodeGenerator::returnValueLocal).first));
	}

	func.addReturnBranch(func.numInstructions());
	func.addInstruction("BR");
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

void IfElseStatementAST::visit(VisitFn visitFn) const {
	mConditionExpression->visit(visitFn);
	mThenBlock->visit(visitFn);

	if (mElseBlock != nullptr) {
		mElseBlock->visit(visitFn);
	}
	
	visitFn(this);
}

void IfElseStatementAST::rewrite() {
	std::shared_ptr<AbstractSyntaxTree> newAST;

	if (mConditionExpression->rewriteAST(newAST)) {
		mConditionExpression = std::dynamic_pointer_cast<ExpressionAST>(newAST);
	}

	if (mThenBlock->rewriteAST(newAST)) {
		mThenBlock = std::dynamic_pointer_cast<BlockAST>(newAST);
	}

	if (mElseBlock != nullptr && mElseBlock->rewriteAST(newAST)) {
		mElseBlock = std::dynamic_pointer_cast<BlockAST>(newAST);
	}

	mConditionExpression->rewrite();
	mThenBlock->rewrite();

	if (mElseBlock != nullptr) {
		mElseBlock->rewrite();
	}
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

void IfElseStatementAST::verify(SemanticVerifier& verifier) {
	mConditionExpression->verify(verifier);

	mThenBlock->verify(verifier);

	if (mElseBlock != nullptr) {
		mElseBlock->verify(verifier);
	}
}

void generateBranch(std::shared_ptr<BinaryOpExpressionAST> binOpExpr, CodeGenerator& codeGen, GeneratedFunction& func, int& condIndex) {
	auto op = binOpExpr->op();

	if (op == Operator('<')) {
		binOpExpr->leftHandSide()->generateCode(codeGen, func);
		binOpExpr->rightHandSide()->generateCode(codeGen, func);
		condIndex = func.numInstructions();
		func.addInstruction("BGE");
	} else if (op == Operator('>')) {
		binOpExpr->leftHandSide()->generateCode(codeGen, func);
		binOpExpr->rightHandSide()->generateCode(codeGen, func);
		condIndex = func.numInstructions();
		func.addInstruction("BLE");
	} else if (op == Operator('<', '=')) {
		binOpExpr->leftHandSide()->generateCode(codeGen, func);
		binOpExpr->rightHandSide()->generateCode(codeGen, func);
		condIndex = func.numInstructions();
		func.addInstruction("BGT");
	} else if (op == Operator('>', '=')) {
		binOpExpr->leftHandSide()->generateCode(codeGen, func);
		binOpExpr->rightHandSide()->generateCode(codeGen, func);
		condIndex = func.numInstructions();
		func.addInstruction("BLT");
	} else if (op == Operator('=', '=')) {
		binOpExpr->leftHandSide()->generateCode(codeGen, func);
		binOpExpr->rightHandSide()->generateCode(codeGen, func);
		condIndex = func.numInstructions();
		func.addInstruction("BNE");
	} else if (op == Operator('!', '=')) {
		binOpExpr->leftHandSide()->generateCode(codeGen, func);
		binOpExpr->rightHandSide()->generateCode(codeGen, func);
		condIndex = func.numInstructions();
		func.addInstruction("BEQ");
	}
}

void IfElseStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	int condIndex = -1;

	//If simple expression, use branch instructions
	if (auto binOpExpr = std::dynamic_pointer_cast<BinaryOpExpressionAST>(mConditionExpression)) {
		generateBranch(binOpExpr, codeGen, func, condIndex);
	}

	//Else use compare & jump instructions
	if (condIndex == -1) {
		if (mConditionExpression->expressionType(codeGen.typeChecker())->name() == "Bool") {
			mConditionExpression->generateCode(codeGen, func);
			func.addInstruction("PUSHTRUE");
			condIndex = func.numInstructions();
			func.addInstruction("BNE");
		} else {
			codeGen.codeGenError("The condition must be a boolean expression.");
		}
	}

	mThenBlock->generateCode(codeGen, func);
	int elseIndex = -1;

	if (mElseBlock != nullptr) {
		elseIndex = func.numInstructions();
		func.addInstruction("BR");
	}

	func.instruction(condIndex) += " " + std::to_string(func.numInstructions());

	if (mElseBlock != nullptr) {
		mElseBlock->generateCode(codeGen, func);
		func.instruction(elseIndex) += " " + std::to_string(func.numInstructions());
	}
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

void WhileLoopStatementAST::visit(VisitFn visitFn) const {
	mConditionExpression->visit(visitFn);
	mBodyBlock->visit(visitFn);
	visitFn(this);
}

void WhileLoopStatementAST::rewrite() {
	std::shared_ptr<AbstractSyntaxTree> newAST;

	if (mConditionExpression->rewriteAST(newAST)) {
		mConditionExpression = std::dynamic_pointer_cast<ExpressionAST>(newAST);
	}

	if (mBodyBlock->rewriteAST(newAST)) {
		mBodyBlock = std::dynamic_pointer_cast<BlockAST>(newAST);
	}

	mConditionExpression->rewrite();
	mBodyBlock->rewrite();
}

void WhileLoopStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	mConditionExpression->generateSymbols(binder, symbolTable);
	mBodyBlock->generateSymbols(binder, symbolTable);
}

void WhileLoopStatementAST::typeCheck(TypeChecker& checker) {
	mConditionExpression->typeCheck(checker);
	mBodyBlock->typeCheck(checker);
}

void WhileLoopStatementAST::verify(SemanticVerifier& verifier) {
	mConditionExpression->verify(verifier);
	mBodyBlock->verify(verifier);
}

void WhileLoopStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	int condStart = func.numInstructions();
	int condIndex = -1;

	//If simple expression, use branch instructions
	if (auto binOpExpr = std::dynamic_pointer_cast<BinaryOpExpressionAST>(mConditionExpression)) {
		generateBranch(binOpExpr, codeGen, func, condIndex);
	}

	//Else use compare & jump instructions
	if (condIndex == -1) {
		if (mConditionExpression->expressionType(codeGen.typeChecker())->name() == "Bool") {
			mConditionExpression->generateCode(codeGen, func);
			func.addInstruction("PUSHTRUE");
			condIndex = func.numInstructions();
			func.addInstruction("BNE");
		} else {
			codeGen.codeGenError("The condition must be a boolean expression.");
		}
	}

	mBodyBlock->generateCode(codeGen, func);
	func.addInstruction("BR " + std::to_string(condStart));

	func.instruction(condIndex) += " " + std::to_string(func.numInstructions());
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

bool ForLoopStatementAST::rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST) const {
	auto bodyStatements = mBodyBlock->statements();
	bodyStatements.push_back(std::make_shared<ExpressionStatementAST>(mChangeExpression));

	std::vector<std::shared_ptr<StatementAST>> outerBlockStatements;
	outerBlockStatements.push_back(std::make_shared<ExpressionStatementAST>(mInitExpression));
	outerBlockStatements.push_back(std::make_shared<WhileLoopStatementAST>(mConditionExpression, std::make_shared<BlockAST>(bodyStatements)));

	newAST = std::make_shared<BlockAST>(outerBlockStatements);
	return true;
}