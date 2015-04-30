#include "typename.h"

TypeName::TypeName(std::string name)
	: mName(name), mIsArray(false) {

}

TypeName::TypeName(std::string name, std::unique_ptr<TypeName> elementTypeName)
	: mName(name), mElementTypeName(std::move(elementTypeName)) {

}

TypeName TypeName::make(std::string name) {
	return TypeName(name);
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