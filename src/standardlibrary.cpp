#include "standardlibrary.h"
#include "binder.h"
#include "typechecker.h"
#include <string>

void StandardLibrary::add(Binder& binder, TypeChecker& typeChecker) {
	binder.addFunction("println", { { "Int", "x" } }, "Void");
	binder.addFunction("print", { { "Int", "x" } }, "Void");
	binder.addFunction("printchar", { { "Int", "x" } }, "Void");

	binder.addFunction("printfln", { { "Float", "x" } }, "Void");

	//Add conversions
	auto intType = typeChecker.getType("Int");
	auto floatType = typeChecker.getType("Float");

	binder.addFunction("Int", { { "Float", "x" } }, "Int");
	typeChecker.defineExplicitConversion(floatType, intType);

	binder.addFunction("Float", { { "Int", "x" } }, "Float");
	typeChecker.defineExplicitConversion(intType, floatType);
}