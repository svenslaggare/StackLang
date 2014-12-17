#include "function.h"

Parameter::Parameter(std::shared_ptr<Type> type, std::string name)
	: mType(type), mName(name) {

}

std::shared_ptr<Type> Parameter::type() {
	return mType;
}

std::string Parameter::name() const {
	return mName;
}

Function::Function(std::string name, std::vector<Parameter> parameters, std::shared_ptr<Type> returnType)
	: mName(name), mParameters(parameters), mReturnType(returnType) {

}

Function::Function()
	: mName(""), mParameters({}), mReturnType(nullptr) {

}

std::string Function::name() const {
	return mName;
}

const std::vector<Parameter>& Function::parameters() const {
	return mParameters;
}

std::shared_ptr<Type> Function::returnType() const {
	return mReturnType;
}