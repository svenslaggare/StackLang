#include "functionast.h"
#include "expressionast.h"
#include "statementast.h"
#include "blockast.h"
#include "symboltable.h"
#include "binder.h"
#include "typechecker.h"
#include "type.h"
#include "semantics.h"

//Function prototype AST
FunctionPrototypeAST::FunctionPrototypeAST(std::string name, const std::vector<std::shared_ptr<VariableDeclerationExpressionAST>>& parameters, std::string returnType)
	: mName(name), mParameters(parameters), mReturnType(returnType) {

}

const std::string FunctionPrototypeAST::name() const {
	return mName;
}

const std::vector<std::shared_ptr<VariableDeclerationExpressionAST>>& FunctionPrototypeAST::parameters() const {
	return mParameters;
}

const std::string FunctionPrototypeAST::returnType() const {
	return mReturnType;
}

std::string FunctionPrototypeAST::type() const {
	return "FunctionPrototype";
}

std::string FunctionPrototypeAST::asString() const {
	std::string funcStr = "func " + mName + "(";

	bool isFirst = true;
	for (auto param : mParameters) {
		if (!isFirst) {
			funcStr += ", ";
		} else {
			isFirst = false;
		}

		funcStr += param->asString();
	}
	funcStr += "): " + mReturnType + " ";

	return funcStr;
}

void FunctionPrototypeAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	
	for (auto param : mParameters) {
		param->generateSymbols(binder, symbolTable);
	}
}

void FunctionPrototypeAST::typeCheck(TypeChecker& checker) {
	for (auto param : mParameters) {
		param->typeCheck(checker);
	}

	checker.assertTypeExists(mReturnType);
}

void FunctionPrototypeAST::verify(SemanticVerifier& verifier) {
	for (auto param : mParameters) {
		param->verify(verifier);
	}
}

//Function AST
FunctionAST::FunctionAST(std::shared_ptr<FunctionPrototypeAST> prototype, std::shared_ptr<BlockAST> body)
	: mPrototype(prototype), mBody(body) {

}

const std::shared_ptr<FunctionPrototypeAST> FunctionAST::prototype() const {
	return mPrototype;
}

std::shared_ptr<BlockAST> FunctionAST::body() const {
	return mBody;
}

std::string FunctionAST::type() const {
	return "Function";
}

std::string FunctionAST::asString() const {
	auto funcStr = mPrototype->asString();
	funcStr += mBody->asString();
	return funcStr;
}

void FunctionAST::rewrite() {
	std::shared_ptr<AbstractSyntaxTree> newAST;

	if (mBody->rewriteAST(newAST)) {
		mBody = std::dynamic_pointer_cast<BlockAST>(newAST);
	}

	mBody->rewrite();
}

void FunctionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);

	auto inner = SymbolTable::newInner(symbolTable);
	mPrototype->generateSymbols(binder, inner);
	mBody->setBlockTable(inner);
	mBody->generateSymbols(binder, inner);
}

void FunctionAST::typeCheck(TypeChecker& checker) {
	mPrototype->typeCheck(checker);
	mBody->typeCheck(checker);
}

void FunctionAST::verify(SemanticVerifier& verifier) {
	mPrototype->verify(verifier);
	mBody->verify(verifier);

	//Check the return statement(s)
	auto checker = verifier.typeChecker();

	auto funcName = mPrototype->name();
	bool anyReturn = false;
	auto returnType = checker.getType(mPrototype->returnType());

	for (auto statement : mBody->statements()) {
		auto returnStatement = std::dynamic_pointer_cast<ReturnStatementAST>(statement->findAST([] (std::shared_ptr<AbstractSyntaxTree> ast) {
			return std::dynamic_pointer_cast<ReturnStatementAST>(ast) != nullptr;
		}));

		if (returnStatement == nullptr) {
			returnStatement = std::dynamic_pointer_cast<ReturnStatementAST>(statement);
		}

		if (returnStatement != nullptr) {
			anyReturn = true;

			if (returnType->name() != "Void") {
				if (returnStatement->returnExpression() == nullptr) {
					verifier.semanticError(returnStatement->asString() + ": Empty return only allowed in void functions.");
				}

				checker.assertSameType(*returnType, *returnStatement->returnExpression()->expressionType(checker), returnStatement->asString());
			} else {
				if (returnStatement->returnExpression() != nullptr) {
					verifier.semanticError(returnStatement->asString() + ": Found expression after return in void function.");
				}
			}
		}
	}

	if (returnType->name() != "Void" && !anyReturn) {
		verifier.semanticError("Expected return statement in function '" + funcName + "'.");
	}
}

void FunctionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mBody->generateCode(codeGen, func);
}