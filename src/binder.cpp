#include "binder.h"
#include "programast.h"
#include "expressionast.h"
#include "functionast.h"
#include "symbol.h"
#include "helpers.h"

#include <stdexcept>
#include <vector>

Binder::Binder()
	: mSymbolTable(std::make_shared<SymbolTable>(SymbolTable())) {

}

void Binder::generateSymbolTable(std::shared_ptr<ProgramAST> programAST) {
	programAST->generateSymbols(*this, mSymbolTable);
}

void Binder::addFunction(std::string name, const std::vector<std::pair<std::string, std::string>>& parameters, std::string returnType) {
	std::vector<std::shared_ptr<VariableSymbol>> parameterSymbols;

	for (auto param : parameters) {
		parameterSymbols.push_back(std::make_shared<VariableSymbol>(param.second, param.first, true));
	}

	auto paramStr = Helpers::join<std::shared_ptr<VariableSymbol>>(
		parameterSymbols,
		[](std::shared_ptr<VariableSymbol> param) { return param->variableType(); },
		", ");
	
	mSymbolTable->add(name, std::make_shared<FunctionSymbol>(name, parameterSymbols, returnType));
}

std::shared_ptr<SymbolTable> Binder::symbolTable() const {
	return mSymbolTable;
}

void Binder::error(std::string message) {
	throw std::runtime_error(message);
}