#include "operatorast.h"
#include "ast.h"
#include "expressionast.h"
#include "variableast.h"
#include "arrayast.h"
#include "objectast.h"
#include "namespaceast.h"
#include "../compiler.h"
#include "../typechecker.h"
#include "../type.h"
#include "../typename.h"
#include "../symboltable.h"
#include "../codegenerator.h"
#include "../symbol.h"
#include "../semantics.h"

//Binary OP expression AST
std::set<std::string> BinaryOpExpressionAST::arithmeticTypes = {
	TypeSystem::toString(PrimitiveTypes::Int),
	TypeSystem::toString(PrimitiveTypes::Float)
};

std::set<std::string> BinaryOpExpressionAST::equalityTypes = {
	TypeSystem::toString(PrimitiveTypes::Int),
	TypeSystem::toString(PrimitiveTypes::Bool),
	TypeSystem::toString(PrimitiveTypes::Float)
};

std::set<std::string> BinaryOpExpressionAST::comparableTypes = {
	TypeSystem::toString(PrimitiveTypes::Int),
	TypeSystem::toString(PrimitiveTypes::Float)
};

std::set<std::string> BinaryOpExpressionAST::logicalTypes = {
	TypeSystem::toString(PrimitiveTypes::Bool)
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

void BinaryOpExpressionAST::visit(VisitFn visitFn) const {
	mLeftHandSide->visit(visitFn);
	mRightHandSide->visit(visitFn);
	visitFn(this);
}

void BinaryOpExpressionAST::rewrite(Compiler& compiler) {
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

bool BinaryOpExpressionAST::rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST, Compiler& compiler) const {
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

		if (auto varDec = std::dynamic_pointer_cast<VariableDeclarationExpressionAST>(mLeftHandSide)) {
			varRefExpr = std::make_shared<VariableReferenceExpressionAST>(varDec->name());
		} else if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mLeftHandSide)) {
			varRefExpr = std::make_shared<VariableReferenceExpressionAST>(varRef->name());
		} else {
			return false;
		}

		newAST = std::make_shared<BinaryOpExpressionAST>(
			mLeftHandSide,
			std::make_shared<BinaryOpExpressionAST>(varRefExpr, mRightHandSide, op),
			Operator('='));

		return true;
	}

	auto arraySetElem = std::dynamic_pointer_cast<ArrayAccessAST>(mLeftHandSide); 
	if (arraySetElem != nullptr && mOp == Operator('=')) {
		newAST = std::make_shared<ArraySetElementAST>(
			arraySetElem->arrayRefExpression(),
			arraySetElem->accessExpression(),
			mRightHandSide);

		return true;
	}

	if (mOp == Operator('.')) {
		newAST = std::make_shared<MemberAccessAST>(
		 	mLeftHandSide,
		 	mRightHandSide);
		return true;
	}

	if (mOp == Operator('=')) {
		std::shared_ptr<AbstractSyntaxTree> newLHS;
		if (!mLeftHandSide->rewriteAST(newLHS, compiler)) {
			newLHS = mLeftHandSide;
		}

		auto setFieldValue = std::dynamic_pointer_cast<MemberAccessAST>(newLHS);
		if (setFieldValue != nullptr) {
			newAST = std::make_shared<SetFieldValueAST>(
				setFieldValue->accessExpression(),
				setFieldValue->memberExpression(),
				mRightHandSide);

			return true;
		}
	}

	//Check if the symbols are bound
	if (mSymbolTable != nullptr) {		
		//After symbol binding, check if lhs is field ref. 
		//This is for the case when the field is assigned inside a member function without a this ref.
		if (mOp == Operator('=')) {
			if (auto lhsVarRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mLeftHandSide)) {
				if (lhsVarRef->symbol()->attribute() == VariableSymbolAttribute::FIELD) {
					newAST = std::make_shared<SetFieldValueAST>(
						std::make_shared<VariableReferenceExpressionAST>("this"),
						mLeftHandSide,
						mRightHandSide);

					newAST->generateSymbols(compiler.binder(), mSymbolTable);

					return true;
				}
			}
		}
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

	auto lhsType = mLeftHandSide->expressionType(checker);
	auto rhsType = mRightHandSide->expressionType(checker);

	if (lhsType->name() != "Auto") {
		auto intType = checker.findType("Int");
		auto floatType = checker.findType("Float");

		checker.assertSameType(
			*lhsType, 
			*rhsType,
			asString());
	} else {
		//Infer the type
		auto lhsVarDec = std::dynamic_pointer_cast<VariableDeclarationExpressionAST>(mLeftHandSide);

		if (lhsVarDec != nullptr) {
			if (lhsType->name() == "Auto" && *rhsType == NullReferenceType()) {
				checker.typeError("Implicitly type of a null variable is not allowed.");
			}

			mLeftHandSide = std::make_shared<VariableDeclarationExpressionAST>(
				rhsType->name(),
				lhsVarDec->name(),
				lhsVarDec->isFunctionParameter());

			//Update the symbol
			mSymbolTable->remove(lhsVarDec->name());
			mLeftHandSide->generateSymbols(checker.binder(), mSymbolTable);
		} else {
			//Should never happen
			checker.typeError("Auto type is only allowed in variable declaration.");
		}
	}
}

void BinaryOpExpressionAST::verify(SemanticVerifier& verifier) {
	mRightHandSide->verify(verifier);
	mLeftHandSide->verify(verifier);

	auto checker = verifier.typeChecker();
	auto lhsType = mLeftHandSide->expressionType(checker);
	auto rhsType = mRightHandSide->expressionType(checker);

	if (mOp == Operator('+') || mOp == Operator('-') || mOp == Operator('*') || mOp == Operator('/')) {
		if (arithmeticTypes.count(lhsType->name()) == 0) {
			verifier.semanticError("The type '" + lhsType->name() + "' doesn't support arithmetic operators.");
		}
	}

	if (mOp == Operator('!', '=') || mOp == Operator('=', '=')) {
		if (equalityTypes.count(lhsType->name()) == 0) {
			verifier.semanticError("The type '" + lhsType->name() + "' doesn't support equality operators.");
		}
	}

	if (mOp == Operator('&', '&') || mOp == Operator('|', '|')) {
		if (logicalTypes.count(lhsType->name()) == 0) {
			verifier.semanticError("The type '" + lhsType->name() + "' doesn't support logical operators.");
		}
	}

	if (mOp == Operator('<') || mOp == Operator('<', '=') || mOp == Operator('>') || mOp == Operator('>', '=')) {
		if (comparableTypes.count(lhsType->name()) == 0) {
			verifier.semanticError("The type '" + lhsType->name() + "' doesn't support compare operators.");
		}
	}

	if (mOp == Operator('=')) {
		if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mLeftHandSide)) {
			auto varDec = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->name()));

			if (varDec->attribute() == VariableSymbolAttribute::FUNCTION_PARAMETER) {
				verifier.semanticError("Assignment to function parameter is not allowed.");
			}
		} else if (auto varDec = std::dynamic_pointer_cast<VariableDeclarationExpressionAST>(mLeftHandSide) != nullptr) {

		} else {
			verifier.semanticError("Left hand side is not declaration or variable reference.");
		}
	}
}

std::shared_ptr<Type> BinaryOpExpressionAST::expressionType(const TypeChecker& checker) const {
	auto& boolTypes = checker.operators().binaryOpReturnTypes();

	if (boolTypes.count(mOp) > 0) {
		return boolTypes.at(mOp);
	} else {
		if (mOp == Operator('=')) {
			return checker.findType("Void");
		} else {
			return mLeftHandSide->expressionType(checker);
		}
	}
}

void BinaryOpExpressionAST::generateSidesCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mLeftHandSide->generateCode(codeGen, func);
	mRightHandSide->generateCode(codeGen, func);
}

void BinaryOpExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	if (mOp == Operator('+')) {
		generateSidesCode(codeGen, func);
		func.addInstruction("ADD");
	} else if (mOp == Operator('-')) {
		generateSidesCode(codeGen, func);
		func.addInstruction("SUB");
	} else if (mOp == Operator('*')) {
		generateSidesCode(codeGen, func);
		func.addInstruction("MUL");
	} else if (mOp == Operator('/')) {
		generateSidesCode(codeGen, func);
		func.addInstruction("DIV");
	} else if (mOp == Operator('=')) {
		if (auto varDec = std::dynamic_pointer_cast<VariableDeclarationExpressionAST>(mLeftHandSide)) {
			auto varRefSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varDec->name()));
			generateSidesCode(codeGen, func);
			func.addInstruction("STLOC " + std::to_string(func.getLocal(varRefSymbol).first));
		} else if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mLeftHandSide)) {
			mRightHandSide->generateCode(codeGen, func);
			auto varRefSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->name()));

			if (varRefSymbol->attribute() != VariableSymbolAttribute::FUNCTION_PARAMETER) {
				func.addInstruction("STLOC " + std::to_string(func.getLocal(varRefSymbol).first));
			}
		}
	} else if(mOp == Operator('=', '=')) {
		generateSidesCode(codeGen, func);
		func.addInstruction("CMPEQ");
	} else if(mOp == Operator('!', '=')) {
		generateSidesCode(codeGen, func);
		func.addInstruction("CMPNE");
	} else if(mOp == Operator('>')) {
		generateSidesCode(codeGen, func);
		func.addInstruction("CMPGT");
	} else if(mOp == Operator('>', '=')) {
		generateSidesCode(codeGen, func);
		func.addInstruction("CMPGE");
	} else if(mOp == Operator('<')) {
		generateSidesCode(codeGen, func);
		func.addInstruction("CMPLT");
	} else if(mOp == Operator('<', '=')) {
		generateSidesCode(codeGen, func);
		func.addInstruction("CMPLE");
	} else if(mOp == Operator('&', '&')) {
		//Generate with short circuit
		int resLocal = func.newLocal("$tmp$_" + std::to_string(func.numLocals()), codeGen.typeChecker().findType("Bool"));

		mLeftHandSide->generateCode(codeGen, func);
		func.addInstruction("LDTRUE");
		int branchIndex = func.numInstructions();
		func.addInstruction("BNE ");

		mRightHandSide->generateCode(codeGen, func);
		func.addStoreLocal(resLocal);

		int skipIndex = func.numInstructions();
		func.addInstruction("BR ");
		func.instruction(branchIndex) += std::to_string(func.numInstructions());
		func.addInstruction("LDFALSE");
		func.addStoreLocal(resLocal);
		func.instruction(skipIndex) += std::to_string(func.numInstructions());

		func.addLoadLocal(resLocal);
	} else if(mOp == Operator('|', '|')) {
		//Generate with short circuit
		int resLocal = func.newLocal("$tmp$_" + std::to_string(func.numLocals()), codeGen.typeChecker().findType("Bool"));

		mLeftHandSide->generateCode(codeGen, func);
		func.addInstruction("LDTRUE");
		int branchIndex = func.numInstructions();
		func.addInstruction("BEQ ");

		mRightHandSide->generateCode(codeGen, func);
		func.addStoreLocal(resLocal);

		int skipIndex = func.numInstructions();
		func.addInstruction("BR ");
		func.instruction(branchIndex) += std::to_string(func.numInstructions());
		func.addInstruction("LDTRUE");
		func.addStoreLocal(resLocal);
		func.instruction(skipIndex) += std::to_string(func.numInstructions());

		func.addLoadLocal(resLocal);
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

void UnaryOpExpressionAST::visit(VisitFn visitFn) const {
	mOperand->visit(visitFn);
	visitFn(this);
}

bool UnaryOpExpressionAST::rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST, Compiler& compiler) const {
	//Rewrite -constant as a constant expression
	if (mOp == Operator('-')) {
		auto intExpr = std::dynamic_pointer_cast<IntegerExpressionAST>(mOperand);

		if (intExpr != nullptr) {
			newAST = std::make_shared<IntegerExpressionAST>(-intExpr->value());
			return true;
		}

		auto floatExpr = std::dynamic_pointer_cast<FloatExpressionAST>(mOperand);

		if (floatExpr != nullptr) {
			newAST = std::make_shared<FloatExpressionAST>(-floatExpr->value());
			return true;
		}
	}

	return false;
}

void UnaryOpExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mOperand->generateSymbols(binder, symbolTable);
}

void UnaryOpExpressionAST::typeCheck(TypeChecker& checker) {
	mOperand->typeCheck(checker);

	auto opType = mOperand->expressionType(checker);

	if (mOp == Operator('!') && opType != checker.findType("Bool")) {
		checker.typeError("The '!' operator can only be applied to values/variables of type 'Bool'.");
	} else if (mOp == Operator('-') && opType != checker.findType("Int") && opType != checker.findType("Float")) {
		checker.typeError("The '-' operator can only be applied to values/variables of type 'Int' or 'Float'.");
	}
}

void UnaryOpExpressionAST::verify(SemanticVerifier& verifier) {
	mOperand->verify(verifier);
}

std::shared_ptr<Type> UnaryOpExpressionAST::expressionType(const TypeChecker& checker) const {
	return mOperand->expressionType(checker);
}

void UnaryOpExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	if (mOp == Operator('-')) {
		auto opType = mOperand->expressionType(codeGen.typeChecker());

		if (*opType == *codeGen.typeChecker().findType("Int")) {
			func.addInstruction("LDINT 0");
		} else if (*opType == *codeGen.typeChecker().findType("Float")) {
			func.addInstruction("LDFLOAT 0");
		}

		mOperand->generateCode(codeGen, func);
		func.addInstruction("SUB");
	} else if (mOp == Operator('!')) {
		mOperand->generateCode(codeGen, func);
		func.addInstruction("NOT");
	}
}