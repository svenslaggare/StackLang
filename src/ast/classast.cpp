#include "classast.h"
#include "functionast.h"
#include "variableast.h"
#include "blockast.h"
#include "statementast.h"
#include "../helpers.h"
#include "../symboltable.h"
#include "../binder.h"
#include "../symbol.h"
#include "../type.h"
#include "../typechecker.h"
#include "../typename.h"
#include "../object.h"
#include "../codegenerator.h"
#include "../helpers.h"
#include <unordered_map>

//Field declaration
FieldDeclarationExpressionAST::FieldDeclarationExpressionAST(std::string fieldType, std::string fieldName)
	: mFieldType(TypeName::make(fieldType)), mFieldName(fieldName) {

}

std::string FieldDeclarationExpressionAST::fieldType() const {
	return mFieldType->name();
}

std::string FieldDeclarationExpressionAST::fieldName() const {
	return mFieldName;
}

std::string FieldDeclarationExpressionAST::asString() const {
	return fieldType() + " " + fieldName();
}

void FieldDeclarationExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
}

void FieldDeclarationExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	if (symbolTable->find(fieldName()) != nullptr) {
		binder.error("The symbol '" + fieldName() + "' is already defined.");	
	}

	if (mFieldType->name() == "Point") {
		mFieldType = std::move(TypeName::makeFull(mFieldType.get(), symbolTable));
		// std::cout << mFieldType->name() << std::endl;
		// std::cout << symbolTable->find("Point") << std::endl;
		// for (auto symbol : symbolTable->outer()->inner()) {
		// 	std::cout << symbol.first << std::endl;
		// }
	}

	symbolTable->add(
		fieldName(),
		std::make_shared<VariableSymbol>(fieldName(), fieldType(), VariableSymbolAttribute::FIELD, mSymbolTable->name()));
}

void FieldDeclarationExpressionAST::typeCheck(TypeChecker& checker) {
	checker.assertTypeExists(fieldType());
}

std::shared_ptr<Type> FieldDeclarationExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(fieldType());
}

void FieldDeclarationExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	
}

//Class definition
ClassDefinitionAST::ClassDefinitionAST(
	std::string name,
	std::vector<std::shared_ptr<FieldDeclarationExpressionAST>> fields,
	std::vector<std::shared_ptr<FunctionAST>> functions)
	: mName(name), mFields(fields), mFunctions(functions) {
	
	bool hasConstructor = false;

	for (auto func : mFunctions) {
		if (func->prototype()->name() == ".constructor") {
			hasConstructor = true;
			break;
		}
	}

	//Add default constructor
	if (!hasConstructor) {
		mFunctions.push_back(std::make_shared<FunctionAST>(
			std::make_shared<FunctionPrototypeAST>(
				".constructor",
				std::vector<std::shared_ptr<VariableDeclarationExpressionAST>>({}),
				"Void"),
			std::make_shared<BlockAST>(std::vector<std::shared_ptr<StatementAST>>({}))));
	}
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

std::string ClassDefinitionAST::findNamespaceName(std::shared_ptr<SymbolTable> symbolTable, std::string sep) const {
	if (symbolTable == nullptr) {
		return "";
	} else {
		auto name = symbolTable->name();

		if (name != "" && symbolTable != mSymbolTable) {
			name += sep;
		}

		return findNamespaceName(symbolTable->outer(), sep) + name;
	}
}

std::string ClassDefinitionAST::fullName(std::string namespaceSep) const {
	return findNamespaceName(mSymbolTable, namespaceSep);
}

std::string ClassDefinitionAST::asString() const {
	auto fieldsStr = Helpers::join<std::shared_ptr<FieldDeclarationExpressionAST>>(
		mFields,
		[](std::shared_ptr<FieldDeclarationExpressionAST> field) {
			return field->asString() + ";";
		}, "\n");

	auto funcsStr = AST::combineAST(mFunctions, "\n\n");

	auto classStr = "class " + mName + " {\n";

	classStr += fieldsStr;

	if (funcsStr != "") {
		classStr += "\n\n" + funcsStr;
	}

	classStr += "\n}";
	return classStr;
}

void ClassDefinitionAST::addClassDefinition(TypeChecker& checker) const {
	if (checker.findType(name()) == nullptr) {
		auto classType = std::make_shared<ClassType>(fullName());
		checker.addType(classType);

		std::unordered_map<std::string, Field> fields;

		for (auto field : mFields) {
			checker.assertTypeExists(field->fieldType(), false);
			auto fieldType = checker.findType(field->fieldType());
			fields.insert({ field->fieldName(), Field(field->fieldName(), fieldType) });
		}

		checker.addObject(Object(fullName(), classType, fields));
	} else {
		checker.typeError("The class '" + name() + "' is already defined.");
	}
}

void ClassDefinitionAST::setDefiningTable(std::shared_ptr<SymbolTable> symbolTable) {
	mDefiningTable = symbolTable;
}

void ClassDefinitionAST::rewrite(Compiler& compiler) {
	for (auto& field : mFields) {
		std::shared_ptr<AbstractSyntaxTree> newAST;

		while (field->rewriteAST(newAST, compiler)) {
			field = std::dynamic_pointer_cast<FieldDeclarationExpressionAST>(newAST);
		}

		field->rewrite(compiler);
	}

	for (auto& func : mFunctions) {
		std::shared_ptr<AbstractSyntaxTree> newAST;

		while (func->rewriteAST(newAST, compiler)) {
			func = std::dynamic_pointer_cast<FunctionAST>(newAST);
		}

		func->rewrite(compiler);
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
	auto classTable = std::make_shared<SymbolTable>(symbolTable, mName);
	AbstractSyntaxTree::generateSymbols(binder, classTable);

	classTable->add(
		"this",
		std::make_shared<VariableSymbol>("this", mName, VariableSymbolAttribute::THIS_REFERENCE));

	for (auto field : mFields) {
		field->generateSymbols(binder, classTable);
	}

	for (auto func : mFunctions) {
		func->bindSignature(binder, classTable);

		//Declare member functions
		auto funcName = func->prototype()->name();
		std::vector<VariableSymbol> parameters;

		for (auto param : func->prototype()->parameters()) {
			parameters.push_back(VariableSymbol(
				param->name(),
				param->type(),
				VariableSymbolAttribute::FUNCTION_PARAMETER));
		}

		auto symbol = classTable->find(funcName);

		if (symbol != nullptr && std::dynamic_pointer_cast<FunctionSymbol>(symbol) == nullptr) {
			binder.error("The symbol '" + funcName + "' is already defined.");
		}

		if (!classTable->addFunction(funcName, parameters, func->prototype()->returnType())) {
			auto paramsStr = Helpers::join<VariableSymbol>(
				parameters,
				[](VariableSymbol param) { return param.variableType(); },
				", ");

			binder.error("The already exists a member function with the given signature: '" + funcName + "(" + paramsStr + ")" + "'.");
		}
	}

	for (auto func : mFunctions) {
		func->generateSymbols(binder, classTable);
	}

	if (mDefiningTable == nullptr) {
		mDefiningTable = symbolTable;
	}

	if (mDefiningTable->find(mName) == nullptr) {
		mDefiningTable->addClass(mName, classTable);
	} else {
		binder.error("The symbol '" + mName + "' is already defined.");
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

std::shared_ptr<Symbol> NewClassExpressionAST::constructorSymbol(std::shared_ptr<SymbolTable> symbolTable) const {
	return symbolTable->find(".constructor");
}

std::shared_ptr<FunctionSignatureSymbol> NewClassExpressionAST::constructorSignature(const TypeChecker& typeChecker) const {
	std::vector<std::string> argumentsTypes;

	for (auto arg : mConstructorArguments) {
		argumentsTypes.push_back(arg->expressionType(typeChecker)->name());
	}

	auto funcSymbol = std::dynamic_pointer_cast<FunctionSymbol>(constructorSymbol(mClassSymbol->symbolTable()));
	return funcSymbol->findOverload(argumentsTypes);
}

void NewClassExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	auto classSymbol = std::dynamic_pointer_cast<ClassSymbol>(Helpers::findSymbolInNamespace(mSymbolTable, mTypeName));

	if (classSymbol == nullptr) {
		binder.error("There exists no class named '" + mTypeName + "'.");
	}

	mClassSymbol = classSymbol;
	mTypeName = mClassSymbol->fullName();

	auto symbol = constructorSymbol(classSymbol->symbolTable());

	if (symbol == nullptr) {
		binder.error("No constructor is defined for the class '" + mTypeName + "'.");
	} else {
		auto func = std::dynamic_pointer_cast<FunctionSymbol>(symbol);

		for (auto arg : mConstructorArguments) {
			arg->generateSymbols(binder, symbolTable);
		}

		if (func == nullptr) {
			binder.error("The binded symbol is not a constructor.");
		}
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

	auto constructor = constructorSignature(checker);

	if (constructor == nullptr) {
		auto paramsStr = Helpers::join<std::shared_ptr<ExpressionAST>>(
			constructorArguments(),
			[&](std::shared_ptr<ExpressionAST> arg) { return arg->expressionType(checker)->name(); },
			", ");

		checker.typeError("There exists no constructor overload with the given signature: '" + mTypeName + "(" + paramsStr + ")" + "'.");
	}

	for (int i = 0; i < constructorArguments().size(); i++) {
		auto arg = constructorArguments().at(i);

		auto param = constructor->parameters().at(i);

		auto argType = arg->expressionType(checker);
		auto paramType = checker.findType(param.variableType());

		checker.assertSameType(*paramType, *argType);
	}
}

std::shared_ptr<Type> NewClassExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(mTypeName);
}

void NewClassExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	auto classType = std::dynamic_pointer_cast<ClassType>(codeGen.typeChecker().findType(mTypeName));

	//Generate code for the constructor arguments
	for (auto arg : mConstructorArguments) {
		arg->generateCode(codeGen, func);
	}

	auto paramsStr = Helpers::join<std::shared_ptr<ExpressionAST>>(
		constructorArguments(),
		[&](std::shared_ptr<ExpressionAST> arg) { return arg->expressionType(codeGen.typeChecker())->vmType(); },
		" ");

	func.addInstruction("NEWOBJ " + classType->vmClassName() + "::.constructor(" + paramsStr + ")");
}