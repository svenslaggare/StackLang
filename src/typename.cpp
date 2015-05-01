#include "typename.h"
#include "symbol.h"
#include "helpers.h"
#include <iostream>

TypeName::TypeName(std::string name)
	: mName(name), mIsArray(false) {

}

TypeName::TypeName(std::string name, std::unique_ptr<TypeName> elementTypeName)
	: mName(name), mIsArray(true), mElementTypeName(std::move(elementTypeName)) {

}

std::unique_ptr<TypeName> TypeName::make(std::string name) {
	if (name.at(name.length() - 1) == ']' && name.at(name.length() - 2) == '[') {
		auto elementTypeName = make(name.substr(0, name.length() - 2));
		return std::unique_ptr<TypeName>(new TypeName(name, std::move(elementTypeName)));
	}

	return std::unique_ptr<TypeName>(new TypeName(name));
}

std::unique_ptr<TypeName> TypeName::makeFull(const TypeName* const typeName, std::shared_ptr<SymbolTable> symbolTable) {
	if (typeName->isArray()) {
		auto elementTypeName = makeFull(typeName->elementTypeName(), symbolTable);
		return std::unique_ptr<TypeName>(new TypeName(elementTypeName->name() + "[]", std::move(elementTypeName)));
	} else {
		std::string fullTypeName;

		auto typeSymbol = std::dynamic_pointer_cast<ClassSymbol>(Helpers::findSymbolInNamespace(symbolTable, typeName->name()));

		if (typeSymbol != nullptr) {
			fullTypeName = typeSymbol->fullName();
		} else {
			fullTypeName = typeName->name();
		}

		return std::unique_ptr<TypeName>(new TypeName(fullTypeName));
	}
}

std::string TypeName::name() const {
	return mName;
}

bool TypeName::isArray() const {
	return mIsArray;
}

const TypeName* const TypeName::elementTypeName() const {
	return mElementTypeName.get();
}