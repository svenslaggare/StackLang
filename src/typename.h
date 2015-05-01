#pragma once
#include <string>
#include <memory>

class SymbolTable;

//Represents a type name
class TypeName {
private:
	std::string mName;
	bool mIsArray;
	std::unique_ptr<TypeName> mElementTypeName;

	//Creates a new type name
	TypeName(std::string name);

	//Creates a new array type name
	TypeName(std::string name, std::unique_ptr<TypeName> elementTypeName);
public:
	//Constructs the given type name
	static std::unique_ptr<TypeName> make(std::string name);

	//Constructs the given type name using defined symbols
	// static std::unique_ptr<TypeName> make(std::string name, std::shared_ptr<SymbolTable> symbolTable);
	
	//Constructs a full name from the given type name using defined symbols
	static std::unique_ptr<TypeName> makeFull(const TypeName* const typeName, std::shared_ptr<SymbolTable> symbolTable);

	//Returns the name of the type
	std::string name() const;

	//Indicates if the current type name is an array
	bool isArray() const;

	//Returns the name of the element type. Nullptr if not an array.
	const TypeName* const elementTypeName() const;
};