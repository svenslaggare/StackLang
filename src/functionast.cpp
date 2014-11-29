#include "functionast.h"
#include "expressionast.h"
#include "blockast.h"
#include "symboltable.h"
#include "binder.h"

FunctionAST::FunctionAST(std::string name, const std::vector<std::shared_ptr<VariableDeclerationExpressionAST>>& arguments, std::string returnType, std::shared_ptr<BlockAST> body)
	: mName(name), mArguments(arguments), mReturnType(returnType), mBody(body) {

}

const std::string FunctionAST::name() const {
	return mName;
}

const std::vector<std::shared_ptr<VariableDeclerationExpressionAST>>& FunctionAST::arguments() const {
	return mArguments;
}

const std::string FunctionAST::returnType() const {
	return mReturnType;
}

std::shared_ptr<BlockAST> FunctionAST::body() const {
	return mBody;
}

std::string FunctionAST::type() const {
	return "Function";
}

std::string FunctionAST::asString() const {
	std::string funcStr = "func " + mName + "(";

	bool isFirst = true;
	for (auto arg : mArguments) {
		if (!isFirst) {
			funcStr += ", ";
		} else {
			isFirst = false;
		}

		funcStr += arg->asString();
	}
	funcStr += "): " + mReturnType + " ";

	funcStr += mBody->asString();

	return funcStr;
}

void FunctionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	if (!symbolTable->add(name(), std::make_shared<FunctionAST>(*this))) {
		binder.error("The symbol '" + name() + "' is already defined.");
	}

	auto inner = std::make_shared<SymbolTable>(SymbolTable(symbolTable));

	for (auto arg : mArguments) {
		arg->generateSymbols(binder, inner);
	}

	mBody->generateSymbols(binder, inner);
}