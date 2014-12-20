#include "typechecker.h"
#include "type.h"
#include "programast.h"
#include <stdexcept>

TypeChecker::TypeChecker(const OperatorContainer& operators, std::map<std::string, std::shared_ptr<Type>> types)
	: mOperators(operators), mTypes(types) {

}

const OperatorContainer& TypeChecker::operators() const {
	return mOperators;
}

std::shared_ptr<Type> TypeChecker::getType(std::string typeName) const {
	if (mTypes.count(typeName) > 0) {
		return mTypes.at(typeName);
	} else {
		return nullptr;
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

bool TypeChecker::assertTypeExists(std::string name) {
	if (!typeExists(name)) {
		typeError("There is no type named '" + name + "'.");
		return false;
	}

	return true;
}

bool TypeChecker::assertSameType(const Type& expected, const Type&  actual, std::string errorMessage) {
	if (expected != actual) {
		auto errorMsg = errorMessage;

		if (errorMsg != "") {
			errorMsg += ": ";
		}

		errorMsg += "Expected type '" + expected.name() + "' but got '" + actual.name() + "'.";
		typeError(errorMsg);
		return false;
	}

	return true;
}

void TypeChecker::checkTypes(std::shared_ptr<ProgramAST> program) {
	program->typeCheck(*this);
}