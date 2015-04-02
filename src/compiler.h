#pragma once

class Binder;
class TypeChecker;
class SemanticVerifier;
class CodeGenerator;

//Represents a compiler
class Compiler {
private:
	Binder& mBinder;
	TypeChecker& mTypeChecker;
	SemanticVerifier& mSemanticVerifier;
	CodeGenerator& mCodeGenerator;
public:
	//Creates a new compiler
	Compiler(Binder& binder, TypeChecker& typeChecker, SemanticVerifier& semanticVerifier, CodeGenerator& codeGenerator);

	//Returns the binder
	Binder& binder();

	//Returns the type checker
	TypeChecker& typeChecker();

	//Returns the semantic verifier
	SemanticVerifier& semanticVerifier();

	//Returns the code generator
	CodeGenerator& codeGenerator();
};