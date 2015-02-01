#include "arrayast.h"
#include "typechecker.h"
#include "type.h"
#include "symboltable.h"
#include "binder.h"
#include "symbol.h"
#include "codegenerator.h"

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
}
	
std::shared_ptr<Type> ArrayDeclarationAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(mElementType + "[]");
}

void ArrayDeclarationAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mLengthExpression->generateCode(codeGen, func);
	func.addInstruction("NEWARR " + codeGen.typeChecker().findType(mElementType)->vmType());
}

//Array access
ArrayAccessAST::ArrayAccessAST(std::string arrayName, std::shared_ptr<ExpressionAST> accessExpression)
	: mArrayName(arrayName), mAccessExpression(accessExpression) {
	
}

std::string ArrayAccessAST::arrayName() const {
	return mArrayName;
}

std::shared_ptr<ExpressionAST> ArrayAccessAST::accessExpression() const {
	return mAccessExpression;
}

std::string ArrayAccessAST::asString() const {
	return mArrayName + "[" + mAccessExpression->asString() + "]";
}

void ArrayAccessAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mAccessExpression->generateSymbols(binder, symbolTable);

	auto symbol = symbolTable->find(arrayName());

	if (symbol == nullptr) {
		binder.error("The variable '" + arrayName() + "' is not defined.");
	} else {
		if (std::dynamic_pointer_cast<VariableSymbol>(symbol) == nullptr) {
			binder.error("'" + arrayName() + "' is not a variable.");
		}
	}
}

void ArrayAccessAST::typeCheck(TypeChecker& checker) {
	mAccessExpression->typeCheck(checker);

	//Check access
	auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(arrayName()));
	auto varType = std::dynamic_pointer_cast<ArrayType>(checker.getType(varSymbol->variableType()));

	if (varType == nullptr) {
		checker.typeError("The variable '" + arrayName() + "' is not an array.");
	}

	checker.assertSameType(
		*checker.getType("Int"),
		*mAccessExpression->expressionType(checker),
		"Expected the array access to be of type 'Int'.");
}
	
std::shared_ptr<Type> ArrayAccessAST::expressionType(const TypeChecker& checker) const {
	auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(arrayName()));
	auto varType = std::dynamic_pointer_cast<ArrayType>(checker.findType(varSymbol->variableType()));
	return varType->elementType();
}

void ArrayAccessAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	auto varRefSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(arrayName()));
	auto varType = std::dynamic_pointer_cast<ArrayType>(codeGen.typeChecker().findType(varRefSymbol->variableType()));

	if (varRefSymbol->isFunctionParameter()) {
		func.addInstruction("LDARG " + std::to_string(func.functionParameterIndex(arrayName())));
	} else {
		func.addInstruction("LDLOC " + std::to_string(func.getLocal(arrayName()).first));
	}

	mAccessExpression->generateCode(codeGen, func);
	func.addInstruction("LDELEM " + varType->elementType()->vmType());
}
