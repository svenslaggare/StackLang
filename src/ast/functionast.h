#pragma once
#include "ast.h"

#include <memory>
#include <string>
#include <vector>

class Compiler;
class SymbolTable;
class TypeChecker;
class CodeGenerator;
class BlockAST;
class VariableDeclarationExpressionAST;
class ReturnStatementAST;
class TypeName;

//Represents a function prototype AST
class FunctionPrototypeAST : public AbstractSyntaxTree {
private:
	std::string mName;
	std::vector<std::shared_ptr<VariableDeclarationExpressionAST>> mParameters;
	std::unique_ptr<TypeName> mReturnType;

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
	std::string fullName(std::string namespaceSep = "::", bool memberFunc = false) const;

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
	std::shared_ptr<SymbolTable> mBodyTable;

	//Checks the given return statement
	void checkReturnStatement(SemanticVerifier& verifier, std::shared_ptr<ReturnStatementAST> returnStatement);

	//Checks the branches in the function
	bool checkBranches(SemanticVerifier& verifier, std::shared_ptr<StatementAST> statement);

	//Checks the return statements
	void checkReturnStatements(SemanticVerifier& verifier);
public:
	//Creates a new function
	FunctionAST(std::shared_ptr<FunctionPrototypeAST> prototype, std::shared_ptr<BlockAST> body);

	//Returns the prototype
	const std::shared_ptr<FunctionPrototypeAST> prototype() const;

	//Returns the body
	std::shared_ptr<BlockAST> body() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;
	
	virtual void rewrite(Compiler& compiler) override;

	//Binds the signature of the function
	void bindSignature(Binder& binder, std::shared_ptr<SymbolTable> symbolTable);
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};