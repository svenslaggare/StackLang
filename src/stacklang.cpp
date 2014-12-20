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

int main() {
	auto boolType = std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Bool));

	OperatorContainer operators(
		{
			{ Operator('<'), 5 }, { Operator('>'), 5 }, { Operator('+'), 6 }, { Operator('-'), 6 },
			{ Operator('*'), 7 }, { Operator('/'), 7 }, { Operator('='), 1 },
			{ Operator('<', '='), 5 }, { Operator('>', '='), 5 }, 
			{ Operator('=', '='), 4 }, { Operator('!', '='), 4 }, { Operator('&', '&'), 3 }, { Operator('|', '|'), 2 },
			{ Operator('+', '='), 1 }, { Operator('-', '='), 1 }, { Operator('*', '='), 1 }, { Operator('/', '='), 1 },
		},
		{ Operator('!'), Operator('-'), Operator('+') },
		{ '+', '-', '*', '/' },
		{
			{ Operator('<'), boolType }, { Operator('>'), boolType }, { Operator('<', '='), boolType }, { Operator('>', '='), boolType }, 
			{ Operator('=', '='), boolType }, { Operator('!', '='), boolType }, { Operator('&', '&'), boolType }, { Operator('|', '|'), boolType }, 
		});

	Lexer lexer(operators.operatorChars());
	std::fstream programText("programs/program2.txt");
	auto tokens = lexer.tokenize(programText); 

	Parser parser(operators, tokens);
	auto programAST = parser.parse();

	programAST->rewrite();
	//std::cout << *programAST << std::endl;

	Binder binder;
	binder.addFunction("println", { { "Int", "x" } }, "Void");
	binder.generateSymbolTable(programAST);

	TypeChecker typeChecker(operators, {
		{ "Int", std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Int)) },
		{ "Bool", boolType },
		{ "Void", std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Void)) }
	});

	typeChecker.checkTypes(programAST);

	CodeGenerator codeGenerator(typeChecker);
	codeGenerator.generateProgram(programAST);
	codeGenerator.printGeneratedCode();
}