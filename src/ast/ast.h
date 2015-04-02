#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <functional>

class SymbolTable;
class Binder;
class TypeChecker;
class Type;
class CodeGenerator;
class GeneratedFunction;
class SemanticVerifier;
class Compiler;

//Represents an abstract syntax tree
class AbstractSyntaxTree {
protected:
	std::shared_ptr<SymbolTable> mSymbolTable;
public:
	virtual ~AbstractSyntaxTree() {};

	//Returns the type of the tree
	virtual std::string type() const { return "AST"; }

	//Returns the current AST as a string
	virtual std::string asString() const = 0;

	using VisitFn = std::function<void(const AbstractSyntaxTree*)>;

	//Visits all the nodes
	virtual void visit(VisitFn visitFn) const {};

	//Rewrites the children of the current tree
	virtual void rewrite(Compiler& compiler);

	//Rewrites the current tree and returns the result
	virtual bool rewriteAST(std::shared_ptr<AbstractSyntaxTree>& newAST, Compiler& compiler) const;

	//Generates symbols
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable);

	//Type checks
	virtual void typeCheck(TypeChecker& checker);

	//Verifies the AST according to the semantic rules
	virtual void verify(SemanticVerifier& verifier);

	//Generates code
	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func);
};

std::ostream& operator<<(std::ostream& os, const AbstractSyntaxTree& ast);

//Represents an expression AST
class ExpressionAST : public AbstractSyntaxTree {
public:
	//The type returned by the expression
	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const; 
};

//Represents a statement AST
class StatementAST : public AbstractSyntaxTree {

};

namespace AST {
	//Combines the given ASTs into a string with the given seperator
	template <class T>
	std::string combineAST(std::vector<std::shared_ptr<T>> asts, std::string sep) {
		bool isFirst = true;
		std::string str = "";

		for (auto ast : asts) {
			if (!isFirst) {
				str += sep;
			} else {
				isFirst = false;
			}

			str += ast->asString();
		}

		return str;
	}
}