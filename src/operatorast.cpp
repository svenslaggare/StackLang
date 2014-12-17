#include "operatorast.h"
#include "ast.h"
#include "typechecker.h"
#include "type.h"
#include "codegenerator.h"

//Operator
Operator::Operator(char op1)
	: mOp1(op1), mOp2(0), mIsTwoChars(false) {

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

bool Operator::operator<(const Operator& rhs) const {
	if (mIsTwoChars < rhs.mIsTwoChars) {
		return true;
	} else if (mIsTwoChars > rhs.mIsTwoChars) {
		return false;
	} else {
		if (mIsTwoChars) {
			if (mOp1 < rhs.mOp1) {
				return true;
			} else if (mOp1 > rhs.mOp1) {
				return false;
			} else {
				return mOp2 < rhs.mOp2;
			}
		} else {
			return mOp1 < rhs.mOp1;
		}
	}
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
const std::map<Operator, std::shared_ptr<Type>> BinaryOpExpressionAST::mBoolTypes {
	{ Operator('<'), std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Bool)) },
	{ Operator('>'), std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Bool)) },
	{ Operator('<', '='), std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Bool)) },
	{ Operator('>', '='), std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Bool)) },
	{ Operator('=', '='), std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Bool)) },
	{ Operator('!', '='), std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Bool)) },
	{ Operator('&', '&'), std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Bool)) },
	{ Operator('|', '|'), std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Bool)) }
};

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
	mRightHandSide->generateSymbols(binder, symbolTable);
	mLeftHandSide->generateSymbols(binder, symbolTable);
}

void BinaryOpExpressionAST::typeCheck(TypeChecker& checker) {
	mRightHandSide->typeCheck(checker);
	mLeftHandSide->typeCheck(checker);
	checker.assertSameType(
		*mLeftHandSide->expressionType(checker), 
		*mRightHandSide->expressionType(checker),
		asString());
}

std::shared_ptr<Type> BinaryOpExpressionAST::expressionType(const TypeChecker& checker) const {
	if (mBoolTypes.count(mOp) > 0) {
		return mBoolTypes.at(mOp);
	} else {
		return mLeftHandSide->expressionType(checker);
	}
}

void BinaryOpExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mLeftHandSide->generateCode(codeGen, func);
	mRightHandSide->generateCode(codeGen, func);

	if (mOp == Operator('+')) {
		func.addInstruction("ADD");
	} else if (mOp == Operator('-')) {
		func.addInstruction("SUB");
	} else if (mOp == Operator('*')) {
		func.addInstruction("MUL");
	} else if (mOp == Operator('/')) {
		func.addInstruction("DIV");
	} else {
		codeGen.codeGenError("Operator '" + mOp.asString() + "' is not defined.");
	}
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

void UnaryOpExpressionAST::typeCheck(TypeChecker& checker) {
	mOperand->typeCheck(checker);
}

std::shared_ptr<Type> UnaryOpExpressionAST::expressionType(const TypeChecker& checker) const {
	return mOperand->expressionType(checker);
}

void UnaryOpExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	if (mOp == Operator('-')) {
		func.addInstruction("PUSH 0");
		mOperand->generateCode(codeGen, func);
		func.addInstruction("SUB");
	} else {
		codeGen.codeGenError("Operator '" + mOp.asString() + "' is not defined.");
	}
}