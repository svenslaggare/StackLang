#include "ast.h"

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
		blockStr += "\t" + statement->asString() + "\n";
	}

	blockStr += "}";

	return blockStr;
}

//Function AST
FunctionAST::FunctionAST(std::string name, const std::vector<std::string>& arguments, std::shared_ptr<BlockAST> body)
	: mName(name), mArguments(arguments), mBody(body) {

}

const std::string FunctionAST::name() const {
	return mName;
}

const std::vector<std::string>& FunctionAST::arguments() const {
	return mArguments;
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

		funcStr += arg;
	}
	funcStr += ") = ";

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

//Binary OP expression
BinaryOpExpressionAST::BinaryOpExpressionAST(std::shared_ptr<ExpressionAST> leftHandSide, std::shared_ptr<ExpressionAST> rightHandSide, char opChar)
	: mLeftHandSide(leftHandSide), mRightHandSide(rightHandSide), mOpChar(opChar) {

}

std::shared_ptr<ExpressionAST> BinaryOpExpressionAST::leftHandSide() const {
	return mLeftHandSide;
}

std::shared_ptr<ExpressionAST> BinaryOpExpressionAST::rightHandSide() const {
	return mRightHandSide;
}

char BinaryOpExpressionAST::opChar() const {
	return mOpChar;
}

std::string BinaryOpExpressionAST::asString() const {
	return mLeftHandSide->asString() + mOpChar + mRightHandSide->asString();
}

//Unary OP expression
UnaryOpExpressionAST::UnaryOpExpressionAST(std::shared_ptr<ExpressionAST> operand, char opChar)
	: mOperand(operand), mOpChar(opChar) {

}

std::shared_ptr<ExpressionAST> UnaryOpExpressionAST::operand() const {
	return mOperand;
}

char UnaryOpExpressionAST::opChar() const {
	return mOpChar;
}

std::string UnaryOpExpressionAST::asString() const {
	return mOpChar + mOperand->asString();
}