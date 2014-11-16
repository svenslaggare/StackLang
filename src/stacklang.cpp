#include <iostream>
#include <sstream>
#include <fstream>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

int main() {
	Lexer lexer;
	std::fstream programText("programs/program1.txt");
	auto tokens = lexer.tokenize(programText); 

	Parser parser(tokens);
	auto program = parser.parse();

	std::cout << *program << std::endl;
}