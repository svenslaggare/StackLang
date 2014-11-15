#include <iostream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

int main() {
	Lexer lexer;
	std::stringstream programText("func print() = { for (x = 0; x < 10; x = x + 1) { println(x); } } func abs(x) = { if (x < 0) { return -x; } else { return x; } } func troll(x, y) = { x = x + 2; return abs(-2, x) + 5; }");
	auto tokens = lexer.tokenize(programText); 

	Parser parser(tokens);
	auto program = parser.parse();

	std::cout << *program << std::endl;
}