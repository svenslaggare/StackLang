#include "asthelpers.h"
#include "ast.h"
#include "functionast.h"

std::shared_ptr<FunctionPrototypeAST> ASTHelpers::asPrototype(std::shared_ptr<AbstractSyntaxTree> ast) {
	std::shared_ptr<FunctionPrototypeAST> funcPrototype;

	if (!(funcPrototype = std::dynamic_pointer_cast<FunctionPrototypeAST>(ast))) {
		funcPrototype = std::dynamic_pointer_cast<FunctionAST>(ast)->prototype();
	}

	return funcPrototype;
}

bool ASTHelpers::findAST(std::shared_ptr<AbstractSyntaxTree> ast, std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate, std::shared_ptr<AbstractSyntaxTree>& result) {
	if (predicate(ast)) {
		result = ast;
		return true;
	} else {
		if (auto childAst = ast->findAST(predicate)) {
			result = childAst;
			return true;
		}
	}

	return false;
}