#pragma once
#include "ast.h"

#include <memory>
#include <string>
#include <vector>
#include <functional>

class TypeChecker;
class CodeGenerator;

//Represents a block AST
class BlockAST : public StatementAST {
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

	virtual void visit(VisitFn visitFn) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};