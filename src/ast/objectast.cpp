#include "objectast.h"
#include "variableast.h"
#include "callast.h"
#include "arrayast.h"
#include "../object.h"
#include "../typechecker.h"
#include "../binder.h"
#include "../symbol.h"
#include "../symboltable.h"
#include "../type.h"
#include "../semantics.h"
#include "../codegenerator.h"
#include "../helpers.h"

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

	auto arrayMember = std::dynamic_pointer_cast<ArrayAccessAST>(mMemberExpression);

	if (!(std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)
		  || arrayMember)) {
		binder.error("'" + mMemberExpression->asString() + "' is not a member reference.");
	}

	if (arrayMember != nullptr) {
 		arrayMember->accessExpression()->generateSymbols(binder, symbolTable);
	}
}

std::string MemberAccessAST::getMemberName() const {
	if (auto varMember = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)) {
		return varMember->name();
	} else if (auto arrayMember = std::dynamic_pointer_cast<ArrayAccessAST>(mMemberExpression)) {
		if (auto arrayRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(arrayMember->arrayRefExpression())) {
			return arrayRef->name();
		}
	}

	return "";
}

void MemberAccessAST::typeCheck(TypeChecker& checker) {
	mAccessExpression->typeCheck(checker);

	if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mAccessExpression)) {
		auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->name()));
		auto varRefType = checker.findType(varSymbol->variableType());

		std::string memberName = getMemberName();
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

		if (auto arrayMember = std::dynamic_pointer_cast<ArrayAccessAST>(mMemberExpression)) {
			arrayMember->accessExpression()->typeCheck(checker);
		}
	}
}

const Object& MemberAccessAST::getObject(const TypeChecker& checker) const {
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

	return checker.getObject(objName);
}

const Field& MemberAccessAST::getField(const TypeChecker& checker) const {
	auto memberName = getMemberName();
	return getObject(checker).getField(memberName);
}

std::shared_ptr<Type> MemberAccessAST::expressionType(const TypeChecker& checker) const {
	if (auto arrayMember = std::dynamic_pointer_cast<ArrayAccessAST>(mMemberExpression)) {
		return std::dynamic_pointer_cast<ArrayType>(getField(checker).type())->elementType();
	} else {
		return getField(checker).type();
	}
}

void MemberAccessAST::verify(SemanticVerifier& verifier) {
	mAccessExpression->verify(verifier);
	mMemberExpression->verify(verifier);
	
	if (getField(verifier.typeChecker()).accessModifier() == AccessModifiers::Private) {
		auto className = mAccessExpression->expressionType(verifier.typeChecker())->name();
		auto classSymbol = std::dynamic_pointer_cast<ClassSymbol>(mSymbolTable->find(className));

		if (!classSymbol->symbolTable()->containsTable(mSymbolTable)) {
			verifier.semanticError("Cannot access private field of class " + className + ".");
		}
	}
}

void MemberAccessAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	auto memberName = getMemberName();
	auto typeRef = mAccessExpression->expressionType(codeGen.typeChecker());

	//Special handling of the 'length' field on arrays.
	if (memberName == "length" && std::dynamic_pointer_cast<ArrayType>(typeRef)) {
		mAccessExpression->generateCode(codeGen, func);
		func.addInstruction("LDLEN");
	} else {
		auto classTypeRef = std::dynamic_pointer_cast<ClassType>(typeRef);
		mAccessExpression->generateCode(codeGen, func);
		func.addInstruction("LDFIELD " + classTypeRef->vmClassName() + "::" + memberName);

		if (auto arrayMember = std::dynamic_pointer_cast<ArrayAccessAST>(mMemberExpression)) {
			arrayMember->generateCode(codeGen, func, expressionType(codeGen.typeChecker()));
		}
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

		std::string objName = varRefType->name();

		if (!checker.objectExists(objName)) {
			checker.typeError(varRefType->name() + " is not an object type.");
		}

		auto classSymbol = std::dynamic_pointer_cast<ClassSymbol>(Helpers::findSymbolInNamespace(mSymbolTable, objName));

		mMemberCallExpression->setCallTable(classSymbol->symbolTable());
		mMemberCallExpression->generateSymbols(checker.binder(), mSymbolTable);
		mMemberCallExpression->typeCheck(checker);
	} else {
		auto varRefType = mAccessExpression->expressionType(checker);

		std::string objName = varRefType->name();

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

void MemberCallExpressionAST::verify(SemanticVerifier& verifier) {
	mAccessExpression->verify(verifier);
	mMemberCallExpression->verify(verifier);

	auto accessModifier = mMemberCallExpression->funcSignature(verifier.typeChecker())->accessModifier();
	if (accessModifier == AccessModifiers::Private) {
		auto className = mAccessExpression->expressionType(verifier.typeChecker())->name();
		auto classSymbol = std::dynamic_pointer_cast<ClassSymbol>(mSymbolTable->find(className));

		if (!classSymbol->symbolTable()->containsTable(mSymbolTable)) {
			verifier.semanticError("Cannot call private function of class " + className + ".");
		}
	}
}

void MemberCallExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mAccessExpression->generateCode(codeGen, func);
	auto classType = std::dynamic_pointer_cast<ClassType>(mAccessExpression->expressionType(codeGen.typeChecker()));
	mMemberCallExpression->generateMemberCallCode(codeGen, func, classType);
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
	return mObjectRefExpression->asString() + "." + mMemberExpression->asString() + " = " + mRightHandSide->asString();
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

std::string SetFieldValueAST::getMemberName() const {
	if (auto varMember = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)) {
		return varMember->name();
	} else if (auto arrayMember = std::dynamic_pointer_cast<ArrayAccessAST>(mMemberExpression)) {
		if (auto arrayRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(arrayMember->arrayRefExpression())) {
			return arrayRef->name();
		}
	}

	return "";
}

void SetFieldValueAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mObjectRefExpression->generateSymbols(binder, symbolTable);

	auto arrayMember = std::dynamic_pointer_cast<ArrayAccessAST>(mMemberExpression);

	if (!(std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mMemberExpression)
		  || arrayMember)) {
		binder.error("'" + mMemberExpression->asString() + "' is not a member reference.");
	}

	mRightHandSide->generateSymbols(binder, symbolTable);

	if (arrayMember != nullptr) {
		arrayMember->accessExpression()->generateSymbols(binder, symbolTable);
	}
}

const Object& SetFieldValueAST::getObject(const TypeChecker& checker) const {
	std::shared_ptr<Type> objRefType;

	if (auto varRef = std::dynamic_pointer_cast<VariableReferenceExpressionAST>(mObjectRefExpression)) {
		auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varRef->name()));
		objRefType = checker.findType(varSymbol->variableType());
	} else if (auto arrayRef = std::dynamic_pointer_cast<ArrayAccessAST>(mObjectRefExpression)) {
		objRefType = arrayRef->expressionType(checker);
	}

	return checker.getObject(objRefType->name());
}

const Field& SetFieldValueAST::getField(const TypeChecker& checker) const {
	return getObject(checker).getField(getMemberName());
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

	auto memberName = getMemberName();
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
	std::shared_ptr<Type> fieldType;

	if (std::dynamic_pointer_cast<ArrayAccessAST>(mMemberExpression)) {
		fieldType = std::dynamic_pointer_cast<ArrayType>(object.getField(memberName).type())->elementType();
	} else {
		fieldType = object.getField(memberName).type();
	}

	checker.assertSameType(
		*fieldType,
		*mRightHandSide->expressionType(checker),
		asString());
}

std::shared_ptr<Type> SetFieldValueAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("Void");
}

void SetFieldValueAST::verify(SemanticVerifier& verifier) {
	mObjectRefExpression->verify(verifier);
	mMemberExpression->verify(verifier);
	mRightHandSide->verify(verifier);

	if (getField(verifier.typeChecker()).accessModifier() == AccessModifiers::Private) {
		auto className = getObject(verifier.typeChecker()).name();
		auto classSymbol = std::dynamic_pointer_cast<ClassSymbol>(mSymbolTable->find(className));

		if (!classSymbol->symbolTable()->containsTable(mSymbolTable)) {
			verifier.semanticError("Cannot access private field of class " + className + ".");
		}
	}
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

	auto memberName = getMemberName();

	if (auto arrayMember = std::dynamic_pointer_cast<ArrayAccessAST>(mMemberExpression)) {
		mObjectRefExpression->generateCode(codeGen, func);
		func.addInstruction("LDFIELD " + objRefType->vmClassName() + "::" + memberName);
		arrayMember->accessExpression()->generateCode(codeGen, func);

		mRightHandSide->generateCode(codeGen, func);
		func.addInstruction("STELEM " + mRightHandSide->expressionType(codeGen.typeChecker())->vmType());
	} else {
		mObjectRefExpression->generateCode(codeGen, func);
		mRightHandSide->generateCode(codeGen, func);
		func.addInstruction("STFIELD " + objRefType->vmClassName() + "::" + memberName);
	}
}