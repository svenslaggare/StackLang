#include "symbol.h"
#include "type.h"
#include "helpers.h"

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

std::string Symbol::scopeName() const {
	return mScopeName;
}

std::string Symbol::asString() const {
	return "(" + mName + ", " + mType + ")";
}

//Variable
VariableSymbol::VariableSymbol(std::string name, std::string variableType, VariableSymbolAttribute attribute, std::string className)
	: Symbol(name, "Variable"), mVariableType(variableType), mAttribute(attribute), mClassName(className) {

}

std::string VariableSymbol::asString() const {
	return Symbol::asString() + ": " + mVariableType;
}

std::string VariableSymbol::variableType() const {
	return mVariableType;
}

VariableSymbolAttribute VariableSymbol::attribute() const {
	return mAttribute;
}
std::string VariableSymbol::className() const {
	return mClassName;
}

//Function
FunctionSymbol::FunctionSymbol(std::string name, std::shared_ptr<FunctionSignatureSymbol> signature, Namespace definedNamespace)
	: Symbol(name, "Function"), mOverloads({ signature }), mDefinedNamespace(definedNamespace) {

}

const Namespace& FunctionSymbol::definedNamespace() const {
	return mDefinedNamespace;
}

std::string FunctionSymbol::asString() const {
	return Symbol::asString();
}

const std::vector<std::shared_ptr<FunctionSignatureSymbol>>& FunctionSymbol::overloads() const {
	return mOverloads;
}

bool FunctionSymbol::addOverload(std::shared_ptr<FunctionSignatureSymbol> signature) {
	std::vector<std::string> signatureParameters;

	for (auto param : signature->parameters()) {
		signatureParameters.push_back(param.variableType());
	}

	if (findOverload(signatureParameters) != nullptr) {
		return false;
	}

	mOverloads.push_back(signature);
	return true;
}

std::shared_ptr<FunctionSignatureSymbol> FunctionSymbol::findOverload(std::vector<std::string> parameterTypes) const {
	for (auto current : mOverloads) {
		if (current->parameters().size() == parameterTypes.size()) {
			bool found = true;

			for (int i = 0; i < current->parameters().size(); i++) {
				auto currentType = current->parameters().at(i).variableType();
				auto otherType = parameterTypes.at(i);

				if (currentType != otherType) {
					found = false;
					break;
				}
			}

			if (found) {
				return current;
			}
		}
	}

	return nullptr;
}

//Function signature
FunctionSignatureSymbol::FunctionSignatureSymbol(
	std::string name,
	std::vector<VariableSymbol> parameters,
	std::string returnType,
	AccessModifiers accessModifier)
	: Symbol(name, "FunctionSignature"), mParameters(parameters), mReturnType(returnType), mAccessModifier(accessModifier) {

}

std::string FunctionSignatureSymbol::asString() const {
	auto paramsStr = Helpers::join<VariableSymbol>(
		mParameters,
		[](VariableSymbol param) { return param.variableType(); },
		", ");

	return Symbol::asString() + ": " + name() + "(" + paramsStr + "): " + mReturnType;
}

const std::vector<VariableSymbol>& FunctionSignatureSymbol::parameters() const {
	return mParameters;
}

std::string FunctionSignatureSymbol::returnType() const {
	return mReturnType;
}

AccessModifiers FunctionSignatureSymbol::accessModifier() const {
	return mAccessModifier;
}

//Namespace
NamespaceSymbol::NamespaceSymbol(std::string name, std::shared_ptr<SymbolTable> symbolTable)
	: Symbol(name, "Namespace"), mSymbolTable(symbolTable) {

}

std::shared_ptr<SymbolTable> NamespaceSymbol::symbolTable() const {
	return mSymbolTable;
}

//Class
ClassSymbol::ClassSymbol(std::string name, std::shared_ptr<SymbolTable> symbolTable, Namespace definedNamespace)
	: Symbol(name, "Class"), mSymbolTable(symbolTable), mDefinedNamespace(definedNamespace) {

}

std::shared_ptr<SymbolTable> ClassSymbol::symbolTable() const {
	return mSymbolTable;
}

const Namespace& ClassSymbol::definedNamespace() const {
	return mDefinedNamespace;
}

std::string ClassSymbol::fullName() const {
	if (mDefinedNamespace.name() != "") {
		return mDefinedNamespace.name() + "::" + name();
	} else {
		return name();
	}
}