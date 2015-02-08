#include "objectast.h"
#include "expressionast.h"
#include "typechecker.h"
#include "binder.h"
#include "symbol.h"
#include "symboltable.h"
#include "type.h"
#include "codegenerator.h"

//Member access
MemberAccessAST::MemberAccessAST(std::shared_ptr<ExpressionAST> accessExpression, std::shared_ptr<ExpressionAST> memberExpression)
	: mAccessExpression(accessExpression), mMemberExpression(memberExpression) {

}

std::shared_ptr<ExpressionAST> MemberAccessAST::accessExpression() const {
	return mAccessExpression;
}

std::shared_ptr<ExpressionAST> MemberAccessAST::memberExpression() const {
	return mMemberExpression;
}

std::string MemberAccessAST::asString() const {
	return mAccessExpression->asString() + "." + mMemberExpression->asString();
}

void MemberAccessAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mAccessExpression->generateSymbols(binder, symbolTable);

	if (!std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)) {
		binder.error("'" + mMemberExpression->asString() + "' is not a member reference.");
	}
}

void MemberAccessAST::typeCheck(TypeChecker& checker) {
	mAccessExpression->typeCheck(checker);

	if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mAccessExpression)) {
		auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->varName()));
		auto varRefType = checker.findType(varSymbol->variableType());

		auto memberName = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)->varName();

		std::string objName = "";

		if (std::dynamic_pointer_cast<ArrayType>(varRefType)) {
			objName = "Array";
		}

		if (!checker.objectExists(objName)) {
			checker.typeError(varRefType->name() + " is not an object type.");
		}

		auto& object = checker.getObject(objName);

		if (!object.fieldExists(memberName)) {
			checker.typeError("There exists no field '" + memberName + "' in the type '" + varRefType->name() + "'.");
		}
	}
}

std::shared_ptr<Type> MemberAccessAST::expressionType(const TypeChecker& checker) const {
	auto memberName = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)->varName();
	std::string objName = "";

	std::shared_ptr<Type> varRefType = nullptr;

	if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mAccessExpression)) {
		auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->varName()));
		varRefType = checker.findType(varSymbol->variableType());
	} else {
		varRefType = mAccessExpression->expressionType(checker);
	}

	if (std::dynamic_pointer_cast<ArrayType>(varRefType)) {
		objName = "Array";
	}

	return checker.getObject(objName).getField(memberName).type();
}

void MemberAccessAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	auto memberName = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)->varName();
	auto typeRef = mAccessExpression->expressionType(codeGen.typeChecker());

	//Special handling of the 'length' field on arrays.
	if (memberName == "length" && std::dynamic_pointer_cast<ArrayType>(typeRef)) {
		mAccessExpression->generateCode(codeGen, func);
		func.addInstruction("LDLEN");
	} else {
		mAccessExpression->generateCode(codeGen, func);
		func.addInstruction("LDFIELD " + typeRef->name() + "::" + memberName);
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
	mMemberExpression->generateSymbols(binder, findNamespaceTable(binder, symbolTable, mNamespaceExpression));
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