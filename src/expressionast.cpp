#include "expressionast.h"
#include "symboltable.h"
#include "binder.h"
#include "functionast.h"
#include "typechecker.h"
#include "type.h"
#include "asthelpers.h"
#include "codegenerator.h"

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

std::shared_ptr<Type> IntegerExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.getType("Int");
}

void IntegerExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	func.addInstruction("PUSH " + std::to_string(mValue));
}

//Bool expression AST
BoolExpressionAST::BoolExpressionAST(bool value)
	: mValue(value) {

}

bool BoolExpressionAST::value() const {
	return mValue;
}

std::string BoolExpressionAST::asString() const {
	if (mValue) {
		return "true";
	} else {
		return "false";
	}
}

std::shared_ptr<Type> BoolExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.getType("Bool");
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

void VariableReferenceExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	auto symbol = symbolTable->find(varName());

	if (symbol == nullptr) {
		binder.error("The variable '" + varName() + "' is not defined.");
	} else {
		if (std::dynamic_pointer_cast<VariableDeclerationExpressionAST>(symbol) == nullptr) {
			binder.error("'" + varName() + "' is a variable.");
		}
	}
}

std::shared_ptr<Type> VariableReferenceExpressionAST::expressionType(const TypeChecker& checker) const {
	auto expressionSymbol = std::dynamic_pointer_cast<ExpressionAST>(mSymbolTable->find(varName()));

	if (expressionSymbol != nullptr) {
		return expressionSymbol->expressionType(checker);
	} else {
		return checker.getType("Void");
	}
}

void VariableReferenceExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	bool isFuncParam = std::dynamic_pointer_cast<VariableDeclerationExpressionAST>(mSymbolTable->find(varName()))->isFunctionParameter();

	if (isFuncParam) {
		func.addInstruction("LDARG " + std::to_string(func.functionParameterIndex(mVarName)));
	} else {

	}
}

//Variable decleration expression AST
VariableDeclerationExpressionAST::VariableDeclerationExpressionAST(std::string varType, std::string varName, bool isFunctionParameter)
	: mVarType(varType), mVarName(varName), mIsFunctionParameter(isFunctionParameter) {

}

std::string VariableDeclerationExpressionAST::varType() const {
	return mVarType;
}

std::string VariableDeclerationExpressionAST::varName() const {
	return mVarName;
}

bool VariableDeclerationExpressionAST::isFunctionParameter() const {
	return mIsFunctionParameter;
}

std::string VariableDeclerationExpressionAST::type() const {
	return "Variable";
}

std::string VariableDeclerationExpressionAST::asString() const {
	return mVarType + " " + mVarName;
}

void VariableDeclerationExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	if (!symbolTable->add(varName(), std::make_shared<VariableDeclerationExpressionAST>(*this))) {
		binder.error("The symbol '" + varName() + "' is already defined.");
	}
}

void VariableDeclerationExpressionAST::typeCheck(TypeChecker& checker) {
	checker.assertTypeExists(mVarType);
}

std::shared_ptr<Type> VariableDeclerationExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.getType(mVarType);
}

void VariableDeclerationExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {

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

	callStr += mFunctionName + "(" + AST::combineAST(mArguments, ", ") + ")";

	return callStr;
}

void CallExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	
	auto symbol = symbolTable->find(functionName());

	if (symbol == nullptr) {
		binder.error("The function '" + functionName() + "' is not defined.");
	} else {
		for (auto arg : mArguments) {
			arg->generateSymbols(binder, symbolTable);
		}

		auto func = std::dynamic_pointer_cast<FunctionAST>(symbol);
		auto funcPrototype = std::dynamic_pointer_cast<FunctionPrototypeAST>(symbol);

		if (func == nullptr && funcPrototype == nullptr) {
			binder.error("'" + functionName() + "' is not a function.");
		}

		if (func != nullptr) {
			funcPrototype = func->prototype();
		}

		if (funcPrototype->parameters().size() != arguments().size()) {
			binder.error("Expected " + std::to_string(funcPrototype->parameters().size()) + " arguments but got: " + std::to_string(arguments().size()));
		}
	}
}

void CallExpressionAST::typeCheck(TypeChecker& checker) {
	auto func = mSymbolTable->find(functionName());
	std::shared_ptr<FunctionPrototypeAST> funcPrototype = ASTHelpers::asPrototype(func);

	for (int i = 0; i < arguments().size(); i++) {
		auto arg = arguments()[i];
		arg->typeCheck(checker);

		auto param = funcPrototype->parameters()[i];

		auto argType = arg->expressionType(checker);
		auto paramType = param->expressionType(checker);

		checker.assertSameType(*paramType, *argType);
	}
}

std::shared_ptr<Type> CallExpressionAST::expressionType(const TypeChecker& checker) const {
	auto func = mSymbolTable->find(functionName());
	return checker.getType(ASTHelpers::asPrototype(func)->returnType());
}

void CallExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	for (auto arg : arguments()) {
		arg->generateCode(codeGen, func);
	}

	func.addInstruction("CALL " + mFunctionName);
}