#include "type.h"

Type::Type(std::string name)
	: mName(name) {

}

const std::string Type::name() const {
	return mName;
}

bool Type::operator==(const Type& other) const {
	return mName == other.mName;
}

bool Type::operator!=(const Type& other) const {
	return !(*this == other);
}

std::string Type::vmType() const {
	return name();
}

AutoType::AutoType()
	: Type("Auto") {

}

PrimitiveType::PrimitiveType(PrimitiveTypes type)
	: Type(TypeSystem::toString(type)) {

}

ArrayType::ArrayType(std::shared_ptr<Type> elementType)
	: Type(elementType->name() + "[]"), mElementType(elementType) {

}

std::shared_ptr<Type> ArrayType::elementType() const {
	return mElementType;
}

std::string ArrayType::vmType() const {
	return "Ref.Array[" + elementType()->vmType() + "]";
}

std::map<std::string, std::shared_ptr<Type>> TypeSystem::defaultTypes() {
	auto intType = std::make_shared<PrimitiveType>(PrimitiveTypes::Int);
	auto boolType = std::make_shared<PrimitiveType>(PrimitiveTypes::Bool);
	auto floatType = std::make_shared<PrimitiveType>(PrimitiveTypes::Float);
	auto voidType = std::make_shared<PrimitiveType>(PrimitiveTypes::Void);

	return {
		{ "var", std::make_shared<AutoType>() },
		{ intType->name(), intType },
		{ boolType->name(), boolType },
		{ floatType->name(), floatType },
		{ voidType->name(), voidType }
	};
}

bool TypeSystem::fromString(std::string typeName, PrimitiveTypes& type) {
	if (typeName == "Int") {
		type = PrimitiveTypes::Int;
		return true;
	} else if (typeName == "Bool") {
		type = PrimitiveTypes::Bool;
		return true;
	} else if (typeName == "Float") {
		type = PrimitiveTypes::Float;
		return true;
	} else if (typeName == "Void") {
		type = PrimitiveTypes::Void;
		return true;
	} else {
		return false;
	}
}

std::string TypeSystem::toString(PrimitiveTypes type) {
	switch (type) {
	case PrimitiveTypes::Int:
		return "Int";
	case PrimitiveTypes::Bool:
		return "Bool";
	case PrimitiveTypes::Float:
		return "Float";
	case PrimitiveTypes::Void:
		return "Void";
	}
}

std::shared_ptr<Type> TypeSystem::makeType(std::string typeName) {
	PrimitiveTypes primType;
	if (TypeSystem::fromString(typeName, primType)) {
		return std::make_shared<PrimitiveType>(PrimitiveType(primType));
	} else if (typeName.at(typeName.length() - 1) == ']' && typeName.at(typeName.length() - 2) == '[') {
		auto elementType = makeType(typeName.substr(0, typeName.length() - 2));

		if (elementType == nullptr) {
			return nullptr;
		}

		return std::make_shared<ArrayType>(elementType);
	}

	return nullptr;
}