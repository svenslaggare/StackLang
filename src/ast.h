#pragma once
#include <vector>
#include <memory>
#include <iostream>

class SymbolTable;
class Binder;

//Represents an abstract syntax tree
class AbstractSyntaxTree {
protected:
	std::shared_ptr<SymbolTable> mSymbolTable;
public:
	virtual ~AbstractSyntaxTree() {};

	//Returns the type of the tree
	//virtual std::string type() const = 0;
	virtual std::string type() const { return "AST"; }

	//Returns the current AST as a string
	virtual std::string asString() const = 0;

	//Generates symbols
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable);
};

std::ostream& operator<<(std::ostream& os, const AbstractSyntaxTree& ast);

//Represents an expression AST
class ExpressionAST : public AbstractSyntaxTree {

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