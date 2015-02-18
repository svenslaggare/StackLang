#include "expressionast.h"
#include "functionast.h"
#include "../symboltable.h"
#include "../binder.h"
#include "../typechecker.h"
#include "../type.h"
#include "../codegenerator.h"
#include "../symbol.h"
#include "../helpers.h"

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

void IntegerExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
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

void BoolExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
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

void FloatExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
}

std::shared_ptr<Type> FloatExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("Float");
}

void FloatExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	func.addInstruction("PUSHFLOAT " + std::to_string(mValue));
}

//Null ref expression AST
NullRefExpressionAST::NullRefExpressionAST() {

}

std::string NullRefExpressionAST::asString() const {
	return "null";
}

void NullRefExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
}

std::shared_ptr<Type> NullRefExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("NullRef");
}

void NullRefExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	func.addInstruction("PUSHNULL");
}

//Char expression AST
CharExpressionAST::CharExpressionAST(char value)
	: mValue(value) {

}

char CharExpressionAST::value() const {
	return mValue;
}

std::string CharExpressionAST::asString() const {
	return "'" + std::to_string(mValue) + "'";
}

void CharExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
}

std::shared_ptr<Type> CharExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("Char");
}

void CharExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	func.addInstruction("PUSHCHAR " + std::to_string((int)mValue));
}

//String expression AST
StringExpressionAST::StringExpressionAST(std::string value)
	: mValue(value) {

}

std::string toProgramString(std::string str) {
	std::string res = "";

	for (char c : str) {
		if (c == '"' || c == '\\') {
			res += "\\";
		}

		res += c;
	}

	return res;
}

std::string StringExpressionAST::value() const {
	return mValue;
}

std::string StringExpressionAST::asString() const {
	return "\"" + toProgramString(mValue) + "\"";
}

void StringExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
}

std::shared_ptr<Type> StringExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType("Char[]");
}

void StringExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	func.addInstruction("LDSTR \"" + toProgramString(mValue)  + "\"");
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

void VariableReferenceExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
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
	auto varRefSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(varName()));
	bool isFuncParam = varRefSymbol->attribute() == VariableSymbolAttribute::FUNCTION_PARAMETER;
	bool isField = varRefSymbol->attribute() == VariableSymbolAttribute::FIELD;
	bool isThis = varRefSymbol->attribute() == VariableSymbolAttribute::THIS_REFERENCE;

	if (isFuncParam) {
		func.addInstruction("LDARG " + std::to_string(func.functionParameterIndex(mVarName)));
	} else if (isField) {
		func.addInstruction("LDARG 0");
		func.addInstruction("LDFIELD " + varRefSymbol->className() + "::" + mVarName);
	} else if (isThis) {
		func.addInstruction("LDARG 0");
	} else {
		func.addInstruction("LDLOC " + std::to_string(func.getLocal(varRefSymbol).first));
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

void VariableDeclarationExpressionAST::visit(VisitFn visitFn) const {
	visitFn(this);
}

void VariableDeclarationExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	if (symbolTable->find(varName()) != nullptr) {
		binder.error("The symbol '" + varName() + "' is already defined.");	
	}

	VariableSymbolAttribute attribute = VariableSymbolAttribute::NONE;

	if (mIsFunctionParameter) {
		attribute = VariableSymbolAttribute::FUNCTION_PARAMETER;
	}

	symbolTable->add(varName(), std::make_shared<VariableSymbol>(varName(), varType(), attribute));
}

void VariableDeclarationExpressionAST::typeCheck(TypeChecker& checker) {
	checker.assertTypeExists(mVarType);
}

std::shared_ptr<Type> VariableDeclarationExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(mVarType);
}

void VariableDeclarationExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	if (!mIsFunctionParameter) {
		auto varRefSymbol = std::dynamic_pointer_cast<VariableSymbol>(mSymbolTable->find(mVarName));
		func.newLocal(varRefSymbol, codeGen.typeChecker().findType(mVarType));
	}
}

//Call expression AST
CallExpressionAST::CallExpressionAST(std::string functionName, std::vector<std::shared_ptr<ExpressionAST>> arguments)
	: mFunctionName(functionName), mArguments(arguments) {

}

std::shared_ptr<Symbol> CallExpressionAST::funcSymbol(std::shared_ptr<SymbolTable> symbolTable) const {
	return symbolTable->find(mFunctionName);
}

std::shared_ptr<FunctionSignatureSymbol> CallExpressionAST::funcSignature(const TypeChecker& typeChecker) const {
	std::vector<std::string> argumentsTypes;

	for (auto arg : mArguments) {
		argumentsTypes.push_back(arg->expressionType(typeChecker)->name());
	}

	return std::dynamic_pointer_cast<FunctionSymbol>(funcSymbol(callTable()))->findOverload(argumentsTypes);
}

std::string CallExpressionAST::functionName() const {
	return mFunctionName;
}

const std::vector<std::shared_ptr<ExpressionAST>>& CallExpressionAST::arguments() const {
	return mArguments;
}

std::shared_ptr<SymbolTable> CallExpressionAST::callTable() const {
	if (mCallTable == nullptr) {
		return mSymbolTable;
	} else {
		return mCallTable;
	}
}

void CallExpressionAST::setCallTable(std::shared_ptr<SymbolTable> callTable) {
	mCallTable = callTable;
}

std::string CallExpressionAST::asString() const {
	std::string callStr = "";
	callStr += mFunctionName + "(" + AST::combineAST(mArguments, ", ") + ")";
	return callStr;
}

void CallExpressionAST::visit(VisitFn visitFn) const {
	for (auto arg : mArguments) {
		arg->visit(visitFn);
	}

	visitFn(this);
}

void CallExpressionAST::rewrite() {
	for (auto& arg : mArguments) {
		std::shared_ptr<AbstractSyntaxTree> newAST;

		if (arg->rewriteAST(newAST)) {
			arg = std::dynamic_pointer_cast<ExpressionAST>(newAST);
		}

		arg->rewrite();
	}
}

void CallExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	
	auto symbol = funcSymbol(callTable());

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
	}
}

void CallExpressionAST::typeCheck(TypeChecker& checker) {
	for (int i = 0; i < arguments().size(); i++) {
		auto arg = arguments().at(i);
		arg->typeCheck(checker);
	}

	auto func = funcSignature(checker);

	if (func == nullptr) {
		auto paramsStr = Helpers::join<std::shared_ptr<ExpressionAST>>(
			arguments(),
			[&](std::shared_ptr<ExpressionAST> arg) { return arg->expressionType(checker)->name(); },
			", ");

		checker.typeError("There exists no function overload with the given signature: '" + mFunctionName + "(" + paramsStr + ")" + "'.");
	}

	for (int i = 0; i < arguments().size(); i++) {
		auto arg = arguments().at(i);

		auto param = func->parameters().at(i);

		auto argType = arg->expressionType(checker);
		auto paramType = checker.findType(param.variableType());

		checker.assertSameType(*paramType, *argType);
	}
}

void CallExpressionAST::verify(SemanticVerifier& verifier) {
	for (auto arg : arguments()) {
		arg->verify(verifier);
	}
}

std::shared_ptr<Type> CallExpressionAST::expressionType(const TypeChecker& checker) const {
	auto symbol = funcSymbol(callTable());
	auto func = funcSignature(checker);
	return checker.findType(func->returnType());
}

void CallExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func, std::string scopeName) {
	for (auto arg : arguments()) {
		arg->generateCode(codeGen, func);
	}

	auto argsTypeStr = Helpers::join<std::shared_ptr<ExpressionAST>>(
		arguments(),
		[&](std::shared_ptr<ExpressionAST> arg) {
			return arg->expressionType(codeGen.typeChecker())->vmType();
		}, " ");

	auto calldedFuncName = mFunctionName;

	if (scopeName != "") {
		calldedFuncName = scopeName + "." + mFunctionName;
	}

	func.addInstruction("CALL " + calldedFuncName + "(" + argsTypeStr + ")");
}

void CallExpressionAST::generateMemberCallCode(CodeGenerator& codeGen, GeneratedFunction& func, std::shared_ptr<Type> classType) {
	for (auto arg : arguments()) {
		arg->generateCode(codeGen, func);
	}

	auto argsTypeStr = Helpers::join<std::shared_ptr<ExpressionAST>>(
		arguments(),
		[&](std::shared_ptr<ExpressionAST> arg) {
			return arg->expressionType(codeGen.typeChecker())->vmType();
		}, " ");

	auto calldedFuncName = classType->name() + "::" + mFunctionName;
	func.addInstruction("CALLINST " + calldedFuncName + "(" + argsTypeStr + ")");
}

void CallExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	generateCode(codeGen, func, "");
}

//Cast expression
CastExpressionAST::CastExpressionAST(std::string typeName, std::shared_ptr<ExpressionAST> expression)
	: mTypeName(typeName), mExpression(expression) {

}

std::string CastExpressionAST::functionName() const {
	return mTypeName;
}

std::shared_ptr<ExpressionAST> CastExpressionAST::expression() const {
	return mExpression;
}

std::string CastExpressionAST::asString() const {
	return "cast<" + mTypeName + ">(" + mExpression->asString() + ")";
}

void CastExpressionAST::visit(VisitFn visitFn) const {
	mExpression->visit(visitFn);
	visitFn(this);
}

void CastExpressionAST::rewrite() {
	std::shared_ptr<AbstractSyntaxTree> newAST;

	if (mExpression->rewriteAST(newAST)) {
		mExpression = std::dynamic_pointer_cast<ExpressionAST>(newAST);
	}

	mExpression->rewrite();
}

void CastExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mExpression->generateSymbols(binder, symbolTable);
}

void CastExpressionAST::typeCheck(TypeChecker& checker) {
	mExpression->typeCheck(checker);
	checker.assertTypeExists(mTypeName);

	auto toType = checker.getType(mTypeName);
	auto fromType = mExpression->expressionType(checker);

	if (!checker.existsExplicitConversion(fromType, toType)) {
		checker.typeError("There exists no explicit conversion from type '" + fromType->name() + "' to type '" + toType->name() + "'.");
	}
}

void CastExpressionAST::verify(SemanticVerifier& verifier) {
	mExpression->verify(verifier);
}

std::shared_ptr<Type> CastExpressionAST::expressionType(const TypeChecker& checker) const {
	return checker.findType(mTypeName);
}

void CastExpressionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mExpression->generateCode(codeGen, func);
	auto& typeChecker = codeGen.typeChecker();
	auto toType = typeChecker.findType(mTypeName);
	auto fromType = mExpression->expressionType(typeChecker);
	typeChecker.getExplicitConversion(fromType, toType).applyConversion(codeGen, func);
}