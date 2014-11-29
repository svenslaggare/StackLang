#include "symboltable.h"
#include "ast.h"

SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> outer)
	: mOuter(outer) {

}

bool SymbolTable::add(std::string name, std::shared_ptr<AbstractSyntaxTree> symbol) {
	if (mInner.count(name) == 0) {
		mInner.insert({ name, symbol });
		return true;
	}

	return false;
}

std::shared_ptr<AbstractSyntaxTree> SymbolTable::find(std::string name) const {
	//First check in the inner
	if (mInner.count(name) > 0) {
		return mInner.at(name);
	}

	//Else in the outer
	if (mOuter != nullptr) {
		return mOuter->find(name);
	} else {
		return nullptr;
	}
}

const std::map<std::string, std::shared_ptr<AbstractSyntaxTree>>& SymbolTable::inner() const {
	return mInner;
}

std::shared_ptr<SymbolTable> SymbolTable::outer() const {
	return mOuter;
}

void printSymbolTable(std::ostream& os, const SymbolTable& symbolTable, int indentLevel = 0) {
	for (auto symbol : symbolTable.inner()) {
		for (int i = 0; i < indentLevel; i++) {
			os << "\t";
		}

		os << symbol.first << ": " << symbol.second->type() << std::endl;
	}

	if (symbolTable.outer() != nullptr) {
		printSymbolTable(os, symbolTable.outer(), indentLevel + 1);
	}
}

std::ostream& operator<<(std::ostream& os, const SymbolTable& symbolTable) {
	printSymbolTable(os, symbolTable);
	return os;
}