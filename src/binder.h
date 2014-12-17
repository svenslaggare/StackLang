#pragma once
#include <map>
#include <memory>
#include <string>

#include "symboltable.h"

class ProgramAST;
class Program;

//Binds references
class Binder {
private:
	std::shared_ptr<SymbolTable> mSymbolTable;
	std::shared_ptr<Program> mProgram;
public:
	//Creates a new binder for the given program
	Binder(std::shared_ptr<Program> program);
	
	//Generates the symbol table for the given program
	void generateSymbolTable(std::shared_ptr<ProgramAST> programAST);

	//Adds a new function
	void addFunction(std::string name, const std::vector<std::pair<std::string, std::string>>& parameters, std::string returnType);

	//Returns the program being bound
	std::shared_ptr<Program> program() const;

	//Returns the symbol table
	std::shared_ptr<SymbolTable> symbolTable() const;

	//Signals that an error has occured
	void error(std::string message);
};