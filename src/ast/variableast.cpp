#include "variableast.h"
#include "../symboltable.h"
#include "../binder.h"
#include "../typechecker.h"
#include "../type.h"
#include "../codegenerator.h"
#include "../symbol.h"
#include "../helpers.h"
#include "../typename.h"
#include "../compiler.h"
#include "objectast.h"

//Variable reference expression AST
VariableReferenceExpressionAST::VariableReferenceExpressionAST(std::string name)
	: mName(name) {

}

std::string VariableReferenceExpressionAST::name() const {
	return mName;
}

std::string VariableReferenceExpressionAST::asString() const {
	return mName;
}

std::shared_ptr<VariableSymbol> VariableReferenceExpressionAST::symbol() const {
	if (mSymbolTable == nullptr) {
		return nullptr;
	}

	return std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(name()));
}

void VariableReferenceExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
}

bool VariableReferenceExpressionAST::rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST, Compiler& compiler) const {
	if (mSymbolTable != nullptr) {
		auto varRefSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(name()));

		if (varRefSymbol->attribute() == VariableSymbolAttribute::FIELD) {
			newAST = std::make_shared<MemberAccessAST>(
				std::make_shared<VariableReferenceExpressionAST>("this"),
				std::make_shared<VariableReferenceExpressionAST>(mName));
			newAST->generateSymbols(compiler.binder(), mSymbolTable);

			return true;
		}
	}

	return false;
}

void VariableReferenceExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	auto symbol = symbolTable->find(name());

	if (symbol == nullptr) {
		binder.error("The variable '" + name() + "' is not defined.");
	} else {
		if (std::dynamic_pointer_cast<VariableSymbol>(symbol) == nullptr) {
			binder.error("'" + name() + "' is not a variable.");
		}
	}
}

std::shared_ptr<Type> VariableReferenceExpressionAST::expressionType(const TypeChecker& checker) const {
	auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(name()));

	if (varSymbol != nullptr) {
		return checker.findType(varSymbol->variableType());
	} else {
		return checker.findType("Void");
	}
}

void VariableReferenceExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	auto varRefSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(name()));
	bool isFuncParam = varRefSymbol->attribute() == VariableSymbolAttribute::FUNCTION_PARAMETER;
	bool isThis = varRefSymbol->attribute() == VariableSymbolAttribute::THIS_REFERENCE;

	if (isFuncParam) {
		func.addInstruction("LDARG " + std::to_string(func.functionParameterIndex(mName)));
	} else if (isThis) {
		func.addInstruction("LDARG 0");
	} else {
		func.addInstruction("LDLOC " + std::to_string(func.getLocal(varRefSymbol).first));
	}
}

//Variable declaration expression AST
VariableDeclarationExpressionAST::VariableDeclarationExpressionAST(std::string type, std::string name, bool isFunctionParameter)
	: mType(TypeName::make(type)), mName(name), mIsFunctionParameter(isFunctionParameter) {

}

std::string VariableDeclarationExpressionAST::type() const {
	return mType->name();
}

std::string VariableDeclarationExpressionAST::name() const {
	return mName;
}

bool VariableDeclarationExpressionAST::isFunctionParameter() const {
	return mIsFunctionParameter;
}

std::string VariableDeclarationExpressionAST::asString() const {
	return type() + " " + name();
}

void VariableDeclarationExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
}

void VariableDeclarationExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	if (symbolTable->find(name()) != nullptr) {
		binder.error("The symbol '" + name() + "' is already defined.");	
	}

	VariableSymbolAttribute attribute = VariableSymbolAttribute::NONE;

	if (mIsFunctionParameter) {
		attribute = VariableSymbolAttribute::FUNCTION_PARAMETER;
	}

	//Find the full type name
	mType = std::move(TypeName::makeFull(mType.get(), symbolTable));
	symbolTable->add(name(), std::make_shared<VariableSymbol>(name(), type(), attribute));
}

void VariableDeclarationExpressionAST::typeCheck(TypeChecker& checker) {
	checker.assertTypeExists(type());
}

std::shared_ptr<Type> VariableDeclarationExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(type());
}

void VariableDeclarationExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	if (!mIsFunctionParameter) {
		auto varRefSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(mName));
		func.newLocal(varRefSymbol, codeGen.typeChecker().findType(type()));
	}
}