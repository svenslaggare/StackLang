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
class TypeChecker;

using Local = std::pair<int, std::shared_ptr<Type>>;

//Represents a function parameter
struct FunctionParameter {
	const std::string name;
	const std::shared_ptr<Type> type;

	//Creates a new function parameter
	FunctionParameter(std::string name, std::shared_ptr<Type> type);
	FunctionParameter();
};

//Represents a generated function
class GeneratedFunction {
private:
	std::string mFunctionName;
	std::vector<FunctionParameter> mParameters;
	std::shared_ptr<Type> mReturnType;

	std::map<std::string, Local> mLocals;
	std::vector<std::string> mInstructions;
	std::vector<int> mReturnBranches;
public:
	//Creates a new generated function
	GeneratedFunction(std::string functionName, std::vector<FunctionParameter> parameters, std::shared_ptr<Type> returnType);
	GeneratedFunction();

	//Returns the number of locals
	int numLocals() const;

	const std::map<std::string, std::pair<int, std::shared_ptr<Type>>>& locals() const;

	//Creates a new local
	int newLocal(std::string name, std::shared_ptr<Type> type);

	//Returns the given local
	Local getLocal(std::string name) const;

	//Returns the index for the given function parameter
	int functionParameterIndex(std::string paramName) const;

	//Adds a new instructions
	void addInstruction(const std::string& instruction);

	//Returns the number of instructions
	int numInstructions() const;

	//Returns the instruction at the given index
	std::string& instruction(int index);

	//Adds the instruction at the given index to the list of return branches
	void addReturnBranch(int index);

	//Ouputs the generated code to the given stream
	void outputGeneratedCode(std::ostream& os);
};

//Represents a code generator
class CodeGenerator {
private:
	std::vector<GeneratedFunction> mFunctions;
	const TypeChecker& mTypeChecker;
public:
	//Creates a new type checker
	CodeGenerator(const TypeChecker& typeChecker);

	//Returns the type checker
	const TypeChecker& typeChecker() const;

	//Generates the program
	void generateProgram(std::shared_ptr<ProgramAST> programAST);

	//Creates a new function
	GeneratedFunction& newFunction(std::string functionName, std::shared_ptr<FunctionPrototypeAST> functionPrototype);

	//Prints the generated code
	void printGeneratedCode();

	//Indicates that a code gen error has occured
	void codeGenError(std::string errorMessage);

	//The name of the return value local
	static std::string returnValueLocal;
};