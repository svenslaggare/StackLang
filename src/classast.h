#pragma once
#include "ast.h"

#include <vector>
#include <memory>

class VariableDeclarationExpressionAST;
class FunctionAST;

//Represents a class definition AST
class ClassDefinitionAST : public AbstractSyntaxTree {
private:
	std::string mName;
	std::vector<std::shared_ptr<VariableDeclarationExpressionAST>> mFields;
	std::vector<std::shared_ptr<FunctionAST>> mFunctions;
public:
	//Creates a new class definition using the given fields and functions
	ClassDefinitionAST(
		std::string mName,
		std::vector<std::shared_ptr<VariableDeclarationExpressionAST>> fields,
		std::vector<std::shared_ptr<FunctionAST>> functions);

	virtual std::string asString() const override;
};