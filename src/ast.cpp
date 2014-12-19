#include "ast.h"
#include <stdexcept>
#include "typechecker.h"
#include "symboltable.h"

void AbstractSyntaxTree::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	mSymbolTable = symbolTable;
}

void AbstractSyntaxTree::typeCheck(TypeChecker& checker) {

}

std::shared_ptr<AbstractSyntaxTree> AbstractSyntaxTree::findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const {
	return nullptr;
}

bool AbstractSyntaxTree::rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST) const {
	return false;
}

std::ostream& operator<<(std::ostream& os, const AbstractSyntaxTree& ast) {
	os << ast.asString();
	return os;
}

std::shared_ptr<Type> ExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.getType("Void");
}