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
	
	typeChecker.addObject(Object("Array", nullptr, { { "length", Field("length", intType) } }));

	//Add conversions
	binder.symbolTable()->add("Float", std::make_shared<ConversionSymbol>("Float"));
	typeChecker.defineExplicitConversion(floatType, intType, [](CodeGenerator& codeGen, GeneratedFunction& func) { func.addInstruction("CONVFLOATTOINT"); });

	binder.symbolTable()->add("Int", std::make_shared<ConversionSymbol>("Int"));
	typeChecker.defineExplicitConversion(intType, floatType, [](CodeGenerator& codeGen, GeneratedFunction& func) { func.addInstruction("CONVINTTOFLOAT"); });
}