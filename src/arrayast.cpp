#include "arrayast.h"
#include "typechecker.h"
#include "type.h"
#include "symboltable.h"
#include "binder.h"
#include "symbol.h"
#include "codegenerator.h"
#include "helpers.h"
#include "statementast.h"

//Array declaration
ArrayDeclarationAST::ArrayDeclarationAST(std::string elementType, std::shared_ptr<ExpressionAST> lengthExpression)
	: mElementType(elementType), mLengthExpression(lengthExpression) {
	
}

std::string ArrayDeclarationAST::elementType() const {
	return mElementType;
}

std::shared_ptr<ExpressionAST> ArrayDeclarationAST::lengthExpression() const {
	return mLengthExpression;
}

std::string ArrayDeclarationAST::asString() const {
	return "new " + mElementType + "[" + mLengthExpression->asString() + "]";
}

void ArrayDeclarationAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mLengthExpression->generateSymbols(binder, symbolTable);
}

void ArrayDeclarationAST::typeCheck(TypeChecker& checker) {
	mLengthExpression->typeCheck(checker);

	//Check length
	checker.assertSameType(
		*checker.getType("Int"),
		*mLengthExpression->expressionType(checker),
		"Expected the length to be of type 'Int'.");

	//Check if the element type exists
	checker.assertTypeExists(mElementType, false);
	checker.assertNotVoid(*checker.findType(mElementType), "Arrays of type 'Void' is not allowed.");

	//Create the array type if not created
	checker.getType(mElementType + "[]");
}
	
std::shared_ptr<Type> ArrayDeclarationAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(mElementType + "[]");
}

void ArrayDeclarationAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mLengthExpression->generateCode(codeGen, func);
	func.addInstruction("NEWARR " + codeGen.typeChecker().findType(mElementType)->vmType());
}

//Multidim array declaration
MultiDimArrayDeclarationAST::MultiDimArrayDeclarationAST(std::string elementType, std::vector<std::shared_ptr<ExpressionAST>> lengthExpressions)
	: mElementType(elementType), mLengthExpressions(lengthExpressions) {

}

std::string MultiDimArrayDeclarationAST::typeString(int dim) const {
	std::string arrayRankStr = "";
	
	if (dim == -1) {
		dim = mLengthExpressions.size();
	}

	for (int i = 0; i < dim; i++) {
		arrayRankStr += "[]";
	}

	return mElementType + arrayRankStr;
}

std::string MultiDimArrayDeclarationAST::elementType() const {
	return mElementType;
}

const std::vector<std::shared_ptr<ExpressionAST>>& MultiDimArrayDeclarationAST::lengthExpressions() const {
	return mLengthExpressions;
}

std::string MultiDimArrayDeclarationAST::asString() const {
	auto lengthsStr = Helpers::join<std::shared_ptr<ExpressionAST>>(
		mLengthExpressions,
		[](std::shared_ptr<ExpressionAST> length) { return length->asString(); },
		", ");

	return "new " + mElementType + "[" + lengthsStr + "]";
}

void MultiDimArrayDeclarationAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	
	for (auto lengthExpr : mLengthExpressions) {
		lengthExpr->generateSymbols(binder, symbolTable);
	}
}

void MultiDimArrayDeclarationAST::typeCheck(TypeChecker& checker) {
	for (auto lengthExpr : mLengthExpressions) {
		lengthExpr->typeCheck(checker);
	}

	//Check lengths
	int dim = 0;
	for (auto lengthExpr : mLengthExpressions) {
		checker.assertSameType(
			*checker.getType("Int"),
			*lengthExpr->expressionType(checker),
			"Expected the length of dimension " + std::to_string(dim) + " to be of type 'Int'.");
		dim++;
	}

	//Check if the element type exists
	checker.assertTypeExists(mElementType, false);
	checker.assertNotVoid(*checker.findType(mElementType), "Arrays of type 'Void' is not allowed.");

	//Create the array type if not created
	checker.getType(typeString());
}
	
std::shared_ptr<Type> MultiDimArrayDeclarationAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(typeString());
}

void MultiDimArrayDeclarationAST::generateArrayDim(CodeGenerator& codeGen, GeneratedFunction& func, int dim) {
	auto lengthExpr = mLengthExpressions.at(mLengthExpressions.size() - dim - 1);

	if (dim == 1) {
		lengthExpr->generateCode(codeGen, func);
		func.addInstruction("NEWARR " + codeGen.typeChecker().findType(mElementType)->vmType());
	} else {

	}
}

void MultiDimArrayDeclarationAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	if (mLengthExpressions.size() > 2) {
		codeGen.codeGenError("Array creation of dimension larger than 2 is not supported.");
	}

	auto& typeChecker = codeGen.typeChecker();

	int outerLocal = func.newLocal("$local$_outer_" + std::to_string(func.numLocals()), typeChecker.findType(typeString()));
	int subArrayLocal = func.newLocal("$local$_sub_" + std::to_string(func.numLocals()), typeChecker.findType("Int"));

	//Create the outer array
	mLengthExpressions.at(0)->generateCode(codeGen, func);
	func.addInstruction("NEWARR " + typeChecker.findType(typeString(mLengthExpressions.size() - 1))->vmType());
	func.addInstruction("STLOC " + std::to_string(outerLocal));

	int condStart = func.numInstructions();
	int condIndex = -1;

	//Condition
	mLengthExpressions.at(0)->generateCode(codeGen, func);
	func.addInstruction("LDLOC " + std::to_string(subArrayLocal));
	condIndex = func.numInstructions();
	func.addInstruction("BGE");

	//Body
	func.addInstruction("LDLOC " + std::to_string(outerLocal));
	func.addInstruction("LDLOC " + std::to_string(subArrayLocal));
	mLengthExpressions.at(1)->generateCode(codeGen, func);
	func.addInstruction("NEWARR " + typeChecker.findType(typeString(mLengthExpressions.size() - 2))->vmType());

	func.addInstruction("STELEM " + typeChecker.findType(typeString(mLengthExpressions.size() - 1))->vmType());

	func.addInstruction("LDLOC " + std::to_string(subArrayLocal));
	func.addInstruction("PUSHINT 1");
	func.addInstruction("ADD");
	func.addInstruction("STLOC " + std::to_string(subArrayLocal));

	func.addInstruction("BR " + std::to_string(condStart));
	func.instruction(condIndex) += " " + std::to_string(func.numInstructions());

	func.addInstruction("LDLOC " + std::to_string(outerLocal));
}

//Array access
ArrayAccessAST::ArrayAccessAST(std::shared_ptr<ExpressionAST> arrayRefExpression, std::shared_ptr<ExpressionAST> accessExpression)
	: mArrayRefExpression(arrayRefExpression), mAccessExpression(accessExpression) {
	
}

std::shared_ptr<ExpressionAST> ArrayAccessAST::arrayRefExpression() const {
	return mArrayRefExpression;
}

std::shared_ptr<ExpressionAST> ArrayAccessAST::accessExpression() const {
	return mAccessExpression;
}

std::string ArrayAccessAST::asString() const {
	return mArrayRefExpression->asString() + "[" + mAccessExpression->asString() + "]";
}

void ArrayAccessAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mArrayRefExpression->generateSymbols(binder, symbolTable);
	mAccessExpression->generateSymbols(binder, symbolTable);
}

void ArrayAccessAST::typeCheck(TypeChecker& checker) {
	mAccessExpression->typeCheck(checker);
	mArrayRefExpression->typeCheck(checker);

	//Check if array
	auto varType = std::dynamic_pointer_cast<ArrayType>(mArrayRefExpression->expressionType(checker));

	if (varType == nullptr) {
		checker.typeError("The expression '" + mArrayRefExpression->asString() + "' is not of array type.");
	}

	checker.assertSameType(
		*checker.getType("Int"),
		*mAccessExpression->expressionType(checker),
		"Expected the array access indexing to be of type 'Int'.");
}
	
std::shared_ptr<Type> ArrayAccessAST::expressionType(const TypeChecker& checker) const {
	auto varType = std::dynamic_pointer_cast<ArrayType>(mArrayRefExpression->expressionType(checker));
	return varType->elementType();
}

void ArrayAccessAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mArrayRefExpression->generateCode(codeGen, func);
	mAccessExpression->generateCode(codeGen, func);
	auto varType = std::dynamic_pointer_cast<ArrayType>(mArrayRefExpression->expressionType(codeGen.typeChecker()));
	func.addInstruction("LDELEM " + varType->elementType()->vmType());
}

//Array set element
ArraySetElementAST::ArraySetElementAST(
	std::shared_ptr<ExpressionAST> arrayRefExpression,
	std::shared_ptr<ExpressionAST> accessExpression,
	std::shared_ptr<ExpressionAST> rightHandSide)
	: mArrayRefExpression(arrayRefExpression), mAccessExpression(accessExpression), mRightHandSide(rightHandSide) {
	
}

std::shared_ptr<ExpressionAST> ArraySetElementAST::arrayRefExpression() const {
	return mArrayRefExpression;
}

std::shared_ptr<ExpressionAST> ArraySetElementAST::accessExpression() const {
	return mAccessExpression;
}

std::shared_ptr<ExpressionAST> ArraySetElementAST::rightHandSide() const {
	return mRightHandSide;
}

std::string ArraySetElementAST::asString() const {
	return mArrayRefExpression->asString() + "[" + mAccessExpression->asString() + "] = " + mRightHandSide->asString();
}

void ArraySetElementAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mArrayRefExpression->generateSymbols(binder, symbolTable);
	mAccessExpression->generateSymbols(binder, symbolTable);
	mRightHandSide->generateSymbols(binder, symbolTable);
}

void ArraySetElementAST::typeCheck(TypeChecker& checker) {
	mArrayRefExpression->typeCheck(checker);
	mAccessExpression->typeCheck(checker);
	mRightHandSide->typeCheck(checker);

	//Check if array
	auto arrayRefType = std::dynamic_pointer_cast<ArrayType>(mArrayRefExpression->expressionType(checker));

	if (arrayRefType == nullptr) {
		checker.typeError("The expression '" + mArrayRefExpression->asString() + "' is not of array type.");
	}

	//Access access
	checker.assertSameType(
		*checker.getType("Int"),
		*mAccessExpression->expressionType(checker),
		"Expected the array indexing to be of type 'Int'.");

	//Check rhs
	checker.assertSameType(
		*arrayRefType->elementType(), 
		*mRightHandSide->expressionType(checker),
		asString());
}
	
std::shared_ptr<Type> ArraySetElementAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("Void");
}

void ArraySetElementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	auto arrayRefType = std::dynamic_pointer_cast<ArrayType>(mArrayRefExpression->expressionType(codeGen.typeChecker()));

	mArrayRefExpression->generateCode(codeGen, func);
	mAccessExpression->generateCode(codeGen, func);
	mRightHandSide->generateCode(codeGen, func);
	func.addInstruction("STELEM " + arrayRefType->elementType()->vmType());
}
