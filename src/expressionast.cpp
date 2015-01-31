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
	return checker.getType("Int");
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
	return checker.getType("Bool");
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
	return checker.getType("Float");
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
		return checker.getType(varSymbol->variableType());
	} else {
		return checker.getType("Void");
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

//Variable decleration expression AST
VariableDeclerationExpressionAST::VariableDeclerationExpressionAST(std::string varType, std::string varName, bool isFunctionParameter)
	: mVarType(varType), mVarName(varName), mIsFunctionParameter(isFunctionParameter) {

}

std::string VariableDeclerationExpressionAST::varType() const {
	return mVarType;
}

std::string VariableDeclerationExpressionAST::varName() const {
	return mVarName;
}

bool VariableDeclerationExpressionAST::isFunctionParameter() const {
	return mIsFunctionParameter;
}

std::string VariableDeclerationExpressionAST::type() const {
	return "Variable";
}

std::string VariableDeclerationExpressionAST::asString() const {
	return mVarType + " " + mVarName;
}

void VariableDeclerationExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	if (!symbolTable->add(varName(), std::make_shared<VariableSymbol>(varName(), varType(), mIsFunctionParameter))) {
		binder.error("The symbol '" + varName() + "' is already defined.");
	}
}

void VariableDeclerationExpressionAST::typeCheck(TypeChecker& checker) {
	checker.assertTypeExists(mVarType);
}

std::shared_ptr<Type> VariableDeclerationExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.getType(mVarType);
}

void VariableDeclerationExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	if (!mIsFunctionParameter) {
		func.newLocal(mVarName, codeGen.typeChecker().getType(mVarType));
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

		for (auto arg : mArguments) {
			arg->generateSymbols(binder, symbolTable);
		}

		if (func == nullptr) {
			binder.error("'" + functionName() + "' is not a function.");
		}

		if (func->parameters().size() != arguments().size()) {
			binder.error("Expected " + std::to_string(func->parameters().size()) + " arguments but got: " + std::to_string(arguments().size()));
		}
	}
}

void CallExpressionAST::typeCheck(TypeChecker& checker) {
	auto func = std::dynamic_pointer_cast<FunctionSymbol>(mSymbolTable->find(functionName()));

	for (int i = 0; i < arguments().size(); i++) {
		auto arg = arguments().at(i);
		arg->typeCheck(checker);

		auto param = func->parameters().at(i);

		auto argType = arg->expressionType(checker);
		auto paramType = checker.getType(param->variableType());

		checker.assertSameType(*paramType, *argType);
	}
}

void CallExpressionAST::verify(SemanticVerifier& verifier) {
	for (auto arg : arguments()) {
		arg->verify(verifier);
	}
}

std::shared_ptr<Type> CallExpressionAST::expressionType(const TypeChecker& checker) const {
	auto func = std::dynamic_pointer_cast<FunctionSymbol>(mSymbolTable->find(functionName()));
	return checker.getType(func->returnType());
}

void CallExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	for (auto arg : arguments()) {
		arg->generateCode(codeGen, func);
	}

	//Check if conversion
	if (arguments().size() == 1 && codeGen.typeChecker().getType(mFunctionName) != nullptr) {
		auto fromType = arguments().at(0)->expressionType(codeGen.typeChecker());
		func.addInstruction("CONV" + fromType->name() + "TO" + mFunctionName);
	} else {
		func.addInstruction("CALL " + mFunctionName);
	}
}