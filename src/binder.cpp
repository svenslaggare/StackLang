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
	std::vector<VariableSymbol> parameterSymbols;

	for (auto param : parameters) {
		parameterSymbols.push_back(VariableSymbol(param.second, param.first, true));
	}

	mSymbolTable->addFunction(name, parameterSymbols, returnType);
}

std::shared_ptr<SymbolTable> Binder::symbolTable() const {
	return mSymbolTable;
}

void Binder::error(std::string message) {
	throw std::runtime_error(message);
}