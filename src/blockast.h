#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "ast.h"

class TypeChecker;
class CodeGenerator;

//Represents a block AST
class BlockAST : public AbstractSyntaxTree {
private:
	std::vector<std::shared_ptr<StatementAST>> mStatements;
	std::shared_ptr<SymbolTable> mBlockTable;
public:
	//Creates a new block
	BlockAST(const std::vector<std::shared_ptr<StatementAST>>& statements);

	//Returns the statements
	const std::vector<std::shared_ptr<StatementAST>>& statements() const;

	//Sets the table that will be merged with the blocks scope
	void setBlockTable(std::shared_ptr<SymbolTable> blockTable);

	std::string asString() const override;

	virtual void rewrite() override;
	
	virtual std::shared_ptr<AbstractSyntaxTree> findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};