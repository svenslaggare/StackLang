#include "classast.h"
#include "helpers.h"
#include "functionast.h"
#include "expressionast.h"
#include "symboltable.h"
#include "binder.h"
#include "symbol.h"
#include "type.h"
#include "typechecker.h"
#include "object.h"
#include "codegenerator.h"
#include "helpers.h"

#include <unordered_map>

//Field declaration
FieldDeclarationExpressionAST::FieldDeclarationExpressionAST(std::string fieldType, std::string fieldName)
	: mFieldType(fieldType), mFieldName(fieldName) {

}

std::string FieldDeclarationExpressionAST::fieldType() const {
	return mFieldType;
}

std::string FieldDeclarationExpressionAST::fieldName() const {
	return mFieldName;
}

std::string FieldDeclarationExpressionAST::type() const {
	return "Field";
}

std::string FieldDeclarationExpressionAST::asString() const {
	return mFieldType + " " + mFieldName;
}

void FieldDeclarationExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
}

void FieldDeclarationExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	if (symbolTable->find(fieldName()) != nullptr) {
		binder.error("The symbol '" + fieldName() + "' is already defined.");	
	}

	symbolTable->add(fieldName(), std::make_shared<VariableSymbol>(fieldName(), fieldType()));
}

void FieldDeclarationExpressionAST::typeCheck(TypeChecker& checker) {
	checker.assertTypeExists(mFieldType);
}

std::shared_ptr<Type> FieldDeclarationExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(mFieldType);
}

void FieldDeclarationExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	
}

//Class definition
ClassDefinitionAST::ClassDefinitionAST(
	std::string name,
	std::vector<std::shared_ptr<FieldDeclarationExpressionAST>> fields,
	std::vector<std::shared_ptr<FunctionAST>> functions)
	: mName(name), mFields(fields), mFunctions(functions) {
	
}

std::string ClassDefinitionAST::name() const {
	return mName;
}

const std::vector<std::shared_ptr<FieldDeclarationExpressionAST>>& ClassDefinitionAST::fields() const {
	return mFields;
}

const std::vector<std::shared_ptr<FunctionAST>>& ClassDefinitionAST::functions() const {
	return mFunctions;
}

std::string ClassDefinitionAST::asString() const {
	auto fieldsStr = Helpers::join<std::shared_ptr<FieldDeclarationExpressionAST>>(
		mFields,
		[](std::shared_ptr<FieldDeclarationExpressionAST> field) {
			return field->asString() + ";";
		}, "\n");
	return "class " + mName + " {\n" + fieldsStr + "\n\n" + AST::combineAST(mFunctions, "\n\n") + "\n}";
}

#include <iostream>

void ClassDefinitionAST::addClassDefinition(TypeChecker& checker) const {
	if (checker.findType(name()) == nullptr) {
		auto classType = std::make_shared<ClassType>(name());
		checker.addType(classType);

		std::unordered_map<std::string, Field> fields;

		for (auto field : mFields) {
			checker.assertTypeExists(field->fieldType(), false);
			auto fieldType = checker.findType(field->fieldType());
			fields.insert({ field->fieldName(), Field(field->fieldName(), fieldType) });
		}

		checker.addObject(Object(name(), classType, fields));
	} else {
		checker.typeError("The class '" + name() + "' is already defined.");
	}
}

void ClassDefinitionAST::rewrite() {
	for (auto& field : mFields) {
		std::shared_ptr<AbstractSyntaxTree> newAST;

		if (field->rewriteAST(newAST)) {
			field = std::dynamic_pointer_cast<FieldDeclarationExpressionAST>(newAST);
		}

		field->rewrite();
	}

	for (auto& func : mFunctions) {
		std::shared_ptr<AbstractSyntaxTree> newAST;

		if (func->rewriteAST(newAST)) {
			func = std::dynamic_pointer_cast<FunctionAST>(newAST);
		}

		func->rewrite();
	}
}

void ClassDefinitionAST::visit(VisitFn visitFn) const {
	for (auto field : mFields) {
		field->visit(visitFn);
	}

	for (auto func : mFunctions) {
		func->visit(visitFn);
	}

	visitFn(this);
}

void ClassDefinitionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, SymbolTable::newInner(symbolTable));

	for (auto field : mFields) {
		field->generateSymbols(binder, mSymbolTable);
	}

	for (auto func : mFunctions) {
		func->generateSymbols(binder, mSymbolTable);
	}
}

void ClassDefinitionAST::typeCheck(TypeChecker& checker) {
	for (auto field : mFields) {
		field->typeCheck(checker);
	}

	for (auto func : mFunctions) {
		func->typeCheck(checker);
	}
}

void ClassDefinitionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {

}

//New class expression
NewClassExpressionAST::NewClassExpressionAST(std::string typeName, std::vector<std::shared_ptr<ExpressionAST>> constructorArguments)
	: mTypeName(typeName), mConstructorArguments(constructorArguments) {

}

std::string NewClassExpressionAST::typeName() const {
	return mTypeName;
}

const std::vector<std::shared_ptr<ExpressionAST>>& NewClassExpressionAST::constructorArguments() const {
	return mConstructorArguments;
}

std::string NewClassExpressionAST::asString() const {
	return "new " + mTypeName + "(" + AST::combineAST(mConstructorArguments, ", ") + ")";
}

void NewClassExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	for (auto arg : mConstructorArguments) {
		arg->generateSymbols(binder, symbolTable);
	}
}

void NewClassExpressionAST::typeCheck(TypeChecker& checker) {
	for (auto arg : mConstructorArguments) {
		arg->typeCheck(checker);
	}

	checker.assertTypeExists(mTypeName);

	if (!checker.objectExists(mTypeName)) {
		checker.typeError(mTypeName + " is not an object type.");
	}

	if (mConstructorArguments.size() > 0) {
		auto argsStr = Helpers::join<std::shared_ptr<ExpressionAST>>(
			mConstructorArguments,
			[&](std::shared_ptr<ExpressionAST> arg) { return arg->expressionType(checker)->name(); },
			", ");

		checker.typeError("There exists no constructor with the signature: " + mTypeName + "(" + argsStr  + ").");
	}
}

std::shared_ptr<Type> NewClassExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(mTypeName);
}

void NewClassExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	for (auto arg : mConstructorArguments) {
		arg->generateCode(codeGen, func);
	}

	func.addInstruction("NEWOBJ " + codeGen.typeChecker().findType(mTypeName)->vmType());
}