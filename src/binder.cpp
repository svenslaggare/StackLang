#include "binder.h"
#include "ast/programast.h"
#include "ast/expressionast.h"
#include "ast/functionast.h"
#include "symbol.h"
#include "helpers.h"
#include "symboltable.h"

#include <stdexcept>
#include <vector>

Binder::Binder()
	: mSymbolTable(std::make_shared<SymbolTable>()) {

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