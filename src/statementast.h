#pragma once
#include <memory>
#include <string>

#include "ast.h"

class BlockAST;
class TypeChecker;
class CodeGenerator;

//Represents an expression statement
class ExpressionStatementAST : public StatementAST {
private:
	std::shared_ptr<ExpressionAST> mExpression;
public:
	//Creates an expression statement
	ExpressionStatementAST(std::shared_ptr<ExpressionAST> expression);

	//Returns the expression
	std::shared_ptr<ExpressionAST> expression() const;

	std::string asString() const override;

	virtual void rewrite() override;

	virtual std::shared_ptr<AbstractSyntaxTree> findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a return statement
class ReturnStatementAST : public StatementAST {
private:
	std::shared_ptr<ExpressionAST> mReturnExpression;
public:
	//Creates a new return statement
	ReturnStatementAST(std::shared_ptr<ExpressionAST> returnExpression);

	//Creates a new (void) return statement
	ReturnStatementAST();

	//Returns the return expression
	std::shared_ptr<ExpressionAST> returnExpression() const;

	std::string asString() const override;

	virtual void rewrite() override;

	virtual std::shared_ptr<AbstractSyntaxTree> findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents an if and else statement
class IfElseStatementAST : public StatementAST {
private:
	std::shared_ptr<ExpressionAST> mConditionExpression;
	std::shared_ptr<BlockAST> mThenBlock;
	std::shared_ptr<BlockAST> mElseBlock;
public:
	//Creates a new for statement
	IfElseStatementAST(std::shared_ptr<ExpressionAST> conditionExpression, std::shared_ptr<BlockAST> thenBlock, std::shared_ptr<BlockAST> elseBlock);

	//Returns the condition expression
	std::shared_ptr<ExpressionAST> conditionExpression() const;

	//Returns the then block
	std::shared_ptr<BlockAST> thenBlock() const;

	//Returns the else block
	std::shared_ptr<BlockAST> elseBlock() const;

	std::string asString() const override;

	virtual void rewrite() override;

	virtual std::shared_ptr<AbstractSyntaxTree> findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a while loop statement
class WhileLoopStatementAST : public StatementAST {
private:
	std::shared_ptr<ExpressionAST> mConditionExpression;
	std::shared_ptr<BlockAST> mBodyBlock;
public:
	//Creates a new while statement
	WhileLoopStatementAST(std::shared_ptr<ExpressionAST> conditionExpression,std::shared_ptr<BlockAST> bodyBlock);

	//Returns the condition expression
	std::shared_ptr<ExpressionAST> conditionExpression() const;

	//Returns the body block
	std::shared_ptr<BlockAST> bodyBlock() const;

	std::string asString() const override;

	virtual void rewrite() override;

	virtual std::shared_ptr<AbstractSyntaxTree> findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a for loop statement
class ForLoopStatementAST : public StatementAST {
private:
	std::shared_ptr<ExpressionAST> mInitExpression;
	std::shared_ptr<ExpressionAST> mConditionExpression;
	std::shared_ptr<ExpressionAST> mChangeExpression;
	std::shared_ptr<BlockAST> mBodyBlock;
public:
	//Creates a new if and else statement
	ForLoopStatementAST(
		std::shared_ptr<ExpressionAST> initExpression, std::shared_ptr<ExpressionAST> conditionExpression,
		std::shared_ptr<ExpressionAST> changeExpression, std::shared_ptr<BlockAST> bodyBlock);

	//Returns the init expression
	std::shared_ptr<ExpressionAST> initExpression() const;

	//Returns the condition expression
	std::shared_ptr<ExpressionAST> conditionExpression() const;

	//Returns the change expression
	std::shared_ptr<ExpressionAST> changeExpression() const;

	//Returns the body block
	std::shared_ptr<BlockAST> bodyBlock() const;

	std::string asString() const override;

	// virtual void rewrite() override;
	
	virtual bool rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST) const override;
	
	// virtual std::shared_ptr<AbstractSyntaxTree> findAST(std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate) const override;

	// virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	// virtual void typeCheck(TypeChecker& checker) override;

	// virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};