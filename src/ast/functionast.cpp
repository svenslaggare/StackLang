#include "functionast.h"
#include "variableast.h"
#include "statementast.h"
#include "blockast.h"
#include "../symboltable.h"
#include "../symbol.h"
#include "../binder.h"
#include "../typechecker.h"
#include "../type.h"
#include "../semantics.h"
#include "../helpers.h"
#include "../typename.h"

//Function prototype AST
FunctionPrototypeAST::FunctionPrototypeAST(std::string name, const std::vector<std::shared_ptr<VariableDeclarationExpressionAST>>& parameters, std::string returnType)
	: mName(name), mParameters(parameters), mReturnType(TypeName::make(returnType)) {

}

const std::string FunctionPrototypeAST::name() const {
	return mName;
}

const std::vector<std::shared_ptr<VariableDeclarationExpressionAST>>& FunctionPrototypeAST::parameters() const {
	return mParameters;
}

const std::string FunctionPrototypeAST::returnType() const {
	return mReturnType->name();
}

std::string FunctionPrototypeAST::findNamespaceName(std::shared_ptr<SymbolTable> symbolTable, std::string sep) const {
	if (symbolTable == nullptr) {
		return "";
	} else {
		auto name = symbolTable->name();

		if (name != "") {
			name += sep;
		}

		return findNamespaceName(symbolTable->outer(), sep) + name;
	}
}

std::string FunctionPrototypeAST::fullName(std::string namespaceSep, bool memberFunc) const {
	if (!memberFunc) {
		return findNamespaceName(mSymbolTable, namespaceSep) + mName;
	} else {
		return findNamespaceName(mSymbolTable->outer()->outer(), namespaceSep)
			   + mSymbolTable->outer()->name() + "::" + mName;
	}
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

	funcStr += "): " + returnType() + " ";
	return funcStr;
}

void FunctionPrototypeAST::visit(VisitFn visitFn) const {
	for (auto param : mParameters) {
		param->visit(visitFn);
	}

	visitFn(this);
}


void FunctionPrototypeAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	
	for (auto param : mParameters) {
		param->generateSymbols(binder, symbolTable);
	}

	//Find the full type name
	mReturnType = std::move(TypeName::makeFull(mReturnType.get(), symbolTable));
}

void FunctionPrototypeAST::typeCheck(TypeChecker& checker) {
	for (auto param : mParameters) {
		param->typeCheck(checker);
	}

	auto returnTypeName = returnType();

	if (auto returnTypeSymbol = std::dynamic_pointer_cast<ClassSymbol>(mSymbolTable->find(returnType()))) {
		returnTypeName = returnTypeSymbol->fullName();
		mReturnType = std::move(TypeName::makeFull(TypeName::make(returnTypeName).get(), mSymbolTable));
	}

	checker.assertTypeExists(returnTypeName, false);
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

std::string FunctionAST::asString() const {
	auto funcStr = mPrototype->asString();
	funcStr += mBody->asString();
	return funcStr;
}

void FunctionAST::visit(VisitFn visitFn) const {
	mPrototype->visit(visitFn);
	mBody->visit(visitFn);
	visitFn(this);
}

void FunctionAST::rewrite(Compiler& compiler) {
	std::shared_ptr<AbstractSyntaxTree> newAST;

	if (mBody->rewriteAST(newAST, compiler)) {
		mBody = std::dynamic_pointer_cast<BlockAST>(newAST);
	}

	mBody->rewrite(compiler);
}

void FunctionAST::bindSignature(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	mBodyTable = SymbolTable::newInner(symbolTable);
	mPrototype->generateSymbols(binder, mBodyTable);
}

void FunctionAST::generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) {
	AbstractSyntaxTree::generateSymbols(binder, symbolTable);
	mBody->setBlockTable(mBodyTable);
	mBody->generateSymbols(binder, mBodyTable);
}

void FunctionAST::typeCheck(TypeChecker& checker) {
	mPrototype->typeCheck(checker);
	mBody->typeCheck(checker);
}

void FunctionAST::checkReturnStatement(SemanticVerifier& verifier, std::shared_ptr<ReturnStatementAST> returnStatement) {
	auto checker = verifier.typeChecker();
	auto returnType = checker.makeType(mPrototype->returnType());

	if (returnType->name() != "Void") {
		if (returnStatement->returnExpression() == nullptr) {
			verifier.semanticError(returnStatement->asString() + ": Empty return statement is only allowed in void functions.");
		}

		auto returnStatementType = returnStatement->returnExpression()->expressionType(checker);

		checker.assertSameType(
			*returnType,
			*returnStatementType,
			returnStatement->asString() + ": Expected type '" + returnType->name() + "' but got type '" + returnStatementType->name() + "' in return statement.",
			true);
	} else {
		if (returnStatement->returnExpression() != nullptr) {
			verifier.semanticError(returnStatement->asString() + ": Found expression after return in void function.");
		}
	}
}

bool FunctionAST::checkBranches(SemanticVerifier& verifier, std::shared_ptr<StatementAST> statement) {
	if (auto returnStatement = std::dynamic_pointer_cast<ReturnStatementAST>(statement)) {
		checkReturnStatement(verifier, returnStatement);
		return true;
	} else if (auto block = std::dynamic_pointer_cast<BlockAST>(statement)) {
		for (auto blockStatement : block->statements()) {
			if (checkBranches(verifier, blockStatement)) {
				return true;
			}
		}

		return false;
	} else if (auto ifStatement = std::dynamic_pointer_cast<IfElseStatementAST>(statement)) {
		bool ifReturns = checkBranches(verifier, ifStatement->thenBlock());

		if (!ifReturns) {
			return false;
		}

		if (ifStatement->elseBlock() != nullptr) {
			return checkBranches(verifier, ifStatement->elseBlock());
		} else {
			return ifReturns;
		}
	} else if (auto whileStatement = std::dynamic_pointer_cast<WhileLoopStatementAST>(statement)) {
		return checkBranches(verifier, whileStatement->bodyBlock());
	} else {
		return false;
	}
}

void FunctionAST::checkReturnStatements(SemanticVerifier& verifier) {
	auto checker = verifier.typeChecker();
	bool allReturns = checkBranches(verifier, mBody);
	auto returnType = checker.makeType(mPrototype->returnType());

	if (returnType->name() != "Void") {
		if (!allReturns) {
			verifier.semanticError("Not all branches returns.");
		}
	}

	// if (returnType->name() != "Void" && !anyReturn) {
	// 	verifier.semanticError("Expected return statement in function '" + funcName + "'.");
	// }
}

void FunctionAST::verify(SemanticVerifier& verifier) {
	mPrototype->verify(verifier);
	mBody->verify(verifier);

	//Check the return statement(s)
	checkReturnStatements(verifier);
}

void FunctionAST::generateCode(CodeGenerator& codeGen, GeneratedFunction& func) {
	mBody->generateCode(codeGen, func);
}