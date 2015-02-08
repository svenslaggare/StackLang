#include "standardlibrary.h"
#include "binder.h"
#include "symbol.h"
#include "symboltable.h"
#include "typechecker.h"
#include "codegenerator.h"
#include <string>
#include <memory>

void StandardLibrary::add(Binder& binder, TypeChecker& typeChecker) {
	auto intType = typeChecker.getType("Int");
	auto floatType = typeChecker.getType("Float");

	auto globalScope = binder.symbolTable();

	globalScope->newFunction("println", { { "Int", "x" } }, "Void");
	globalScope->newFunction("print", { { "Int", "x" } }, "Void");
	globalScope->newFunction("printchar", { { "Int", "x" } }, "Void");

	globalScope->newFunction("println", { { "Float", "x" } }, "Void");

	auto stdNamespaceTable = std::make_shared<SymbolTable>(binder.symbolTable());
	auto stdMathNamespaceTable = std::make_shared<SymbolTable>(stdNamespaceTable);
	auto stdMathNamespaceSymbol = std::make_shared<NamespaceSymbol>("math", stdMathNamespaceTable);

	//std
	// stdNamespaceTable->newFunction("println", { { "Int", "x" } }, "Void");
	// stdNamespaceTable->add("math", stdMathNamespaceSymbol);
	// binder.symbolTable()->add("std", std::make_shared<NamespaceSymbol>("std", stdNamespaceTable));

	// //std::math
	// stdMathNamespaceTable->newFunction("abs", { { "Int", "x" } }, "Int");

	typeChecker.addObject(Object("Array", nullptr, { { "length", Field("length", intType) } }));

	//Add conversions
	binder.symbolTable()->add("Float", std::make_shared<ConversionSymbol>("Float"));
	typeChecker.defineExplicitConversion(floatType, intType, [](CodeGenerator& codeGen, GeneratedFunction& func) { func.addInstruction("CONVFLOATTOINT"); });

	binder.symbolTable()->add("Int", std::make_shared<ConversionSymbol>("Int"));
	typeChecker.defineExplicitConversion(intType, floatType, [](CodeGenerator& codeGen, GeneratedFunction& func) { func.addInstruction("CONVINTTOFLOAT"); });
}