#include "semantics.h"
#include <stdexcept>

SemanticVerifier::SemanticVerifier(const Binder& binder, const TypeChecker& typeChecker)
	: mBinder(binder), mTypeChecker(typeChecker) {

}

void SemanticVerifier::semanticError(std::string message) {
	throw std::runtime_error(message);
}

const Binder& SemanticVerifier::binder() const {
	return mBinder;
}

const TypeChecker& SemanticVerifier::typeChecker() const {
	return mTypeChecker;
}