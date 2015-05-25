#pragma once
#include "operators.h"
#include "binder.h"
#include "codegenerator.h"
#include "semantics.h"
#include "typechecker.h"
#include "lexer.h"
#include <memory>

class ProgramAST;

//Represents a compiler
class Compiler {
private:
	Lexer mLexer;
	std::unique_ptr<OperatorContainer> mOperators;
	std::unique_ptr<Binder> mBinder;
	std::unique_ptr<TypeChecker> mTypeChecker;
	std::unique_ptr<SemanticVerifier> mSemanticVerifier;
	std::unique_ptr<CodeGenerator> mCodeGenerator;

	//Creates a new compiler
	Compiler(
		std::unique_ptr<OperatorContainer> operators,
		std::unique_ptr<Binder> binder,
		std::unique_ptr<TypeChecker> typeChecker,
		std::unique_ptr<SemanticVerifier> semanticVerifier,
		std::unique_ptr<CodeGenerator> codeGenerator);
public:
	//Creates a new compiler
	static Compiler create();

	//Returns the defined operators
	const OperatorContainer& operators() const;

	//Returns the lexer
	const Lexer& lexer() const;

	//Returns the binder
	Binder& binder();

	//Returns the type checker
	TypeChecker& typeChecker();

	//Returns the semantic verifier
	SemanticVerifier& semanticVerifier();

	//Returns the code generator
	CodeGenerator& codeGenerator();

	//Loads libraries
	void load(std::vector<std::string> libraries = {});

	//Process the given program
	void process(std::shared_ptr<ProgramAST> programAST);
};