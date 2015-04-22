#include "compiler.h"
#include "lexer.h"
#include "parser.h"

#include <iostream>
#include <fstream>
#include <memory>

int main(int argc, char* argv[]) {
	auto compiler = Compiler::create();

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

	auto tokens = compiler.lexer().tokenize(programText); 

	Parser parser(compiler.operators(), tokens);
	auto programAST = parser.parse();

	//Loads libraries
	compiler.load();

	//Process the program
	compiler.process(programAST);
}