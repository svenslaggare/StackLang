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
#include "standardlibrary.h"

int main() {
	auto intType = std::make_shared<PrimitiveType>(PrimitiveTypes::Int);
	auto boolType = std::make_shared<PrimitiveType>(PrimitiveTypes::Bool);
	auto floatType = std::make_shared<PrimitiveType>(PrimitiveTypes::Float);
	auto voidType = std::make_shared<PrimitiveType>(PrimitiveTypes::Void);

	OperatorContainer operators(
		{
			{ Operator('<'), 5 }, { Operator('>'), 5 }, { Operator('+'), 6 }, { Operator('-'), 6 },
			{ Operator('*'), 7 }, { Operator('/'), 7 }, { Operator('='), 1 },
			{ Operator('<', '='), 5 }, { Operator('>', '='), 5 }, 
			{ Operator('=', '='), 4 }, { Operator('!', '='), 4 }, { Operator('&', '&'), 3 }, { Operator('|', '|'), 2 },
			{ Operator('+', '='), 1 }, { Operator('-', '='), 1 }, { Operator('*', '='), 1 }, { Operator('/', '='), 1 },
		},
		{ Operator('!'), Operator('-') },
		{ '+', '-', '*', '/' },
		{
			{ Operator('<'), boolType }, { Operator('>'), boolType }, { Operator('<', '='), boolType }, { Operator('>', '='), boolType }, 
			{ Operator('=', '='), boolType }, { Operator('!', '='), boolType }, { Operator('&', '&'), boolType }, { Operator('|', '|'), boolType }, 
		});

	Lexer lexer(operators.operatorChars());
	std::fstream programText("programs/program5.txt");
	auto tokens = lexer.tokenize(programText); 

	Parser parser(operators, tokens);
	auto programAST = parser.parse();

	programAST->rewrite();
	// std::cout << *programAST << std::endl;

	Binder binder;
	TypeChecker typeChecker(operators, TypeSystem::defaultTypes());
	SemanticVerifier verifier(binder, typeChecker);
	CodeGenerator codeGenerator(typeChecker);

	StandardLibrary::add(binder, typeChecker);
	binder.generateSymbolTable(programAST);

	programAST->typeCheck(typeChecker);
	programAST->verify(verifier);

	codeGenerator.generateProgram(programAST);
	codeGenerator.printGeneratedCode();
}