#include "objectast.h"
#include "variableast.h"
#include "callast.h"
#include "arrayast.h"
#include "../typechecker.h"
#include "../binder.h"
#include "../symbol.h"
#include "../symboltable.h"
#include "../type.h"
#include "../codegenerator.h"

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

void MemberAccessAST::visit(VisitFn visitFn) const {
	mAccessExpression->visit(visitFn);
	mMemberExpression->visit(visitFn);
	visitFn(this);
}

bool MemberAccessAST::rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST, Compiler& compiler) const {
	if (auto callMember = std::dynamic_pointer_cast<CallExpressionAST>(mMemberExpression)) {
		newAST = std::make_shared<MemberCallExpressionAST>(
			mAccessExpression,
			callMember);
		return true;
	}

 	return false;
}

void MemberAccessAST::rewrite(Compiler& compiler) {
	std::shared_ptr<AbstractSyntaxTree> newAccess;

	while (mAccessExpression->rewriteAST(newAccess, compiler)) {
		mAccessExpression = std::dynamic_pointer_cast<ExpressionAST>(newAccess);
	}

	std::shared_ptr<AbstractSyntaxTree> newMember;

	while (mMemberExpression->rewriteAST(newMember, compiler)) {
		mMemberExpression = std::dynamic_pointer_cast<ExpressionAST>(newMember);
	}

	mAccessExpression->rewrite(compiler);
	mMemberExpression->rewrite(compiler);
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
		auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->name()));
		auto varRefType = checker.findType(varSymbol->variableType());

		auto memberName = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)->name();

		std::string objName = varRefType->name();

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
	auto memberName = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)->name();
	std::shared_ptr<Type> varRefType = nullptr;

	if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mAccessExpression)) {
		auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->name()));
		varRefType = checker.findType(varSymbol->variableType());
	} else {
		varRefType = mAccessExpression->expressionType(checker);
	}

	std::string objName = varRefType->name();

	if (std::dynamic_pointer_cast<ArrayType>(varRefType)) {
		objName = "Array";
	}

	return checker.getObject(objName).getField(memberName).type();
}

void MemberAccessAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	auto memberName = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)->name();
	auto typeRef = mAccessExpression->expressionType(codeGen.typeChecker());

	//Special handling of the 'length' field on arrays.
	if (memberName == "length" && std::dynamic_pointer_cast<ArrayType>(typeRef)) {
		mAccessExpression->generateCode(codeGen, func);
		func.addInstruction("LDLEN");
	} else {
		auto classTypeRef = std::dynamic_pointer_cast<ClassType>(typeRef);
		mAccessExpression->generateCode(codeGen, func);
		func.addInstruction("LDFIELD " + classTypeRef->vmClassName() + "::" + memberName);
	}
}

//Member call
MemberCallExpressionAST::MemberCallExpressionAST(std::shared_ptr<ExpressionAST> accessExpression, std::shared_ptr<CallExpressionAST> memberCallExpression)
	: mAccessExpression(accessExpression), mMemberCallExpression(memberCallExpression) {

}

std::shared_ptr<ExpressionAST> MemberCallExpressionAST::accessExpression() const {
	return mAccessExpression;
}

std::shared_ptr<CallExpressionAST> MemberCallExpressionAST::memberCallExpression() const {
	return mMemberCallExpression;
}

std::string MemberCallExpressionAST::asString() const {
	return mAccessExpression->asString() + "." + mMemberCallExpression->asString();
}

void MemberCallExpressionAST::visit(VisitFn visitFn) const {
	mAccessExpression->visit(visitFn);
	mMemberCallExpression->visit(visitFn);
	visitFn(this);
}

void MemberCallExpressionAST::rewrite(Compiler& compiler) {
	std::shared_ptr<AbstractSyntaxTree> newAccess;

	while (mAccessExpression->rewriteAST(newAccess, compiler)) {
		mAccessExpression = std::dynamic_pointer_cast<ExpressionAST>(newAccess);
	}

	std::shared_ptr<AbstractSyntaxTree> newMember;

	while (mMemberCallExpression->rewriteAST(newMember, compiler)) {
		mMemberCallExpression = std::dynamic_pointer_cast<CallExpressionAST>(newMember);
	}

	mAccessExpression->rewrite(compiler);
	mMemberCallExpression->rewrite(compiler);
}

void MemberCallExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mAccessExpression->generateSymbols(binder, symbolTable);
}

void MemberCallExpressionAST::typeCheck(TypeChecker& checker) {
	mAccessExpression->typeCheck(checker);

	if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mAccessExpression)) {
		auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->name()));
		auto varRefType = checker.findType(varSymbol->variableType());

		auto memberName = mMemberCallExpression->functionName();

		std::string objName = varRefType->name();

		if (std::dynamic_pointer_cast<ArrayType>(varRefType)) {
			objName = "Array";
		}

		if (!checker.objectExists(objName)) {
			checker.typeError(varRefType->name() + " is not an object type.");
		}

		auto classSymbol = std::dynamic_pointer_cast<ClassSymbol>(mSymbolTable->find(objName));
		mMemberCallExpression->setCallTable(classSymbol->symbolTable());
		mMemberCallExpression->generateSymbols(checker.binder(), mSymbolTable);
		mMemberCallExpression->typeCheck(checker);
	} else {
		auto varRefType = mAccessExpression->expressionType(checker);
		auto memberName = mMemberCallExpression->functionName();

		std::string objName = varRefType->name();

		if (std::dynamic_pointer_cast<ArrayType>(varRefType)) {
			objName = "Array";
		}

		if (!checker.objectExists(objName)) {
			checker.typeError(varRefType->name() + " is not an object type.");
		}

		auto classSymbol = std::dynamic_pointer_cast<ClassSymbol>(mSymbolTable->find(objName));
		mMemberCallExpression->setCallTable(classSymbol->symbolTable());
		mMemberCallExpression->generateSymbols(checker.binder(), mSymbolTable);
		mMemberCallExpression->typeCheck(checker);
	}
}

std::shared_ptr<Type> MemberCallExpressionAST::expressionType(const TypeChecker& checker) const {
	return mMemberCallExpression->expressionType(checker);
}

void MemberCallExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mAccessExpression->generateCode(codeGen, func);
	mMemberCallExpression->generateMemberCallCode(codeGen, func, mAccessExpression->expressionType(codeGen.typeChecker()));
}

//Set field value
SetFieldValueAST::SetFieldValueAST(
	std::shared_ptr<ExpressionAST> objectRefExpression,
	std::shared_ptr<ExpressionAST> memberExpression,
	std::shared_ptr<ExpressionAST> rightHandSide)
	: mObjectRefExpression(objectRefExpression), mMemberExpression(memberExpression), mRightHandSide(rightHandSide) {

}

std::shared_ptr<ExpressionAST> SetFieldValueAST::objectRefExpression() const {
	return mObjectRefExpression;
}

std::shared_ptr<ExpressionAST> SetFieldValueAST::memberExpression() const {
	return mMemberExpression;
}

std::shared_ptr<ExpressionAST> SetFieldValueAST::rightHandSide() const {
	return mRightHandSide;
}

std::string SetFieldValueAST::asString() const {
	return 
		mObjectRefExpression->asString() + "." + mMemberExpression->asString() + " = " + mRightHandSide->asString();
}

void SetFieldValueAST::visit(VisitFn visitFn) const {
	mObjectRefExpression->visit(visitFn);
	mMemberExpression->visit(visitFn);
	mRightHandSide->visit(visitFn);
	visitFn(this);
}

void SetFieldValueAST::rewrite(Compiler& compiler) {
	std::shared_ptr<AbstractSyntaxTree> newObjectRef;

	if (mObjectRefExpression->rewriteAST(newObjectRef, compiler)) {
		mObjectRefExpression = std::dynamic_pointer_cast<ExpressionAST>(newObjectRef);
	}

	std::shared_ptr<AbstractSyntaxTree> newMember;

	if (mMemberExpression->rewriteAST(newMember, compiler)) {
		mMemberExpression = std::dynamic_pointer_cast<ExpressionAST>(newMember);
	}

	std::shared_ptr<AbstractSyntaxTree> newRHS;

	if (mRightHandSide->rewriteAST(newRHS, compiler)) {
		mRightHandSide = std::dynamic_pointer_cast<ExpressionAST>(newRHS);
	}

	mObjectRefExpression->rewrite(compiler);
	mMemberExpression->rewrite(compiler);
	mRightHandSide->rewrite(compiler);
}

void SetFieldValueAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mObjectRefExpression->generateSymbols(binder, symbolTable);

	if (!std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)) {
		binder.error("'" + mMemberExpression->asString() + "' is not a member reference.");
	}

	mRightHandSide->generateSymbols(binder, symbolTable);
}

void SetFieldValueAST::typeCheck(TypeChecker& checker) {
	mObjectRefExpression->typeCheck(checker);

	std::shared_ptr<Type> objRefType;

	if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mObjectRefExpression)) {
		auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->name()));
		objRefType = checker.findType(varSymbol->variableType());
	} else if (auto arrayRef = std::dynamic_pointer_cast<ArrayAccessAST>(mObjectRefExpression)) {
		objRefType = arrayRef->expressionType(checker);
	} else {
		checker.typeError("Not implemented");
	}

	auto memberName = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)->name();

	std::string objName = objRefType->name();

	if (!checker.objectExists(objName)) {
		checker.typeError(objRefType->name() + " is not an object type.");
	}

	auto& object = checker.getObject(objName);

	if (!object.fieldExists(memberName)) {
		checker.typeError("There exists no field '" + memberName + "' in the type '" + objRefType->name() + "'.");
	}

	mRightHandSide->typeCheck(checker);

	//Check rhs
	checker.assertSameType(
		*object.getField(memberName).type(), 
		*mRightHandSide->expressionType(checker),
		asString());
}

std::shared_ptr<Type> SetFieldValueAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("Void");
}

void SetFieldValueAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	auto& checker = codeGen.typeChecker();

	std::shared_ptr<ClassType> objRefType;

	if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mObjectRefExpression)) {
		auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->name()));
		objRefType = std::dynamic_pointer_cast<ClassType>(checker.findType(varSymbol->variableType()));
	} else if (auto arrayRef = std::dynamic_pointer_cast<ArrayAccessAST>(mObjectRefExpression)) {
		objRefType = std::dynamic_pointer_cast<ClassType>(arrayRef->expressionType(checker));
	}

	auto memberName = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)->name();

	mObjectRefExpression->generateCode(codeGen, func);
	mRightHandSide->generateCode(codeGen, func);
	func.addInstruction("STFIELD " + objRefType->vmClassName() + "::" + memberName);
}