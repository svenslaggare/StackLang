#include "programast.h"
#include "namespaceast.h"
#include "expressionast.h"
#include "functionast.h"
#include "symboltable.h"
#include "binder.h"
#include "symbol.h"
#include "helpers.h"

ProgramAST::ProgramAST(const std::vector<std::shared_ptr<NamespaceDeclarationAST>>& namespaces)
	: mNamespaces(namespaces) {

}

const std::vector<std::shared_ptr<NamespaceDeclarationAST>>& ProgramAST::namespaces() const {
	return mNamespaces;
}

void ProgramAST::visitFunctions(VisitFunctionsFn visitFn, std::shared_ptr<NamespaceDeclarationAST> currentNamespace, std::string outerNamespaceName) const {
	for (auto currentMember : currentNamespace->members()) {
		if (auto funcMember = std::dynamic_pointer_cast<FunctionAST>(currentMember)) {
			auto funcName = outerNamespaceName != "" ? outerNamespaceName + "." +funcMember->prototype()->name() : funcMember->prototype()->name();
			visitFn(funcName, funcMember);
		} else if (auto namespaceMember = std::dynamic_pointer_cast<NamespaceDeclarationAST>(currentMember)) {
			visitFunctions(visitFn, namespaceMember, outerNamespaceName + "." + namespaceMember->name());
		}
	}
}

void ProgramAST::visitFunctions(VisitFunctionsFn visitFn) const {
	for (auto currentNamespace : mNamespaces) {
		std::string name = "";

		if (currentNamespace->name() != "global") {
			name = currentNamespace->name();
		}

		visitFunctions(visitFn, currentNamespace, name);
	}
}

std::string ProgramAST::type() const {
	return "Program";
}

std::string ProgramAST::asString() const {
	return AST::combineAST(mNamespaces, "\n\n");
}

void ProgramAST::visit(VisitFn visitFn) const {
	for (auto currentNamespace : mNamespaces) {
		currentNamespace->visit(visitFn);
	}

	visitFn(this);
}

void ProgramAST::rewrite() {
	for (auto& curretNamespace : mNamespaces) {
		std::shared_ptr<AbstractSyntaxTree> newAST;

		if (curretNamespace->rewriteAST(newAST)) {
			curretNamespace = std::dynamic_pointer_cast<NamespaceDeclarationAST>(newAST);
		}

		curretNamespace->rewrite();
	}
}

void ProgramAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	for (auto currentNamespace : mNamespaces) {
		currentNamespace->generateSymbols(binder, symbolTable);
	}
}

void ProgramAST::typeCheck(TypeChecker& checker) {
	for (auto currentNamespace : mNamespaces) {
		currentNamespace->typeCheck(checker);
	}
}

void ProgramAST::verify(SemanticVerifier& verifier) {
	for (auto currentNamespace : mNamespaces) {
		currentNamespace->verify(verifier);
	}
}