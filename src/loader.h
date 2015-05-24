#pragma once
#include <iostream>
#include <vector>
#include <memory>

class Binder;
class TypeChecker;
class Type;
class SymbolTable;

namespace AssemblyParser {
	struct Function;
	struct Struct;
}

//Loads assemblies
class Loader {
private:
	Binder& mBinder;
	TypeChecker& mTypeChecker;

	//Returns the type for the given VM type
	std::shared_ptr<Type> getType(std::string vmTypeName);

	//Defines the given function
	void defineFunction(const AssemblyParser::Function& funcDef, std::shared_ptr<SymbolTable> funcScope = nullptr);

	//Defines the given class
	void defineClass(const AssemblyParser::Struct& classDef);

	//Defines the given member function
	void defineMemberFunction(const AssemblyParser::Function& memberDef);
public:
	//Creates a new loader
	Loader(Binder& binder, TypeChecker& typeChecker);

	//Loads an assembly from the given stream
	void loadAssembly(std::istream& stream);
};