#include "programast.h"
#include "functionast.h"
#include "symboltable.h"
#include "binder.h"

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
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	for (auto func : mFunctions) {
		if (!symbolTable->add(func->name(), func)) {
			binder.error("The symbol '" + func->name() + "' is already defined.");
		}
	}

	for (auto func : mFunctions) {
		func->generateSymbols(binder, symbolTable);
	}
}