#include "operatorast.h"
#include "ast.h"

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

void BinaryOpExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	mLeftHandSide->generateSymbols(binder, symbolTable);
	mRightHandSide->generateSymbols(binder, symbolTable);
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

void UnaryOpExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	mOperand->generateSymbols(binder, symbolTable);
}