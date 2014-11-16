#include "ast.h"
#include <stdexcept>

template <class T>
std::string combineAST(std::vector<std::shared_ptr<T>> asts, std::string sep) {
	bool isFirst = true;
	std::string str = "";

	for (auto ast : asts) {
		if (!isFirst) {
			str += sep;
		} else {
			isFirst = false;
		}

		str += ast->asString();
	}

	return str;
}

std::ostream& operator<<(std::ostream& os, const AbstractSyntaxTree& ast) {
	os << ast.asString();
	return os;
}

//Program AST
ProgramAST::ProgramAST(const std::vector<std::shared_ptr<FunctionAST>>& functions)
	: mFunctions(functions) {

}

const std::vector<std::shared_ptr<FunctionAST>>& ProgramAST::functions() const {
	return mFunctions;
}

std::string ProgramAST::asString() const {
	return combineAST(mFunctions, "\n\n");
}

//Block AST
BlockAST::BlockAST(const std::vector<std::shared_ptr<StatementAST>>& statements)
	: mStatements(statements) {

}

const std::vector<std::shared_ptr<StatementAST>>& BlockAST::statements() const {
	return mStatements;
}

std::string BlockAST::asString() const {
	std::string blockStr = "";

	blockStr += "{\n";

	for (auto statement : mStatements) {
		blockStr += statement->asString() + "\n";
	}

	blockStr += "}";

	return blockStr;
}

//Function AST
FunctionAST::FunctionAST(std::string name, const std::vector<std::shared_ptr<VariableDeclerationExpressionAST>>& arguments, std::string returnType, std::shared_ptr<BlockAST> body)
	: mName(name), mArguments(arguments), mReturnType(returnType), mBody(body) {

}

const std::string FunctionAST::name() const {
	return mName;
}

const std::vector<std::shared_ptr<VariableDeclerationExpressionAST>>& FunctionAST::arguments() const {
	return mArguments;
}

const std::string FunctionAST::returnType() const {
	return mReturnType;
}

std::shared_ptr<BlockAST> FunctionAST::body() const {
	return mBody;
}

std::string FunctionAST::asString() const {
	std::string funcStr = "func " + mName + "(";

	bool isFirst = true;
	for (auto arg : mArguments) {
		if (!isFirst) {
			funcStr += ", ";
		} else {
			isFirst = false;
		}

		funcStr += arg->asString();
	}
	funcStr += "): " + mReturnType + " ";

	funcStr += mBody->asString();

	return funcStr;
}

//Expression statement AST
ExpressionStatementAST::ExpressionStatementAST(std::shared_ptr<ExpressionAST> expression)
	: mExpression(expression) {

}

std::shared_ptr<ExpressionAST> ExpressionStatementAST::expression() const {
	return mExpression;
}

std::string ExpressionStatementAST::asString() const {
	return mExpression->asString() + ";";
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

//Foor loop statement AST
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

//Integer expression AST
IntegerExpressionAST::IntegerExpressionAST(int value)
	: mValue(value) {

}

int IntegerExpressionAST::value() const {
	return mValue;
}

std::string IntegerExpressionAST::asString() const {
	return std::to_string(mValue);
}

//Variable reference expression AST
VariableReferenceExpressionAST::VariableReferenceExpressionAST(std::string varName)
	: mVarName(varName) {

}

std::string VariableReferenceExpressionAST::varName() const {
	return mVarName;
}

std::string VariableReferenceExpressionAST::asString() const {
	return mVarName;
}

//Variable decleration expression AST
VariableDeclerationExpressionAST::VariableDeclerationExpressionAST(std::string varType, std::string varName)
	: mVarType(varType), mVarName(varName) {

}

std::string VariableDeclerationExpressionAST::varType() const {
	return mVarType;
}

std::string VariableDeclerationExpressionAST::varName() const {
	return mVarName;
}

std::string VariableDeclerationExpressionAST::asString() const {
	return mVarType + " " + mVarName;
}

//Call expression AST
CallExpressionAST::CallExpressionAST(std::string functionName, std::vector<std::shared_ptr<ExpressionAST>> arguments)
	: mFunctionName(functionName), mArguments(arguments) {

}

std::string CallExpressionAST::functionName() const {
	return mFunctionName;
}

std::vector<std::shared_ptr<ExpressionAST>> CallExpressionAST::arguments() const {
	return mArguments;
}

std::string CallExpressionAST::asString() const {
	std::string callStr = "";

	callStr += mFunctionName + "(" + combineAST(mArguments, ", ") + ")";

	return callStr;
}

//Operator
Operator::Operator(char op1)
	: mOp1(op1), mIsTwoChars(false) {

}

Operator::Operator(char op1, char op2)
	: mOp1(op1), mOp2(op2), mIsTwoChars(true) {

}

bool Operator::isTwoChars() const {
	return mIsTwoChars;
}

char Operator::op1() const {
	return mOp1;
}

char Operator::op2() const {
	if (!mIsTwoChars) {
		throw std::logic_error("The current operator isn't a two-char operator.");
	}

	return mOp2;
}

bool Operator::operator==(const Operator& rhs) const {
	return mIsTwoChars == rhs.mIsTwoChars && mOp1 == rhs.mOp1 && mOp2 == rhs.mOp2;
}

bool Operator::operator!=(const Operator& rhs) const {
	return !((*this) == rhs);
}

std::string Operator::asString() const {
	return mIsTwoChars ? (std::string("") + mOp1 + mOp2) : std::string("") + mOp1;
}

//Binary OP expression AST
BinaryOpExpressionAST::BinaryOpExpressionAST(std::shared_ptr<ExpressionAST> leftHandSide, std::shared_ptr<ExpressionAST> rightHandSide, Operator op)
	: mLeftHandSide(leftHandSide), mRightHandSide(rightHandSide), mOp(op) {

}

std::shared_ptr<ExpressionAST> BinaryOpExpressionAST::leftHandSide() const {
	return mLeftHandSide;
}

std::shared_ptr<ExpressionAST> BinaryOpExpressionAST::rightHandSide() const {
	return mRightHandSide;
}

Operator BinaryOpExpressionAST::op() const {
	return mOp;
}

std::string BinaryOpExpressionAST::asString() const {
	return mLeftHandSide->asString() + " " + mOp.asString() + " " + mRightHandSide->asString();
}

//Unary OP expression AST
UnaryOpExpressionAST::UnaryOpExpressionAST(std::shared_ptr<ExpressionAST> operand, Operator op)
	: mOperand(operand), mOp(op) {

}

std::shared_ptr<ExpressionAST> UnaryOpExpressionAST::operand() const {
	return mOperand;
}

Operator UnaryOpExpressionAST::op() const {
	return mOp;
}

std::string UnaryOpExpressionAST::asString() const {
	return mOp.asString() + mOperand->asString();
}