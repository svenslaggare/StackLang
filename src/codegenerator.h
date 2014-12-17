#pragma once
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

class FunctionPrototypeAST;
class ProgramAST;
class Type;

//Represents a generated function
class GeneratedFunction {
private:
	std::shared_ptr<FunctionPrototypeAST> mPrototype;
	std::vector<std::shared_ptr<Type>> mLocals;
	std::vector<std::string> mInstructions;
public:
	//Creates a new generated function
	GeneratedFunction(std::shared_ptr<FunctionPrototypeAST> prototype);
	GeneratedFunction();

	//Returns the number of locals
	int numLocals() const;

	//Creates a new local
	int newLocal(std::shared_ptr<Type> type);

	//Returns the given local
	std::shared_ptr<Type> getLocal(int index) const;

	//Returns the index for the given function parameter
	int functionParameterIndex(std::string paramName) const;

	//Adds a new instructions
	void addInstruction(const std::string& instruction);

	//Ouputs the generated code to the given stream
	void outputGeneratedCode(std::ostream& os);
};

//Represents a code generator
class CodeGenerator {
private:
	std::map<std::string, GeneratedFunction> mFunctions;
public:
	//Generates the program
	void generateProgram(std::shared_ptr<ProgramAST> programAST);

	//Creates a new function
	GeneratedFunction& newFunction(std::shared_ptr<FunctionPrototypeAST> functionPrototype);

	//Returns the stream for the given function
	GeneratedFunction& getFunction(std::string funcName);

	//Prints the generated code
	void printGeneratedCode();

	//Indicates that a code gen error has occured
	void codeGenError(std::string errorMessage);
};