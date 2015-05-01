#include "callast.h"
#include "functionast.h"
#include "../symboltable.h"
#include "../binder.h"
#include "../typechecker.h"
#include "../type.h"
#include "../codegenerator.h"
#include "../symbol.h"
#include "../helpers.h"

CallExpressionAST::CallExpressionAST(std::string functionName, std::vector<std::shared_ptr<ExpressionAST>> arguments)
	: mFunctionName(functionName), mArguments(arguments) {

}

std::shared_ptr<FunctionSignatureSymbol> CallExpressionAST::funcSignature(const TypeChecker& typeChecker) const {
	std::vector<std::string> argumentsTypes;

	for (auto arg : mArguments) {
		argumentsTypes.push_back(arg->expressionType(typeChecker)->name());
	}

	return std::dynamic_pointer_cast<FunctionSignatureSymbol>(mFuncSymbol->findOverload(argumentsTypes));
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

void CallExpressionAST::rewrite(Compiler& compiler) {
	for (auto& arg : mArguments) {
		std::shared_ptr<AbstractSyntaxTree> newAST;

		while (arg->rewriteAST(newAST, compiler)) {
			arg = std::dynamic_pointer_cast<ExpressionAST>(newAST);
		}

		arg->rewrite(compiler);
	}
}

void CallExpressionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	auto symbol = Helpers::findSymbolInNamespace(callTable(), mFunctionName);

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

		mFuncSymbol = func;
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

	auto nameParts = Helpers::splitString(mFunctionName, "::");
	auto funcName = nameParts[nameParts.size() - 1];
	// auto namespaceName = Helpers::replaceString(mFuncSymbol->namespaceName(), "::", ".");
	auto namespaceName = mFuncSymbol->definedNamespace().vmName();
	std::string calldedFuncName = funcName;

	if (namespaceName != "") {
		calldedFuncName = namespaceName + "." + funcName;
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