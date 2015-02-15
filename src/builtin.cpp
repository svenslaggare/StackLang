#include "builtin.h"
#include "binder.h"
#include "symbol.h"
#include "symboltable.h"
#include "typechecker.h"
#include "codegenerator.h"
#include <string>
#include <memory>

void StackLang::Builtin::add(Binder& binder, TypeChecker& typeChecker) {
	auto intType = typeChecker.getType("Int");
	auto floatType = typeChecker.getType("Float");
	
	typeChecker.addObject(Object("Array", nullptr, { { "length", Field("length", intType) } }));

	//Add conversions
	typeChecker.defineExplicitConversion(floatType, intType, [](CodeGenerator& codeGen, GeneratedFunction& func) {
		func.addInstruction("CONVFLOATTOINT");
	});

	typeChecker.defineExplicitConversion(intType, floatType, [](CodeGenerator& codeGen, GeneratedFunction& func) {
		func.addInstruction("CONVINTTOFLOAT");
	});
}