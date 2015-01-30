#include "symbol.h"
#include "type.h"

//Symbol
Symbol::Symbol(std::string name, std::string type)
	: mName(name), mType(type) {

}

std::string Symbol::name() const {
	return mName;
}

std::string Symbol::type() const {
	return mType;
}

std::string Symbol::asString() const {
	return "(" + mName + ", " + mType + ")";
}

#include <iostream>

//Variable
VariableSymbol::VariableSymbol(std::string name, std::string variableType, bool isFunctionParameter)
	: Symbol(name, "Variable"), mVariableType(variableType), mIsFunctionParameter(isFunctionParameter) {

}

std::string VariableSymbol::asString() const {
	return Symbol::asString() + ": " + mVariableType;
}

std::string VariableSymbol::variableType() const {
	return mVariableType;
}

bool VariableSymbol::isFunctionParameter() const {
	return mIsFunctionParameter;
}

//Function
FunctionSymbol::FunctionSymbol(std::string name, std::vector<std::shared_ptr<VariableSymbol>> parameters, std::string returnType)
	: Symbol(name, "Function"), mParameters(parameters), mReturnType(returnType) {

}

std::string FunctionSymbol::asString() const {
	return Symbol::asString() + ": " + std::to_string(mParameters.size()) + ", " + mReturnType;
}

const std::vector<std::shared_ptr<VariableSymbol>>& FunctionSymbol::parameters() const {
	return mParameters;
}

std::string FunctionSymbol::returnType() const {
	return mReturnType;
}