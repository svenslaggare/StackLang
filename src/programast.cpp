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
		auto funcName = func->prototype()->name();
		if (!symbolTable->add(funcName, func)) {
			binder.error("The symbol '" + funcName + "' is already defined.");
		}
	}

	for (auto func : mFunctions) {
		func->generateSymbols(binder, symbolTable);
	}
}

void ProgramAST::typeCheck(TypeChecker& checker) {
	for (auto func : mFunctions) {
		func->typeCheck(checker);
	}
}