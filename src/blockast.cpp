#include "blockast.h"
#include "symboltable.h"

BlockAST::BlockAST(const std::vector<std::shared_ptr<StatementAST>>& statements)
	: mStatements(statements) {

}

const std::vector<std::shared_ptr<StatementAST>>& BlockAST::statements() const {
	return mStatements;
}

std::string BlockAST::asString() const {
	std::string blockStr = "";

	blockStr += "{\n";

	for (auto statement : mStatements) {
		blockStr += statement->asString() + "\n";
	}

	blockStr += "}";

	return blockStr;
}

void BlockAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	
	auto inner = SymbolTable::newInner(symbolTable);

	for (auto statement : mStatements) {
		statement->generateSymbols(binder, inner);
	}
}