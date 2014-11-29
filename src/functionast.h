#pragma once
#include <memory>
#include <string>
#include <vector>

#include "ast.h"

class VariableDeclerationExpressionAST;
class SymbolTable;
class BlockAST;

//Represents a function AST
class FunctionAST : public AbstractSyntaxTree {
private:
	std::string mName;
	std::vector<std::shared_ptr<VariableDeclerationExpressionAST>> mArguments;
	std::string mReturnType;
	std::shared_ptr<BlockAST> mBody;
public:
	//Creates a new function
	FunctionAST(std::string name, const std::vector<std::shared_ptr<VariableDeclerationExpressionAST>>& arguments, std::string returnType, std::shared_ptr<BlockAST> body);

	//Returns the name
	const std::string name() const;

	//Returns the arguments
	const std::vector<std::shared_ptr<VariableDeclerationExpressionAST>>& arguments() const;

	//Returns the type
	const std::string returnType() const;

	//Returns the body
	std::shared_ptr<BlockAST> body() const;

	virtual std::string type() const override;

	std::string asString() const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;
};