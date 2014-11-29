#pragma once
#include <memory>
#include <map>
#include <string>
#include <iostream>

class AbstractSyntaxTree;

//Represents a symbol table;
class SymbolTable {
private:
	std::shared_ptr<SymbolTable> mOuter;
	std::map<std::string, std::shared_ptr<AbstractSyntaxTree>> mInner;
public:
	//Creates a new symbol table
	SymbolTable(std::shared_ptr<SymbolTable> outer = nullptr);

	//Adds the given symbol to the table. True if added else false.
	bool add(std::string name, std::shared_ptr<AbstractSyntaxTree> symbol);

	//Finds the given symbol. Nullptr if it doesn't exists.
	std::shared_ptr<AbstractSyntaxTree> find(std::string name) const;

	//Returns the entries in the inner table
	const std::map<std::string, std::shared_ptr<AbstractSyntaxTree>>& inner() const;

	//Returns the outer table
	std::shared_ptr<SymbolTable> outer() const;
};

std::ostream& operator<<(std::ostream& os, const SymbolTable& symbolTable);