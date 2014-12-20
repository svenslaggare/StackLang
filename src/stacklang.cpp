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

int main() {
	Lexer lexer;
	std::fstream programText("programs/program2.txt");
	auto tokens = lexer.tokenize(programText); 

	Parser parser(tokens);
	auto programAST = parser.parse();

	programAST->rewrite();
	//std::cout << *programAST << std::endl;

	Binder binder;
	binder.addFunction("println", { { "Int", "x" } }, "Void");
	binder.generateSymbolTable(programAST);

	TypeChecker typeChecker({
		{ "Int", std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Int)) },
		{ "Bool", std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Bool)) },
		{ "Void", std::make_shared<PrimitiveType>(PrimitiveType(PrimitiveTypes::Void)) }
	});

	typeChecker.checkTypes(programAST);

	CodeGenerator codeGenerator(typeChecker);
	codeGenerator.generateProgram(programAST);
	codeGenerator.printGeneratedCode();
}