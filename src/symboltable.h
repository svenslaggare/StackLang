#pragma once
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <iostream>

class AbstractSyntaxTree;
class Symbol;
class VariableSymbol;
class Namespace;

enum class AccessModifiers : char;

//Represents a symbol table
class SymbolTable {
private:
	int mScopesCreated = 0;
	std::string mScopeName;
	std::string mName = "";
	std::shared_ptr<SymbolTable> mOuter;
	std::map<std::string, std::shared_ptr<Symbol>> mInner;

	//Returns the namespace that the symbol table is defined in
	Namespace getNamespace();

	//Gets the namespace parts
	void getNamespaceParts(const SymbolTable* symbolTable, std::vector<std::string>& parts) const;
public:
	//Creates a new symbol table
	SymbolTable(std::shared_ptr<SymbolTable> outer = nullptr, std::string name = "");

	//Returns the name of the scope
	std::string scopeName() const;

	//Returns the name of the table
	std::string name() const;

	//Adds the given symbol to the table. True if added else false.
	bool add(std::string name, std::shared_ptr<Symbol> symbol);

	//Adds the given table to the current
	void add(const SymbolTable& symbolTable);

	//Adds the given function to the symbol table
	bool addFunction(std::string name, std::vector<VariableSymbol> parameters, std::string returnType);

	//Adds the given member function to the symbol table
	bool addMemberFunction(std::string name, std::vector<VariableSymbol> parameters, std::string returnType, AccessModifiers accessModifier);

	//Creates a new function and adds it to the symbol table
	void newFunction(std::string name, const std::vector<std::pair<std::string, std::string>>& parameters, std::string returnType);

	//Adds the given class to the symbol table
	bool addClass(std::string name, std::shared_ptr<SymbolTable> classTable);

	//Finds the given symbol. Nullptr if it doesn't exists.
	std::shared_ptr<Symbol> find(std::string name) const;

	//Updates the given symbol.
	void set(std::string name, std::shared_ptr<Symbol> symbol);

	//Removes the given symbol
	void remove(std::string name);

	//Returns the entries in the inner table
	const std::map<std::string, std::shared_ptr<Symbol>>& inner() const;

	//Returns the outer table
	std::shared_ptr<SymbolTable> outer() const;

	//Indicates if the current symbol table contains the given
	bool containsTable(std::shared_ptr<SymbolTable> symbolTable) const;

	//Creates a new symbol table for the given outer
	static std::shared_ptr<SymbolTable> newInner(std::shared_ptr<SymbolTable> outer);
};

std::ostream& operator<<(std::ostream& os, const SymbolTable& symbolTable);