#include "namespaceast.h"
#include "functionast.h"
#include "expressionast.h"
#include "classast.h"
#include "symboltable.h"
#include "symbol.h"
#include "binder.h"
#include "helpers.h"

NamespaceDeclarationAST::NamespaceDeclarationAST(std::string name, std::vector<std::shared_ptr<AbstractSyntaxTree>> members)
	: mName(name), mMembers(members) {

}

std::string NamespaceDeclarationAST::name() const {
	return mName;
}

const std::vector<std::shared_ptr<AbstractSyntaxTree>>& NamespaceDeclarationAST::members() const {
	return mMembers;
}

std::string NamespaceDeclarationAST::type() const {
	return "Namespace";
}

std::string NamespaceDeclarationAST::asString() const {
	if (mName == "global") {
		return AST::combineAST(mMembers, "\n\n");
	} else {
		return "namespace " + mName + " {\n" + AST::combineAST(mMembers, "\n\n") + "\n}";
	}
}

void NamespaceDeclarationAST::visit(VisitFn visitFn) const {
	for (auto member : mMembers) {
		member->visit(visitFn);
	}

	visitFn(this);
}

void NamespaceDeclarationAST::rewrite() {
	for (auto& member : mMembers) {
		std::shared_ptr<AbstractSyntaxTree> newAST;
		
		if (member->rewriteAST(newAST)) {
			member = newAST;
		}

		member->rewrite();
	}
}

void NamespaceDeclarationAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	auto symbol = symbolTable->find(mName);
	std::shared_ptr<SymbolTable> namespaceTable = symbolTable;

	if (mName != "global") {
		if (symbol != nullptr) {
			auto namespaceSymbol = std::dynamic_pointer_cast<NamespaceSymbol>(symbol);

			if (namespaceSymbol == nullptr) {
				throw std::runtime_error("'" + mName + "' symbol is already defined.");
			}

			namespaceTable = namespaceSymbol->symbolTable();
		} else {
			namespaceTable = std::make_shared<SymbolTable>(symbolTable, mName);
			auto namespaceSymbol = std::make_shared<NamespaceSymbol>(mName, namespaceTable);
			symbolTable->add(mName, namespaceSymbol);
		}
	}

	for (auto member : mMembers) {
		if (auto func = std::dynamic_pointer_cast<FunctionAST>(member)) {
			//Declare functions
			auto funcName = func->prototype()->name();
			std::vector<VariableSymbol> parameters;

			for (auto param : func->prototype()->parameters()) {
				parameters.push_back(VariableSymbol(param->varName(), param->varType(), true));
			}

			auto symbol = namespaceTable->find(funcName);

			if (symbol != nullptr && std::dynamic_pointer_cast<FunctionSymbol>(symbol) == nullptr) {
				binder.error("The symbol '" + funcName + "' is already defined.");
			}

			if (!namespaceTable->addFunction(funcName, parameters, func->prototype()->returnType())) {
				auto paramsStr = Helpers::join<VariableSymbol>(
					parameters,
					[](VariableSymbol param) { return param.variableType(); },
					", ");

				binder.error("The already exists a function with the given signature: '" + funcName + "(" + paramsStr + ")" + "'.");
			}
		}
	}

	for (auto member : mMembers) {
		member->generateSymbols(binder, namespaceTable);
	}
}

void NamespaceDeclarationAST::typeCheck(TypeChecker& checker) {
	//Define classes
	for (auto member : mMembers) {
		 if (auto classDef = std::dynamic_pointer_cast<ClassDefinitionAST>(member)) {
			classDef->addClassDefinition(checker);
		}
	}

	for (auto member : mMembers) {
		member->typeCheck(checker);
	}
}

void NamespaceDeclarationAST::verify(SemanticVerifier& verifier) {
	for (auto member : mMembers) {
		member->verify(verifier);
	}
}