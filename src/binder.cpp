#include "binder.h"
#include "programast.h"
#include <stdexcept>

Binder::Binder()
	: mSymbolTable(nullptr) {

}

void Binder::error(std::string message) {
	throw std::runtime_error(message);
}

void Binder::generateSymbolTable(std::shared_ptr<ProgramAST> programAST) {
	mSymbolTable = std::make_shared<SymbolTable>(SymbolTable());
	programAST->generateSymbols(*this, mSymbolTable);
}

std::shared_ptr<SymbolTable> Binder::symbolTable() const {
	return mSymbolTable;
}