#include "programast.h"
#include "functionast.h"

ProgramAST::ProgramAST(const std::vector<std::shared_ptr<FunctionAST>>& functions)
	: mFunctions(functions) {

}

const std::vector<std::shared_ptr<FunctionAST>>& ProgramAST::functions() const {
	return mFunctions;
}

std::string ProgramAST::type() const {
	return "Program";
}

std::string ProgramAST::asString() const {
	return AST::combineAST(mFunctions, "\n\n");
}

void ProgramAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	for (auto func : mFunctions) {
		func->generateSymbols(binder, symbolTable);
	}
}