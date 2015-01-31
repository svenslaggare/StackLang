#pragma once
#include <string>

class Binder;
class TypeChecker;

//Represents a semantic verifier
class SemanticVerifier {
private:
	const Binder& mBinder;
	const TypeChecker& mTypeChecker;
public:
	//Creates a new semantic verifier
	SemanticVerifier(const Binder& binder, const TypeChecker& typeChecker);

	//Signals that a semantic error has occurred
	void semanticError(std::string message);

	//Returns the binder
	const Binder& binder() const;

	//Returns the type checker
	const TypeChecker& typeChecker() const;
};