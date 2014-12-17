#include "blockast.h"
#include "symboltable.h"

BlockAST::BlockAST(const std::vector<std::shared_ptr<StatementAST>>& statements)
	: mStatements(statements) {

}

const std::vector<std::shared_ptr<StatementAST>>& BlockAST::statements() const {
	return mStatements;
}

void BlockAST::setBlockTable(std::shared_ptr<SymbolTable> blockTable) {
	mBlockTable = blockTable;
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

std::shared_ptr<AbstractSyntaxTree> BlockAST::findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const {
	for (auto statement : mStatements) {
		if (predicate(statement)) {
			return statement;
		} else {
			if (auto ast = statement->findAST(predicate)) {
				return ast;
			}
		}
	}

	return nullptr;
}

void BlockAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	
	auto inner = SymbolTable::newInner(symbolTable);

	if (mBlockTable != nullptr) {
		inner->add(*mBlockTable);
	}

	for (auto statement : mStatements) {
		statement->generateSymbols(binder, inner);
	}
}

void BlockAST::typeCheck(TypeChecker& checker) {
	for (auto statement : mStatements) {
		statement->typeCheck(checker);
	}
}

void BlockAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	for (auto statement : mStatements) {
		statement->generateCode(codeGen, func);
	}
}