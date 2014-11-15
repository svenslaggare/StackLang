#include <iostream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

int main() {
	Lexer lexer;
	std::stringstream programText("func abs(x) = { return std_abs(x); } func troll(x, y) = { x = x + 2; return abs(-2, x) + 5; }");
	auto tokens = lexer.tokenize(programText); 

	Parser parser(tokens);
	auto program = parser.parse();

	std::cout << *program << std::endl;
}