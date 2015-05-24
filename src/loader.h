#pragma once
#include <iostream>
#include <vector>
#include <memory>

class Binder;
class TypeChecker;
class Type;
class SymbolTable;

//Represents a function definition
struct FunctionDefinition {
	std::string name;
	const std::vector<std::shared_ptr<Type>> parameters;
	const std::shared_ptr<Type> returnType;

	FunctionDefinition(std::string name, std::vector<std::shared_ptr<Type>> parameters, std::shared_ptr<Type> returnType);
};

//Represents a class definition
struct ClassDefinition {
	std::string name;
	std::vector<std::pair<std::string, std::string>> fields;

	ClassDefinition(std::string name);
	ClassDefinition();
};

//Loads assemblies
class Loader {
private:
	Binder& mBinder;
	TypeChecker& mTypeChecker;

	//Tokenizes from the given stream
	std::vector<std::string> tokenize(std::istream& stream);

	//Returns the type for the given VM type
	std::shared_ptr<Type> getType(std::string vmTypeName);

	//Parses the given function definition
	FunctionDefinition parseFunctionDef(const std::vector<std::string>& tokens, int& tokenIndex);

	//Defines the given function
	void defineFunction(const FunctionDefinition& funcDef, std::shared_ptr<SymbolTable> funcScope = nullptr);

	//Defines the given class
	void defineClass(const ClassDefinition& classDef);

	//Defines the given member function
	void defineMemberFunction(const FunctionDefinition& memberDef);
public:
	//Creates a new loader
	Loader(Binder& binder, TypeChecker& typeChecker);

	//Loads an assembly from the given stream
	void loadAssembly(std::istream& stream);
};