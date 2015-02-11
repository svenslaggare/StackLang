#pragma once
#include <memory>
#include <string>

class ProgramAST;
class TypeChecker;
class SymbolTable;

//Binds references
class Binder {
private:
	std::shared_ptr<SymbolTable> mSymbolTable;
public:
	//Creates a new binder
	Binder();
	
	//Generates the symbol table for the given program
	void generateSymbolTable(std::shared_ptr<ProgramAST> programAST);

	//Returns the symbol table
	std::shared_ptr<SymbolTable> symbolTable() const;

	//Signals that an error has occured
	void error(std::string message);
};