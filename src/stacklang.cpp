#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include "lexer.h"
#include "parser.h"
#include "asts.h"
#include "binder.h"
#include "program.h"
#include "type.h"
#include "typechecker.h"
#include "codegenerator.h"
#include "operators.h"
#include "standardlibrary.h"

int main() {
	auto intType = std::make_shared<PrimitiveType>(PrimitiveTypes::Int);
	auto boolType = std::make_shared<PrimitiveType>(PrimitiveTypes::Bool);
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
	std::fstream programText("programs/program3.txt");
	auto tokens = lexer.tokenize(programText); 

	Parser parser(operators, tokens);
	auto programAST = parser.parse();

	programAST->rewrite();
	// std::cout << *programAST << std::endl;

	Binder binder;
	StandardLibrary::add(binder);
	binder.generateSymbolTable(programAST);

	TypeChecker typeChecker(operators, {
		{ "Int", intType },
		{ "Bool", boolType },
		{ "Void", voidType }
	});

	typeChecker.checkTypes(programAST);

	CodeGenerator codeGenerator(typeChecker);
	codeGenerator.generateProgram(programAST);
	codeGenerator.printGeneratedCode();
}