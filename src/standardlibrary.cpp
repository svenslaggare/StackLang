#include "standardlibrary.h"
#include "binder.h"
#include "symbol.h"
#include "typechecker.h"
#include "codegenerator.h"
#include <string>
#include <memory>

void StandardLibrary::add(Binder& binder, TypeChecker& typeChecker) {
	binder.addFunction("println", { { "Int", "x" } }, "Void");
	binder.addFunction("print", { { "Int", "x" } }, "Void");
	binder.addFunction("printchar", { { "Int", "x" } }, "Void");

	binder.addFunction("printfln", { { "Float", "x" } }, "Void");

	//Add conversions
	auto intType = typeChecker.getType("Int");
	auto floatType = typeChecker.getType("Float");

	binder.symbolTable()->add("Float", std::make_shared<ConversionSymbol>("Float"));
	typeChecker.defineExplicitConversion(floatType, intType, [](CodeGenerator& codeGen, GeneratedFunction& func) { func.addInstruction("CONVFLOATTOINT"); });

	binder.symbolTable()->add("Int", std::make_shared<ConversionSymbol>("Int"));
	typeChecker.defineExplicitConversion(intType, floatType, [](CodeGenerator& codeGen, GeneratedFunction& func) { func.addInstruction("CONVINTTOFLOAT"); });
}