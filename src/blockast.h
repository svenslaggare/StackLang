#pragma once
#include <memory>
#include <string>
#include <vector>

#include "ast.h"

//Represents a block AST
class BlockAST : public AbstractSyntaxTree {
private:
	std::vector<std::shared_ptr<StatementAST>> mStatements;
public:
	//Creates a new block
	BlockAST(const std::vector<std::shared_ptr<StatementAST>>& statements);

	//Returns the statements
	const std::vector<std::shared_ptr<StatementAST>>& statements() const;

	std::string asString() const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;
};