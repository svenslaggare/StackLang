#include <iostream>
#include <sstream>
#include <fstream>
#include "lexer.h"
#include "parser.h"
#include "asts.h"
#include "binder.h"

int main() {
	Lexer lexer;
	std::fstream programText("programs/program2.txt");
	auto tokens = lexer.tokenize(programText); 

	Parser parser(tokens);
	auto program = parser.parse();

	std::cout << *program << std::endl;

	Binder binder;
	binder.generateSymbolTable(program);
}