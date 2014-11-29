#pragma once
#include <map>
#include <memory>
#include <string>

#include "symboltable.h"

class ProgramAST;

//Binds references
class Binder {
private:
	std::shared_ptr<SymbolTable> mSymbolTable;
public:
	Binder();
	
	//Generates the symbol table for the given program
	void generateSymbolTable(std::shared_ptr<ProgramAST> programAST);

	//Returns the symbol table
	std::shared_ptr<SymbolTable> symbolTable() const;

	//Signals that an error has occured
	void error(std::string message);
};