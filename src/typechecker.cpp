#include "typechecker.h"
#include "type.h"
#include "programast.h"
#include <stdexcept>

ExplicitConversion::ExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType, ExplicitConversionFunction conversionGenerator)
	: mFromType(fromType), mToType(toType), mConversionGenerator(conversionGenerator) {

}

std::shared_ptr<Type> ExplicitConversion::fromType() const {
	return mFromType;
}

std::shared_ptr<Type> ExplicitConversion::toType() const {
	return mToType;
}

void ExplicitConversion::applyConversion(CodeGenerator& codeGen, GeneratedFunction& func) const {
	mConversionGenerator(codeGen, func);
}

TypeChecker::TypeChecker(const OperatorContainer& operators, std::map<std::string, std::shared_ptr<Type>> types)
	: mOperators(operators), mTypes(types) {

}

bool TypeChecker::tryMakeType(std::string name) {
	auto type = TypeSystem::makeType(name);

	if (type != nullptr) {
		mTypes.insert({ name, type });
		return true;
	} else {
		return false;
	}
}

const OperatorContainer& TypeChecker::operators() const {
	return mOperators;
}

std::shared_ptr<Type> TypeChecker::findType(std::string typeName) const {
	if (mTypes.count(typeName) > 0) {
		return mTypes.at(typeName);
	} else {
		return nullptr;
	}
}

std::shared_ptr<Type> TypeChecker::getType(std::string typeName) {
	if (mTypes.count(typeName) > 0) {
		return mTypes.at(typeName);
	} else {
		if (tryMakeType(typeName)) {
			return mTypes.at(typeName);
		} else {
			return nullptr;
		}
	}
}

bool TypeChecker::addType(std::shared_ptr<Type> type) {
	auto typeName = type->name();
	if (mTypes.count(typeName) == 0) {
		mTypes.insert({ typeName, type });
		return true;
	} else {
		return false;
	}
}

bool TypeChecker::typeExists(std::string name) const {
	return mTypes.count(name) > 0;
}

void TypeChecker::typeError(std::string message) {
	throw std::runtime_error(message);
}

bool TypeChecker::assertTypeExists(std::string name, bool allowAuto) {
	bool exists = typeExists(name);

	if (!exists) {
		exists = tryMakeType(name);
	}

	if (!allowAuto && name == "var") {
		exists = false;
	}

	if (!exists) {
		typeError("There is no type named '" + name + "'.");
		return false;
	}

	return true;
}

bool TypeChecker::assertNotVoid(const Type& type, std::string errorMessage) {
	if (type.name() == "Void") {
		if (errorMessage == "") {
			typeError("The Void type is not allowed.");
		} else {
			typeError(errorMessage);
		}
		return false;
	} else {
		return true;
	}
}

bool TypeChecker::assertSameType(const Type& expected, const Type& actual, std::string errorMessage) {
	if (expected != actual) {
		auto errorMsg = errorMessage;

		if (errorMsg != "") {
			errorMsg += ": ";
		}

		errorMsg += "Expected type '" + expected.name() + "' but got type '" + actual.name() + "'.";
		typeError(errorMsg);
		return false;
	}

	return true;
}

void TypeChecker::defineExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType, ExplicitConversionFunction conversionFunc) {
	mExplicitConversions.insert({ fromType, ExplicitConversion(fromType, toType, conversionFunc) });
}

bool TypeChecker::existsExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType) const {
	auto conversions = mExplicitConversions.find(fromType);

	for (auto it = conversions; it != mExplicitConversions.end(); ++it) {
		auto current = *it;

		if (*current.second.toType() == *toType) {
			return true;
		}
	}

	return false;
}

const ExplicitConversion& TypeChecker::getExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType) const {
	auto conversions = mExplicitConversions.find(fromType);

	for (auto it = conversions; it != mExplicitConversions.end(); ++it) {
		auto current = *it;

		if (*current.second.toType() == *toType) {
			return it->second;
		}
	}

	throw std::out_of_range("fromType or toType");
}

void TypeChecker::addObject(const Object& object) {
	if (mObjects.count(object.name()) == 0) {
		mObjects.insert({ object.name(), object });
	}
}

bool TypeChecker::objectExists(std::string name) const {
	return mObjects.count(name) > 0;
}

const Object& TypeChecker::getObject(std::string name) const {
	return mObjects.at(name);
}