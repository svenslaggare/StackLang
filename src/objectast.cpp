#include "objectast.h"
#include "expressionast.h"
#include "typechecker.h"
#include "binder.h"
#include "symbol.h"
#include "symboltable.h"
#include "type.h"
#include "codegenerator.h"

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