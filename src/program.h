#pragma once
#include <map>
#include <string>

#include "function.h"

class ProgramAST;

//Represents a program
class Program {
private:
	std::map<std::string, Function> mFunctions;
public:
	//Creates a new program
	Program();

	//Returns the function with the given name
	bool getFunction(std::string name, Function& function) const;

	//Adds a new function
	bool addFunction(Function function);

	//Extracts the functions from the given program AST
	void extractFunctions(std::shared_ptr<ProgramAST> programAST);
};