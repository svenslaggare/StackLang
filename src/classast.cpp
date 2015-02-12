#include "classast.h"
#include "helpers.h"
#include "functionast.h"
#include "expressionast.h"

ClassDefinitionAST::ClassDefinitionAST(
	std::string name,
	std::vector<std::shared_ptr<VariableDeclarationExpressionAST>> fields,
	std::vector<std::shared_ptr<FunctionAST>> functions)
	: mName(name), mFields(fields), mFunctions(functions) {
	
}

std::string ClassDefinitionAST::asString() const {
	return "class " + mName + " {\n" + AST::combineAST(mFields, "\n") + "\n\n" + AST::combineAST(mFunctions, "\n\n") + "\n}";
}