#pragma once
#include "ast.h"

#include <memory>
#include <string>
#include <vector>

class VariableDeclarationExpressionAST;
class SymbolTable;
class BlockAST;
class TypeChecker;
class CodeGenerator;

//Represents a function prototype AST
class FunctionPrototypeAST : public AbstractSyntaxTree {
private:
	std::string mName;
	std::vector<std::shared_ptr<VariableDeclarationExpressionAST>> mParameters;
	std::string mReturnType;

	//Finds the namespace name for the current function
	std::string findNamespaceName(std::shared_ptr<SymbolTable> symbolTable, std::string sep) const;
public:
	//Creates a new function prototype
	FunctionPrototypeAST(std::string name, const std::vector<std::shared_ptr<VariableDeclarationExpressionAST>>& parameters, std::string returnType);

	//Returns the name
	const std::string name() const;

	//Returns the parameters
	const std::vector<std::shared_ptr<VariableDeclarationExpressionAST>>& parameters() const;

	//Returns the type
	const std::string returnType() const;

	//Returns the full name
	std::string fullName(std::string namespaceSep = "::") const;

	virtual std::string type() const override;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;
};

//Represents a function AST
class FunctionAST : public AbstractSyntaxTree {
private:
	std::shared_ptr<FunctionPrototypeAST> mPrototype;
	std::shared_ptr<BlockAST> mBody;
public:
	//Creates a new function
	FunctionAST(std::shared_ptr<FunctionPrototypeAST> prototype, std::shared_ptr<BlockAST> body);

	//Returns the prototype
	const std::shared_ptr<FunctionPrototypeAST> prototype() const;

	//Returns the body
	std::shared_ptr<BlockAST> body() const;

	virtual std::string type() const override;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;
	
	virtual void rewrite() override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};