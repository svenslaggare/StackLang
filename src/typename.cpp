#include "typename.h"
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

std::string TypeName::name() const {
	return mName;
}

bool TypeName::isArray() const {
	return mIsArray;
}

const TypeName* const TypeName::elementTypeName() const {
	return mElementTypeName.get();
}