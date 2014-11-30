#include "expressionast.h"
#include "symboltable.h"
#include "binder.h"
#include "functionast.h"

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
		if (std::dynamic_pointer_cast<FunctionAST>(symbol) == nullptr) {
			binder.error("'" + functionName() + "' is a function.");
		}
	}
}