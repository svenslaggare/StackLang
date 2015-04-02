#include "ast.h"
#include "../typechecker.h"
#include "../symboltable.h"
#include "../binder.h"
#include <stdexcept>

void AbstractSyntaxTree::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	mSymbolTable = symbolTable;
}

void AbstractSyntaxTree::rewrite(Compiler& compiler) {

}

bool AbstractSyntaxTree::rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST, Compiler& compiler) const {
	return false;
}

void AbstractSyntaxTree::typeCheck(TypeChecker& checker) {

}

void AbstractSyntaxTree::verify(SemanticVerifier& verifier) {

}

void AbstractSyntaxTree::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {

}

std::ostream& operator<<(std::ostream& os, const AbstractSyntaxTree& ast) {
	os << ast.asString();
	return os;
}

std::shared_ptr<Type> ExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("Void");
}