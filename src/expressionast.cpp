#include "expressionast.h"
#include "symboltable.h"
#include "binder.h"
#include "functionast.h"
#include "typechecker.h"
#include "type.h"
#include "asthelpers.h"
#include "codegenerator.h"
#include "symbol.h"

//Integer expression AST
IntegerExpressionAST::IntegerExpressionAST(int value)
	: mValue(value) {

}

int IntegerExpressionAST::value() const {
	return mValue;
}

std::string IntegerExpressionAST::asString() const {
	return std::to_string(mValue);
}

std::shared_ptr<Type> IntegerExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("Int");
}

void IntegerExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	func.addInstruction("PUSHINT " + std::to_string(mValue));
}

//Bool expression AST
BoolExpressionAST::BoolExpressionAST(bool value)
	: mValue(value) {

}

bool BoolExpressionAST::value() const {
	return mValue;
}

std::string BoolExpressionAST::asString() const {
	if (mValue) {
		return "true";
	} else {
		return "false";
	}
}

std::shared_ptr<Type> BoolExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("Bool");
}

void BoolExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	if (mValue) {
		func.addInstruction("PUSHTRUE");
	} else {
		func.addInstruction("PUSHFALSE");
	}
}

//Float expression AST
FloatExpressionAST::FloatExpressionAST(float value)
	: mValue(value) {

}

float FloatExpressionAST::value() const {
	return mValue;
}

std::string FloatExpressionAST::asString() const {
	return std::to_string(mValue);
}

std::shared_ptr<Type> FloatExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("Float");
}

void FloatExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	func.addInstruction("PUSHFLOAT " + std::to_string(mValue));
}

//Variable reference expression AST
VariableReferenceExpressionAST::VariableReferenceExpressionAST(std::string varName)
	: mVarName(varName) {

}

std::string VariableReferenceExpressionAST::varName() const {
	return mVarName;
}

std::string VariableReferenceExpressionAST::asString() const {
	return mVarName;
}

void VariableReferenceExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	auto symbol = symbolTable->find(varName());

	if (symbol == nullptr) {
		binder.error("The variable '" + varName() + "' is not defined.");
	} else {
		if (std::dynamic_pointer_cast<VariableSymbol>(symbol) == nullptr) {
			binder.error("'" + varName() + "' is not a variable.");
		}
	}
}

std::shared_ptr<Type> VariableReferenceExpressionAST::expressionType(const TypeChecker& checker) const {
	auto varSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varName()));

	if (varSymbol != nullptr) {
		return checker.findType(varSymbol->variableType());
	} else {
		return checker.findType("Void");
	}
}

void VariableReferenceExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	bool isFuncParam = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varName()))->isFunctionParameter();

	if (isFuncParam) {
		func.addInstruction("LDARG " + std::to_string(func.functionParameterIndex(mVarName)));
	} else {
		func.addInstruction("LDLOC " + std::to_string(func.getLocal(mVarName).first));
	}
}

//Variable declaration expression AST
VariableDeclarationExpressionAST::VariableDeclarationExpressionAST(std::string varType, std::string varName, bool isFunctionParameter)
	: mVarType(varType), mVarName(varName), mIsFunctionParameter(isFunctionParameter) {

}

std::string VariableDeclarationExpressionAST::varType() const {
	return mVarType;
}

std::string VariableDeclarationExpressionAST::varName() const {
	return mVarName;
}

bool VariableDeclarationExpressionAST::isFunctionParameter() const {
	return mIsFunctionParameter;
}

std::string VariableDeclarationExpressionAST::type() const {
	return "Variable";
}

std::string VariableDeclarationExpressionAST::asString() const {
	return mVarType + " " + mVarName;
}

void VariableDeclarationExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	if (!symbolTable->add(varName(), std::make_shared<VariableSymbol>(varName(), varType(), mIsFunctionParameter))) {
		binder.error("The symbol '" + varName() + "' is already defined.");
	}
}

void VariableDeclarationExpressionAST::typeCheck(TypeChecker& checker) {
	checker.assertTypeExists(mVarType);
}

std::shared_ptr<Type> VariableDeclarationExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(mVarType);
}

void VariableDeclarationExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	if (!mIsFunctionParameter) {
		func.newLocal(mVarName, codeGen.typeChecker().findType(mVarType));
	}
}

//Call expression AST
CallExpressionAST::CallExpressionAST(std::string functionName, std::vector<std::shared_ptr<ExpressionAST>> arguments)
	: mFunctionName(functionName), mArguments(arguments) {

}

std::string CallExpressionAST::functionName() const {
	return mFunctionName;
}

const std::vector<std::shared_ptr<ExpressionAST>>& CallExpressionAST::arguments() const {
	return mArguments;
}

std::string CallExpressionAST::asString() const {
	std::string callStr = "";

	callStr += mFunctionName + "(" + AST::combineAST(mArguments, ", ") + ")";

	return callStr;
}

void CallExpressionAST::rewrite() {
	for (auto& arg : mArguments) {
		std::shared_ptr<AbstractSyntaxTree> newAST;

		if (arg->rewriteAST(newAST)) {
			arg = std::dynamic_pointer_cast<ExpressionAST>(newAST);
		}
	}
}

void CallExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	
	auto symbol = symbolTable->find(functionName());

	if (symbol == nullptr) {
		binder.error("The function '" + functionName() + "' is not defined.");
	} else {
		auto func = std::dynamic_pointer_cast<FunctionSymbol>(symbol);
		auto conversion = std::dynamic_pointer_cast<ConversionSymbol>(symbol);

		for (auto arg : mArguments) {
			arg->generateSymbols(binder, symbolTable);
		}

		if (conversion != nullptr) {
			if (arguments().size() != 1) {
				binder.error("Expected 1 arguments but got: " + std::to_string(arguments().size()));
			}
		} else {
			if (func == nullptr) {
				binder.error("'" + functionName() + "' is not a function.");
			}

			if (func->parameters().size() != arguments().size()) {
				binder.error("Expected " + std::to_string(func->parameters().size()) + " arguments but got: " + std::to_string(arguments().size()));
			}
		}
	}
}

void CallExpressionAST::typeCheck(TypeChecker& checker) {
	//Check if conversion
	auto toType = checker.findType(mFunctionName);

	if (arguments().size() == 1 && toType != nullptr) {	
		auto arg = arguments().at(0);
		arg->typeCheck(checker);

		auto fromType = arg->expressionType(checker);

		if (!checker.existsExplicitConversion(fromType, toType)) {
			checker.typeError("There exists no explicit conversion from type '" + fromType->name() + "' to type '" + toType->name() + "'.");
		}
	} else {
		auto func = std::dynamic_pointer_cast<FunctionSymbol>(mSymbolTable->find(functionName()));

		for (int i = 0; i < arguments().size(); i++) {
			auto arg = arguments().at(i);
			arg->typeCheck(checker);

			auto param = func->parameters().at(i);

			auto argType = arg->expressionType(checker);
			auto paramType = checker.findType(param->variableType());

			checker.assertSameType(*paramType, *argType);
		}
	}
}

void CallExpressionAST::verify(SemanticVerifier& verifier) {
	for (auto arg : arguments()) {
		arg->verify(verifier);
	}
}

std::shared_ptr<Type> CallExpressionAST::expressionType(const TypeChecker& checker) const {
	auto symbol = mSymbolTable->find(functionName());

	if (auto conversion = std::dynamic_pointer_cast<ConversionSymbol>(symbol)) {
		return checker.findType(conversion->name());
	} else {
		auto func = std::dynamic_pointer_cast<FunctionSymbol>(symbol);
		return checker.findType(func->returnType());
	}
}

void CallExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	for (auto arg : arguments()) {
		arg->generateCode(codeGen, func);
	}

	//Check if conversion
	auto& typeChecker = codeGen.typeChecker();
	auto toType = typeChecker.findType(mFunctionName);

	if (arguments().size() == 1 && toType != nullptr) {
		auto fromType = arguments().at(0)->expressionType(typeChecker);
		typeChecker.getExplicitConversion(fromType, toType).applyConversion(codeGen, func);
	} else {
		func.addInstruction("CALL " + mFunctionName);
	}
}