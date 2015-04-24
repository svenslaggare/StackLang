#include "compiler.h"
#include "type.h"
#include "ast/programast.h"
#include "loader.h"

#include <iostream>
#include <fstream>

Compiler::Compiler(
	std::unique_ptr<OperatorContainer> operators, 
	std::unique_ptr<Binder> binder,
	std::unique_ptr<TypeChecker> typeChecker, 
	std::unique_ptr<SemanticVerifier> semanticVerifier,
	std::unique_ptr<CodeGenerator> codeGenerator) :
		mLexer(operators->operatorChars(), operators->twoCharOpChars()),
		mOperators(std::move(operators)),
		mBinder(std::move(binder)),
		mTypeChecker(std::move(typeChecker)),
		mSemanticVerifier(std::move(semanticVerifier)),
		mCodeGenerator(std::move(codeGenerator)) {

}

Compiler Compiler::create() {
	auto defaultTypes = TypeSystem::defaultTypes();

	auto intType = defaultTypes["Int"];
	auto boolType = defaultTypes["Bool"];
	auto floatType = defaultTypes["Float"];
	auto voidType = defaultTypes["Void"];

	auto operators = std::unique_ptr<OperatorContainer>(new OperatorContainer(
		{
			{ Operator('<'), 5 }, { Operator('>'), 5 }, { Operator('+'), 6 }, { Operator('-'), 6 },
			{ Operator('*'), 7 }, { Operator('/'), 7 }, { Operator('='), 1 },
			{ Operator('<', '='), 5 }, { Operator('>', '='), 5 }, 
			{ Operator('=', '='), 4 }, { Operator('!', '='), 4 }, { Operator('&', '&'), 3 }, { Operator('|', '|'), 2 },
			{ Operator('+', '='), 1 }, { Operator('-', '='), 1 }, { Operator('*', '='), 1 }, { Operator('/', '='), 1 },
			{ Operator('.'), 8 },
			{ Operator(':', ':'), 9 }
		},
		{ Operator('!'), Operator('-') },
		{ '+', '-', '*', '/' },
		{
			{ Operator('<'), boolType }, { Operator('>'), boolType }, { Operator('<', '='), boolType }, { Operator('>', '='), boolType }, 
			{ Operator('=', '='), boolType }, { Operator('!', '='), boolType }, { Operator('&', '&'), boolType }, { Operator('|', '|'), boolType }, 
		}));

	auto binder = std::unique_ptr<Binder>(new Binder);

	auto typeChecker = std::unique_ptr<TypeChecker>(new TypeChecker(
		*binder.get(),
		*operators.get(),
		defaultTypes));

	auto semanticVerifier = std::unique_ptr<SemanticVerifier>(new SemanticVerifier(
		*binder.get(),
		*typeChecker.get()));

	auto codeGenerator = std::unique_ptr<CodeGenerator>(new CodeGenerator(
		*typeChecker.get()));

	//Add default objects
	typeChecker->addObject(Object("Array", nullptr, { { "length", Field("length", intType) } }));

	//Add conversions
	typeChecker->defineExplicitConversion(floatType, intType, [](CodeGenerator& codeGen, GeneratedFunction& func) {
		func.addInstruction("CONVFLOATTOINT");
	});

	typeChecker->defineExplicitConversion(intType, floatType, [](CodeGenerator& codeGen, GeneratedFunction& func) {
		func.addInstruction("CONVINTTOFLOAT");
	});

	return Compiler(
		std::move(operators),
		std::move(binder),
		std::move(typeChecker),
		std::move(semanticVerifier),
		std::move(codeGenerator));
}

const OperatorContainer& Compiler::operators() const {
	return *mOperators.get();
}

const Lexer& Compiler::lexer() const {
	return mLexer;
}

Binder& Compiler::binder() {
	return *mBinder.get();
}

TypeChecker& Compiler::typeChecker() {
	return *mTypeChecker.get();
}

SemanticVerifier& Compiler::semanticVerifier() {
	return *mSemanticVerifier.get();
}

CodeGenerator& Compiler::codeGenerator() {
	return *mCodeGenerator.get();
}

void Compiler::load() {
	//Load the runtime library
	Loader loader(binder(), typeChecker());
	std::fstream rtLibText("../StackJIT/rtlib/rtlib.sbc");
	loader.loadAssembly(rtLibText);

	std::fstream vectorLibText("rtlib/vector.sbc");
	loader.loadAssembly(vectorLibText);
}

void Compiler::process(std::shared_ptr<ProgramAST> programAST) {
	programAST->rewrite(*this);
	mBinder->generateSymbolTable(programAST);

	programAST->rewrite(*this);

	programAST->typeCheck(*mTypeChecker.get());
	programAST->verify(*mSemanticVerifier.get());

	mCodeGenerator->generateProgram(programAST);
	mCodeGenerator->printGeneratedCode();
}