#include "compiler.h"

Compiler::Compiler(Binder& binder, TypeChecker& typeChecker, SemanticVerifier& semanticVerifier, CodeGenerator& codeGenerator)
	: mBinder(binder), mTypeChecker(typeChecker), mSemanticVerifier(semanticVerifier), mCodeGenerator(codeGenerator) {

}

Binder& Compiler::binder() {
	return mBinder;
}

TypeChecker& Compiler::typeChecker() {
	return mTypeChecker;
}

SemanticVerifier& Compiler::semanticVerifier() {
	return mSemanticVerifier;
}

CodeGenerator& Compiler::codeGenerator() {
	return mCodeGenerator;
}