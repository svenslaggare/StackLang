#include "ast.h"
#include <stdexcept>

void AbstractSyntaxTree::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	mSymbolTable = symbolTable;
}

std::ostream& operator<<(std::ostream& os, const AbstractSyntaxTree& ast) {
	os << ast.asString();
	return os;
}