#include "programast.h"
#include "functionast.h"
#include "expressionast.h"
#include "symboltable.h"
#include "binder.h"
#include "symbol.h"
#include "helpers.h"

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

void ProgramAST::rewrite() {
	for (auto& func : mFunctions) {
		std::shared_ptr<AbstractSyntaxTree> newAST;

		if (func->rewriteAST(newAST)) {
			func = std::dynamic_pointer_cast<FunctionAST>(newAST);
		}

		func->rewrite();
	}
}

void ProgramAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	for (auto func : mFunctions) {
		auto funcName = func->prototype()->name();
		std::vector<std::shared_ptr<VariableSymbol>> parameters;

		for (auto param : func->prototype()->parameters()) {
			parameters.push_back(std::make_shared<VariableSymbol>(param->varName(), param->varType(), true));
		}

		auto symbol = symbolTable->find(funcName);

		if (symbol != nullptr && std::dynamic_pointer_cast<FunctionSymbol>(symbol) == nullptr) {
			binder.error("The symbol '" + funcName + "' is already defined.");
		}

		if (!symbolTable->addFunction(funcName, parameters, func->prototype()->returnType())) {
			auto paramsStr = Helpers::join<std::shared_ptr<VariableSymbol>>(
				parameters,
				[](std::shared_ptr<VariableSymbol> param) { return param->variableType(); },
				", ");

			binder.error("The already exists a function with the given signature: '" + funcName + "(" + paramsStr + ")" + "'.");
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

void ProgramAST::verify(SemanticVerifier& verifier) {
	for (auto func : mFunctions) {
		func->verify(verifier);
	}
}