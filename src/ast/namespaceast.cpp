#include "namespaceast.h"
#include "functionast.h"
#include "expressionast.h"
#include "classast.h"
#include "../symboltable.h"
#include "../symbol.h"
#include "../binder.h"
#include "../helpers.h"

//Namespace declaration
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

//Namespace access
NamespaceAccessAST::NamespaceAccessAST(std::shared_ptr<ExpressionAST> namespaceExpression, std::shared_ptr<ExpressionAST> memberExpression)
	: mNamespaceExpression(namespaceExpression), mMemberExpression(memberExpression) {

}

std::shared_ptr<ExpressionAST> NamespaceAccessAST::namespaceExpression() const {
	return mNamespaceExpression;
}

std::shared_ptr<ExpressionAST> NamespaceAccessAST::memberExpression() const {
	return mMemberExpression;
}

std::string NamespaceAccessAST::asString() const {
	return mNamespaceExpression->asString() + "::" + mMemberExpression->asString();
}

void NamespaceAccessAST::visit(VisitFn visitFn) const {
	mNamespaceExpression->visit(visitFn);
	mMemberExpression->visit(visitFn);
	visitFn(this);
}

void NamespaceAccessAST::rewrite() {
	std::shared_ptr<AbstractSyntaxTree> newNamespace;

	if (mNamespaceExpression->rewriteAST(newNamespace)) {
		mNamespaceExpression = std::dynamic_pointer_cast<ExpressionAST>(newNamespace);
	}

	std::shared_ptr<AbstractSyntaxTree> newMember;

	if (mMemberExpression->rewriteAST(newMember)) {
		mMemberExpression = std::dynamic_pointer_cast<ExpressionAST>(newMember);
	}

	mNamespaceExpression->rewrite();
	mMemberExpression->rewrite();
}

std::shared_ptr<SymbolTable> NamespaceAccessAST::findNamespaceTable(Binder& binder, std::shared_ptr<SymbolTable> symbolTable, std::shared_ptr<ExpressionAST> namespaceExpression) {
	if (auto namespaceExpr = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(namespaceExpression)) {
		auto namespaceName = namespaceExpr->varName();
		auto namespaceTable = std::dynamic_pointer_cast<NamespaceSymbol>(symbolTable->find(namespaceName));

		if (namespaceTable == nullptr) {
			binder.error("There exists no namespace named '" + namespaceName + "'.");
		}

		return namespaceTable->symbolTable();
	} else if (auto nestedNamespaceExpr = std::dynamic_pointer_cast<NamespaceAccessAST>(namespaceExpression)) {
		auto outerTable = findNamespaceTable(binder, symbolTable, nestedNamespaceExpr->namespaceExpression());

		auto nestedMemberExpr = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(nestedNamespaceExpr->memberExpression());
		auto namespaceName = nestedMemberExpr->varName();
		auto namespaceTable = std::dynamic_pointer_cast<NamespaceSymbol>(outerTable->find(namespaceName));

		if (namespaceTable == nullptr) {
			binder.error("There exists no namespace named '" + namespaceName + "'.");
		}

		return namespaceTable->symbolTable();
	} else { 
		binder.error("'" + namespaceExpression->asString() + "': not a namespace expression.");
		return nullptr;
	}
}

std::string NamespaceAccessAST::namespaceName(std::shared_ptr<SymbolTable> symbolTable, std::shared_ptr<ExpressionAST> namespaceExpression) const {
	if (auto namespaceExpr = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(namespaceExpression)) {
		auto namespaceName = namespaceExpr->varName();
		return namespaceName;
	} else if (auto nestedNamespaceExpr = std::dynamic_pointer_cast<NamespaceAccessAST>(namespaceExpression)) {
		auto nestedMemberExpr = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(nestedNamespaceExpr->memberExpression());
		return namespaceName(symbolTable, nestedNamespaceExpr->namespaceExpression()) + "." + nestedMemberExpr->varName();
	} else {
		return "";
	}
}

void NamespaceAccessAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	if (auto callMember = std::dynamic_pointer_cast<CallExpressionAST>(mMemberExpression)) {
		callMember->setCallTable(findNamespaceTable(binder, symbolTable, mNamespaceExpression));
	}

	mMemberExpression->generateSymbols(binder, mSymbolTable);
}

void NamespaceAccessAST::typeCheck(TypeChecker& checker) {
	mMemberExpression->typeCheck(checker);
}

std::shared_ptr<Type> NamespaceAccessAST::expressionType(const TypeChecker& checker) const {
	return mMemberExpression->expressionType(checker);
}

void NamespaceAccessAST::verify(SemanticVerifier& verifier) {
	mMemberExpression->verify(verifier);
}

void NamespaceAccessAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	auto mMemberFunc = std::dynamic_pointer_cast<CallExpressionAST>(mMemberExpression);
	mMemberFunc->generateCode(codeGen, func, namespaceName(mSymbolTable, mNamespaceExpression));
}