#include "operatorast.h"
#include "ast.h"
#include "typechecker.h"
#include "type.h"
#include "symboltable.h"
#include "codegenerator.h"
#include "expressionast.h"

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

bool BinaryOpExpressionAST::rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST) const {
	bool correctOp = false;
	Operator op(' ');

	if (mOp == Operator('+', '=')) {
		op = Operator('+');
		correctOp = true;
	} else if (mOp == Operator('-', '=')) {
		op = Operator('-');
		correctOp = true;
	} else if (mOp == Operator('*', '=')) {
		op = Operator('*');
		correctOp = true;
	} else if (mOp == Operator('/', '=')) {
		op = Operator('/');
		correctOp = true;
	}

	if (correctOp) {
		std::shared_ptr<ExpressionAST> varRefExpr;

		if (auto varDec = std::dynamic_pointer_cast<VariableDeclerationExpressionAST>(mLeftHandSide)) {
			varRefExpr = std::make_shared<VariableReferenceExpressionAST>(varDec->varName());
		} else if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mLeftHandSide)) {
			varRefExpr = std::make_shared<VariableReferenceExpressionAST>(varRef->varName());
		} else {
			return false;
		}

		newAST = std::make_shared<BinaryOpExpressionAST>(mLeftHandSide, std::make_shared<BinaryOpExpressionAST>(varRefExpr, mRightHandSide, op), Operator('='));
		return true;
	}

	return false;
}

void BinaryOpExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

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
	if (mOp == Operator('+')) {
		mRightHandSide->generateCode(codeGen, func);
		mLeftHandSide->generateCode(codeGen, func);
		func.addInstruction("ADD");
	} else if (mOp == Operator('-')) {
		mRightHandSide->generateCode(codeGen, func);
		mLeftHandSide->generateCode(codeGen, func);
		func.addInstruction("SUB");
	} else if (mOp == Operator('*')) {
		mRightHandSide->generateCode(codeGen, func);
		mLeftHandSide->generateCode(codeGen, func);
		func.addInstruction("MUL");
	} else if (mOp == Operator('/')) {
		mRightHandSide->generateCode(codeGen, func);
		mLeftHandSide->generateCode(codeGen, func);
		func.addInstruction("DIV");
	} else if (mOp == Operator('=')) {
		mRightHandSide->generateCode(codeGen, func);

		if (auto varDec = std::dynamic_pointer_cast<VariableDeclerationExpressionAST>(mLeftHandSide)) {
			mLeftHandSide->generateCode(codeGen, func);
			func.addInstruction("STLOC " + std::to_string(func.getLocal(varDec->varName()).first));
		} else if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mLeftHandSide)) {
			auto varDec = std::dynamic_pointer_cast<VariableDeclerationExpressionAST>(mSymbolTable->find(varRef->varName()));

			if (!varDec->isFunctionParameter()) {
				func.addInstruction("STLOC " + std::to_string(func.getLocal(varRef->varName()).first));
			} else {
				codeGen.codeGenError("Assignment to function parameter.");
			}
		} else {
			codeGen.codeGenError("Left hand side is not decleration or variable reference.");
		}
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