#include "arrayast.h"
#include "statementast.h"
#include "blockast.h"
#include "../typechecker.h"
#include "../type.h"
#include "../symboltable.h"
#include "../binder.h"
#include "../symbol.h"
#include "../codegenerator.h"
#include "../helpers.h"
#include "../typename.h"

//Array declaration
ArrayDeclarationAST::ArrayDeclarationAST(std::string elementType, std::shared_ptr<ExpressionAST> lengthExpression)
	: mElementType(TypeName::make(elementType)), mLengthExpression(lengthExpression) {
	
}

std::string ArrayDeclarationAST::elementType() const {
	return mElementType->name();
}

std::shared_ptr<ExpressionAST> ArrayDeclarationAST::lengthExpression() const {
	return mLengthExpression;
}

std::string ArrayDeclarationAST::asString() const {
	return "new " + elementType() + "[" + mLengthExpression->asString() + "]";
}

void ArrayDeclarationAST::rewrite(Compiler& compiler) {
	std::shared_ptr<AbstractSyntaxTree> newLength;
	while (mLengthExpression->rewriteAST(newLength, compiler)) {
		mLengthExpression = std::dynamic_pointer_cast<ExpressionAST>(newLength);
	}

	mLengthExpression->rewrite(compiler);
}

void ArrayDeclarationAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mLengthExpression->generateSymbols(binder, symbolTable);
	mElementType = std::move(TypeName::makeFull(mElementType.get(), symbolTable));
}

void ArrayDeclarationAST::typeCheck(TypeChecker& checker) {
	mLengthExpression->typeCheck(checker);

	//Check length
	checker.assertSameType(
		*checker.getType("Int"),
		*mLengthExpression->expressionType(checker),
		"Expected the length to be of type 'Int'.");

	//Check if the element type exists
	checker.assertTypeExists(elementType(), false);
	checker.assertNotVoid(*checker.findType(elementType()), "Arrays of type 'Void' is not allowed.");

	//Create the array type if not created
	checker.getType(elementType() + "[]");
}
	
std::shared_ptr<Type> ArrayDeclarationAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(elementType() + "[]");
}

void ArrayDeclarationAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mLengthExpression->generateCode(codeGen, func);
	func.addInstruction("NEWARR " + codeGen.typeChecker().findType(elementType())->vmType());
}

//Multidim array declaration
MultiDimArrayDeclarationAST::MultiDimArrayDeclarationAST(std::string elementType, std::vector<std::shared_ptr<ExpressionAST>> lengthExpressions)
	: mElementType(TypeName::make(elementType)), mLengthExpressions(lengthExpressions) {

}

std::string MultiDimArrayDeclarationAST::typeString(int dim) const {
	std::string arrayRankStr = "";
	
	if (dim == -1) {
		dim = mLengthExpressions.size();
	}

	for (int i = 0; i < dim; i++) {
		arrayRankStr += "[]";
	}

	return elementType() + arrayRankStr;
}

std::string MultiDimArrayDeclarationAST::elementType() const {
	return mElementType->name();
}

const std::vector<std::shared_ptr<ExpressionAST>>& MultiDimArrayDeclarationAST::lengthExpressions() const {
	return mLengthExpressions;
}

std::string MultiDimArrayDeclarationAST::asString() const {
	auto lengthsStr = Helpers::join<std::shared_ptr<ExpressionAST>>(
		mLengthExpressions,
		[](std::shared_ptr<ExpressionAST> length) { return length->asString(); },
		", ");

	return "new " + elementType() + "[" + lengthsStr + "]";
}

void MultiDimArrayDeclarationAST::rewrite(Compiler& compiler) {
	for (auto& lengthExpr : mLengthExpressions) {
		std::shared_ptr<AbstractSyntaxTree> newLength;
		while (lengthExpr->rewriteAST(newLength, compiler)) {
			lengthExpr = std::dynamic_pointer_cast<ExpressionAST>(newLength);
		}

		lengthExpr->rewrite(compiler);
	}
}

void MultiDimArrayDeclarationAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	
	for (auto lengthExpr : mLengthExpressions) {
		lengthExpr->generateSymbols(binder, symbolTable);
	}

	mElementType = std::move(TypeName::makeFull(mElementType.get(), symbolTable));
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
	checker.assertTypeExists(elementType(), false);
	checker.assertNotVoid(*checker.findType(elementType()), "Arrays of type 'Void' is not allowed.");

	//Create all array types
	for (int i = 0; i < mLengthExpressions.size(); i++) {
		checker.getType(typeString(i));
	}
}
	
std::shared_ptr<Type> MultiDimArrayDeclarationAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(typeString());
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
	func.addInstruction("BLE");

	//Body
	func.addInstruction("LDLOC " + std::to_string(outerLocal));
	func.addInstruction("LDLOC " + std::to_string(subArrayLocal));
	mLengthExpressions.at(1)->generateCode(codeGen, func);
	func.addInstruction("NEWARR " + typeChecker.findType(typeString(mLengthExpressions.size() - 2))->vmType());

	func.addInstruction("STELEM " + typeChecker.findType(typeString(mLengthExpressions.size() - 1))->vmType());

	func.addInstruction("LDLOC " + std::to_string(subArrayLocal));
	func.addInstruction("LDINT 1");
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

void ArrayAccessAST::rewrite(Compiler& compiler) {
	std::shared_ptr<AbstractSyntaxTree> newAccess;

	while (mAccessExpression->rewriteAST(newAccess, compiler)) {
		mAccessExpression = std::dynamic_pointer_cast<ExpressionAST>(newAccess);
	}

	std::shared_ptr<AbstractSyntaxTree> newMember;

	while (mArrayRefExpression->rewriteAST(newMember, compiler)) {
		mArrayRefExpression = std::dynamic_pointer_cast<ExpressionAST>(newMember);
	}

	mAccessExpression->rewrite(compiler);
	mArrayRefExpression->rewrite(compiler);
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

void ArrayAccessAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func, std::shared_ptr<Type> elementType) {
	if (elementType == nullptr) {
		mArrayRefExpression->generateCode(codeGen, func);
		elementType = std::dynamic_pointer_cast<ArrayType>(mArrayRefExpression->expressionType(codeGen.typeChecker()))
			->elementType();
	}

	mAccessExpression->generateCode(codeGen, func);
	func.addInstruction("LDELEM " + elementType->vmType());
}

void ArrayAccessAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	generateCode(codeGen, func, nullptr);
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

void ArraySetElementAST::rewrite(Compiler& compiler) {
	std::shared_ptr<AbstractSyntaxTree> newArrayRef;
	while (mArrayRefExpression->rewriteAST(newArrayRef, compiler)) {
		mArrayRefExpression = std::dynamic_pointer_cast<ExpressionAST>(newArrayRef);
	}

	std::shared_ptr<AbstractSyntaxTree> newAccess;
	while (mAccessExpression->rewriteAST(newAccess, compiler)) {
		mAccessExpression = std::dynamic_pointer_cast<ExpressionAST>(newAccess);
	}

	std::shared_ptr<AbstractSyntaxTree> newRHS;
	while (mRightHandSide->rewriteAST(newRHS, compiler)) {
		mRightHandSide = std::dynamic_pointer_cast<ExpressionAST>(newRHS);
	}

	mAccessExpression->rewrite(compiler);
	mArrayRefExpression->rewrite(compiler);
	mRightHandSide->rewrite(compiler);
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

void ArraySetElementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func, std::shared_ptr<Type> elementType) {
	if (elementType == nullptr) {
		auto arrayRefType = std::dynamic_pointer_cast<ArrayType>(mArrayRefExpression->expressionType(codeGen.typeChecker()));
		mArrayRefExpression->generateCode(codeGen, func);
		elementType = arrayRefType->elementType();
	}

	mAccessExpression->generateCode(codeGen, func);
	mRightHandSide->generateCode(codeGen, func);
	func.addInstruction("STELEM " + elementType->vmType());
}

void ArraySetElementAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	generateCode(codeGen, func, nullptr);
}
