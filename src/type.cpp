#include "type.h"
#include "helpers.h"
#include <regex>

//Type
Type::Type(std::string name, bool isReferenceType)
	: mName(name), mIsReferenceType(isReferenceType) {

}

const std::string Type::name() const {
	return mName;
}

const bool Type::isReferenceType() const {
	return mIsReferenceType;
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

std::string Type::vmTypeName() const {
	return Helpers::join<std::string>(
		Helpers::splitString(mName, "::"),
		[](std::string str) { return str; },
		".");
}

//Auto
AutoType::AutoType()
	: Type("Auto") {

}

//Primitive
PrimitiveType::PrimitiveType(PrimitiveTypes type)
	: Type(TypeSystem::toString(type)) {

}

ReferenceType::ReferenceType(std::string name)
	: Type(name, true) {

}

//Null type
NullReferenceType::NullReferenceType()
	: ReferenceType("NullRef") {

}

std::string NullReferenceType::vmType() const {
	return "Ref.Null";
}

//Array type
ArrayType::ArrayType(std::shared_ptr<Type> elementType)
	: ReferenceType(elementType->name() + "[]"), mElementType(elementType) {

}

std::shared_ptr<Type> ArrayType::elementType() const {
	return mElementType;
}

std::string ArrayType::vmType() const {
	return "Ref.Array[" + elementType()->vmType() + "]";
}

//Class type
ClassType::ClassType(std::string name)
	: ReferenceType(name) {

}

std::string ClassType::vmType() const {
	return "Ref.Class." + Helpers::replaceString(name(), "::", ".");
}

std::string ClassType::vmClassName() const {
	return Helpers::replaceString(name(), "::", ".");
}

Types TypeSystem::defaultTypes() {
	auto intType = std::make_shared<PrimitiveType>(PrimitiveTypes::Int);
	auto boolType = std::make_shared<PrimitiveType>(PrimitiveTypes::Bool);
	auto floatType = std::make_shared<PrimitiveType>(PrimitiveTypes::Float);
	auto voidType = std::make_shared<PrimitiveType>(PrimitiveTypes::Void);
	auto charType = std::make_shared<PrimitiveType>(PrimitiveTypes::Char);
	auto nullType = std::make_shared<NullReferenceType>();

	return {
		{ "var", std::make_shared<AutoType>() },
		{ intType->name(), intType },
		{ boolType->name(), boolType },
		{ floatType->name(), floatType },
		{ voidType->name(), voidType },
		{ charType->name(), charType },
		{ nullType->name(), nullType },
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
	} else if (typeName == "Char") {
		type = PrimitiveTypes::Char;
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
	case PrimitiveTypes::Char:
		return "Char";
	}
}

std::shared_ptr<Type> TypeSystem::makeType(std::string typeName, const Types& definedTypes) {
	PrimitiveTypes primType;
	if (TypeSystem::fromString(typeName, primType)) {
		return std::make_shared<PrimitiveType>(PrimitiveType(primType));
	} else if (typeName.at(typeName.length() - 1) == ']' && typeName.at(typeName.length() - 2) == '[') {
		auto elementType = makeType(typeName.substr(0, typeName.length() - 2), definedTypes);

		if (elementType == nullptr) {
			return nullptr;
		}

		return std::make_shared<ArrayType>(elementType);
	} else if (typeName == "NullRef") {
		return std::make_shared<NullReferenceType>();
	} else {
		//Else check in the defined types
		if (definedTypes.count(typeName) > 0) {
			return definedTypes.at(typeName);
		}
	}

	return nullptr;
}

std::string TypeSystem::fromVMType(std::string vmType) {
	//Split the type name
	std::string token;
	std::vector<std::string> typeParts;

	bool isInsideBrackets = false;
	for (char c : vmType) {
		if (!isInsideBrackets) {
			if (c == '[') {
				isInsideBrackets = true;
			}
		} else {
			if (c == ']') {
				isInsideBrackets = false;
			}
		}

		if (c == '.' && !isInsideBrackets) {
			typeParts.push_back(token);
			token = "";
		} else {
			token += c;
		}
	}

	typeParts.push_back(token);

	std::string arrayPattern = "Array.(.*).";
	std::regex arrayRegex(arrayPattern, std::regex_constants::extended);
	PrimitiveTypes primType;

	if (fromString(typeParts.at(0), primType)) {
		return typeParts.at(0);
	} else if (typeParts.at(0) == "Ref") {
		std::smatch match;
		bool foundArray = std::regex_match(typeParts.at(1), match, arrayRegex);

		if (foundArray) {
			std::string elementType = match[1].str();
			elementType = elementType.substr(1, elementType.length() - 2);
			return fromVMType(elementType) + "[]";
		} else if (typeParts[1] == "Class") {
			std::string className = "";
			bool isFirst = true;

			for (int i = 2; i < typeParts.size(); i++) {
				if (isFirst) {
					isFirst = false;
				} else {
					className += ".";
				}

				className += typeParts[i];
			}
			
			return className;
		} else if (typeParts.at(1) == "Null") {
			return NullReferenceType().name();
		}
	}

	return "";
}