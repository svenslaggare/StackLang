#pragma once
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <iostream>

class AbstractSyntaxTree;
class Symbol;

//Represents a symbol table;
class SymbolTable {
private:
	std::shared_ptr<SymbolTable> mOuter;
	std::map<std::string, std::shared_ptr<Symbol>> mInner;
public:
	//Creates a new symbol table
	SymbolTable(std::shared_ptr<SymbolTable> outer = nullptr);

	//Adds the given symbol to the table. True if added else false.
	bool add(std::string name, std::shared_ptr<Symbol> symbol);

	//Adds the given table to the current
	void add(const SymbolTable& symbolTable);

	//Finds the given symbol. Nullptr if it doesn't exists.
	std::shared_ptr<Symbol> find(std::string name) const;

	//Updates the given symbol.
	void set(std::string name, std::shared_ptr<Symbol> symbol);

	//Returns the entries in the inner table
	const std::map<std::string, std::shared_ptr<Symbol>>& inner() const;

	//Returns the outer table
	std::shared_ptr<SymbolTable> outer() const;

	//Creates a new symbol table for the given outer
	static std::shared_ptr<SymbolTable> newInner(std::shared_ptr<SymbolTable> outer);
};

std::ostream& operator<<(std::ostream& os, const SymbolTable& symbolTable);