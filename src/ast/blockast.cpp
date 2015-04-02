#include "blockast.h"
#include "../symboltable.h"

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

void BlockAST::rewrite(Compiler& compiler) {
	for (auto& statement : mStatements) {
		std::shared_ptr<AbstractSyntaxTree> newAST;

		if (statement->rewriteAST(newAST, compiler)) {
			statement = std::dynamic_pointer_cast<StatementAST>(newAST);
		}

		statement->rewrite(compiler);
	}
}

void BlockAST::visit(VisitFn visitFn) const {
	for (auto statement : mStatements) {
		statement->visit(visitFn);
	}

	visitFn(this);
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

void BlockAST::verify(SemanticVerifier& verifier) {
	for (auto statement : mStatements) {
		statement->verify(verifier);
	}
}

void BlockAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	for (auto statement : mStatements) {
		statement->generateCode(codeGen, func);
	}
}