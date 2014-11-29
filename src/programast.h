#pragma once
#include <memory>
#include <string>
#include <vector>

#include "ast.h"

class FunctionAST;
class Binder;
class SymbolTable;

//Represents an program AST
class ProgramAST : public AbstractSyntaxTree {
private:
	std::vector<std::shared_ptr<FunctionAST>> mFunctions;
public:
	ProgramAST(const std::vector<std::shared_ptr<FunctionAST>>& functions);

	//Returns the functions
	const std::vector<std::shared_ptr<FunctionAST>>& functions() const;

	virtual std::string type() const override;

	std::string asString() const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;
};