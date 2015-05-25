#include "compiler.h"
#include "parser.h"
#include <fstream>

int main(int argc, char* argv[]) {
	auto compiler = Compiler::create();

	std::string filePath = "";

	if (argc > 1) {
		filePath = argv[1];
	} else {
		throw std::runtime_error("No input files specified.");
	}

	std::vector<std::string> libraries;

	if (argc > 2) {
		for (int i = 2; i < argc; ++i) {
			std::string library = argv[i];

			if (library.find(".sbc") == library.length() - 4) {
				libraries.push_back(library);
			}
		}
	}

	std::fstream programText(filePath);

	if (!programText.is_open()) {
		throw std::runtime_error("Could not open file '" + filePath + "'.");
	}

	auto tokens = compiler.lexer().tokenize(programText); 

	Parser parser(compiler.operators(), tokens);
	auto programAST = parser.parse();

	//Loads libraries
	compiler.load(libraries);

	//Process the program
	compiler.process(programAST);
}