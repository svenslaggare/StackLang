#include "statementast.h"
#include "blockast.h"
#include "operatorast.h"
#include "typechecker.h"
#include "symboltable.h"
#include "asthelpers.h"
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

std::shared_ptr<AbstractSyntaxTree> ExpressionStatementAST::findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const {
	if (predicate(mExpression)) {
		return mExpression;
	}

	return mExpression->findAST(predicate);
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

void ReturnStatementAST::rewrite() {
	std::shared_ptr<AbstractSyntaxTree> newAST;

	if (mReturnExpression != nullptr && mReturnExpression->rewriteAST(newAST)) {
		mReturnExpression = std::dynamic_pointer_cast<ExpressionAST>(newAST);
	}

	if (mReturnExpression != nullptr) {
		mReturnExpression->rewrite();
	}
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
	int condIndex = -1;

	//If simple expression, use branch instructions
	if (auto binOpExpr = std::dynamic_pointer_cast<BinaryOpExpressionAST>(mConditionExpression)) {
		auto op = binOpExpr->op();

		if (op == Operator('<')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BGE");
		} else if (op == Operator('>')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BLE");
		} else if (op == Operator('<', '=')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BGT");
		} else if (op == Operator('>', '=')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BLT");
		} else if (op == Operator('=', '=')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BNE");
		} else if (op == Operator('!', '=')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BEQ");
		}
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

	mConditionExpression->generateSymbols(binder, symbolTable);
	mBodyBlock->generateSymbols(binder, symbolTable);
}

void WhileLoopStatementAST::typeCheck(TypeChecker& checker) {
	mConditionExpression->typeCheck(checker);
	mBodyBlock->typeCheck(checker);
}

void WhileLoopStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	// if (auto binOpExpr = std::dynamic_pointer_cast<BinaryOpExpressionAST>(mConditionExpression)) {
	// 	auto op = binOpExpr->op();

	// 	int condStart = func.numInstructions();
		
	// 	binOpExpr->rightHandSide()->generateCode(codeGen, func);
	// 	binOpExpr->leftHandSide()->generateCode(codeGen, func);
		
	// 	int condIndex = func.numInstructions();

	// 	if (op == Operator('<')) {
	// 		func.addInstruction("BGE");
	// 	} else if (op == Operator('>')) {
	// 		func.addInstruction("BLE");
	// 	} else if (op == Operator('<', '=')) {
	// 		func.addInstruction("BGT");
	// 	} else if (op == Operator('>', '=')) {
	// 		func.addInstruction("BLT");
	// 	} else if (op == Operator('=', '=')) {
	// 		func.addInstruction("BNE");
	// 	} else if (op == Operator('!', '=')) {
	// 		func.addInstruction("BEQ");
	// 	} else {
	// 		codeGen.codeGenError("'" + op.asString() + "' is not a boolean operator.");
	// 	}

	// 	mBodyBlock->generateCode(codeGen, func);
	// 	func.addInstruction("BR " + std::to_string(condStart));

	// 	func.instruction(condIndex) += " " + std::to_string(func.numInstructions());
	// } else {
	// 	codeGen.codeGenError("While statement not implemented for current expression.");
	// }

	int condStart = func.numInstructions();
	int condIndex = -1;

	//If simple expression, use branch instructions
	if (auto binOpExpr = std::dynamic_pointer_cast<BinaryOpExpressionAST>(mConditionExpression)) {
		auto op = binOpExpr->op();

		if (op == Operator('<')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BGE");
		} else if (op == Operator('>')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BLE");
		} else if (op == Operator('<', '=')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BGT");
		} else if (op == Operator('>', '=')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BLT");
		} else if (op == Operator('=', '=')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BNE");
		} else if (op == Operator('!', '=')) {
			binOpExpr->rightHandSide()->generateCode(codeGen, func);
			binOpExpr->leftHandSide()->generateCode(codeGen, func);
			condIndex = func.numInstructions();
			func.addInstruction("BEQ");
		}
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

// void ForLoopStatementAST::rewrite() {
// 	std::shared_ptr<AbstractSyntaxTree> newAST;

// 	if (mInitExpression->rewriteAST(newAST)) {
// 		mInitExpression = std::dynamic_pointer_cast<ExpressionAST>(newAST);
// 	}

// 	if (mConditionExpression->rewriteAST(newAST)) {
// 		mConditionExpression = std::dynamic_pointer_cast<ExpressionAST>(newAST);
// 	}

// 	if (mChangeExpression->rewriteAST(newAST)) {
// 		mChangeExpression = std::dynamic_pointer_cast<ExpressionAST>(newAST);
// 	}

// 	if (mBodyBlock->rewriteAST(newAST)) {
// 		mBodyBlock = std::dynamic_pointer_cast<BlockAST>(newAST);
// 	}

// 	mInitExpression->rewrite();
// 	mConditionExpression->rewrite();
// 	mChangeExpression->rewrite();
// 	mBodyBlock->rewrite();
// }

bool ForLoopStatementAST::rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST) const {
	auto bodyStatements = mBodyBlock->statements();
	bodyStatements.push_back(std::make_shared<ExpressionStatementAST>(mChangeExpression));

	std::vector<std::shared_ptr<StatementAST>> outerBlockStatements;
	outerBlockStatements.push_back(std::make_shared<ExpressionStatementAST>(mInitExpression));
	outerBlockStatements.push_back(std::make_shared<WhileLoopStatementAST>(mConditionExpression, std::make_shared<BlockAST>(bodyStatements)));

	newAST = std::make_shared<BlockAST>(outerBlockStatements);
	return true;
}

// std::shared_ptr<AbstractSyntaxTree> ForLoopStatementAST::findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const {
// 	std::shared_ptr<AbstractSyntaxTree> result;

// 	if (ASTHelpers::findAST(mInitExpression, predicate, result)
// 		|| ASTHelpers::findAST(mConditionExpression, predicate, result)
// 		|| ASTHelpers::findAST(mChangeExpression, predicate, result)
// 		|| ASTHelpers::findAST(mBodyBlock, predicate, result)) {
// 		return result;
// 	}

// 	return nullptr;
// }

// void ForLoopStatementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
// 	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

// 	auto inner = SymbolTable::newInner(symbolTable);

// 	mInitExpression->generateSymbols(binder, inner);
// 	mConditionExpression->generateSymbols(binder, inner);
// 	mChangeExpression->generateSymbols(binder, inner);
// 	mBodyBlock->generateSymbols(binder, inner);
// }

// void ForLoopStatementAST::typeCheck(TypeChecker& checker) {
// 	mInitExpression->typeCheck(checker);
// 	mConditionExpression->typeCheck(checker);
// 	mChangeExpression->typeCheck(checker);
// 	mBodyBlock->typeCheck(checker);
// }

// void ForLoopStatementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	
// }