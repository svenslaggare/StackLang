#include "binder.h"
#include "programast.h"
#include "expressionast.h"
#include "functionast.h"
#include "symbol.h"

#include <stdexcept>
#include <vector>

Binder::Binder()
	: mSymbolTable(std::make_shared<SymbolTable>(SymbolTable())) {

}

void Binder::addFunction(std::string name, const std::vector<std::pair<std::string, std::string>>& parameters, std::string returnType) {
	std::vector<std::shared_ptr<VariableSymbol>> parameterDecls;

	for (auto param : parameters) {
		parameterDecls.push_back(std::make_shared<VariableSymbol>(param.second, param.first, true));
	}

	mSymbolTable->add(name, std::make_shared<FunctionSymbol>(name, parameterDecls, returnType));
}

void Binder::generateSymbolTable(std::shared_ptr<ProgramAST> programAST) {
	programAST->generateSymbols(*this, mSymbolTable);
}

std::shared_ptr<SymbolTable> Binder::symbolTable() const {
	return mSymbolTable;
}

void Binder::error(std::string message) {
	throw std::runtime_error(message);
}