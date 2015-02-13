#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include "lexer.h"
#include "parser.h"
#include "asts.h"
#include "binder.h"
#include "type.h"
#include "typechecker.h"
#include "codegenerator.h"
#include "operators.h"
#include "semantics.h"
#include "builtin.h"
#include "loader.h"

int main(int argc, char* argv[]) {
	auto defaultTypes = TypeSystem::defaultTypes();

	auto intType = defaultTypes["Int"];
	auto boolType = defaultTypes["Bool"];
	auto floatType = defaultTypes["Float"];
	auto voidType = defaultTypes["Void"];

	OperatorContainer operators(
		{
			{ Operator('<'), 5 }, { Operator('>'), 5 }, { Operator('+'), 6 }, { Operator('-'), 6 },
			{ Operator('*'), 7 }, { Operator('/'), 7 }, { Operator('='), 1 },
			{ Operator('<', '='), 5 }, { Operator('>', '='), 5 }, 
			{ Operator('=', '='), 4 }, { Operator('!', '='), 4 }, { Operator('&', '&'), 3 }, { Operator('|', '|'), 2 },
			{ Operator('+', '='), 1 }, { Operator('-', '='), 1 }, { Operator('*', '='), 1 }, { Operator('/', '='), 1 },
			{ Operator(':', ':'), 8 }
		},
		{ Operator('!'), Operator('-') },
		{ '+', '-', '*', '/' },
		{
			{ Operator('<'), boolType }, { Operator('>'), boolType }, { Operator('<', '='), boolType }, { Operator('>', '='), boolType }, 
			{ Operator('=', '='), boolType }, { Operator('!', '='), boolType }, { Operator('&', '&'), boolType }, { Operator('|', '|'), boolType }, 
		});

	Lexer lexer(operators.operatorChars());
	std::string filePath = "";

	if (argc > 1) {
		filePath = argv[1];
	} else {
		throw std::runtime_error("No input files specified.");
	}

	std::fstream programText(filePath);

	if (!programText.is_open()) {
		throw std::runtime_error("Could not open file '" + filePath + "'.");
	}

	auto tokens = lexer.tokenize(programText); 

	Parser parser(operators, tokens);
	auto programAST = parser.parse();

	programAST->rewrite();
	// std::cout << *programAST << std::endl;

	Binder binder;
	TypeChecker typeChecker(binder, operators, defaultTypes);
	SemanticVerifier verifier(binder, typeChecker);
	CodeGenerator codeGenerator(typeChecker);

	//Load the runtime library
	Loader loader(binder, typeChecker);
	std::fstream assemblyText("../StackJIT/rtlib/rtlib.sbc");
	loader.loadAssembly(assemblyText);

	StackLang::Builtin::add(binder, typeChecker);
	binder.generateSymbolTable(programAST);
	//std::cout << "Generated symbol table." << std::endl;

	programAST->typeCheck(typeChecker);
	programAST->verify(verifier);

	//std::cout << "Typechecked and verfied." << std::endl;
	codeGenerator.generateProgram(programAST);
	codeGenerator.printGeneratedCode();
}