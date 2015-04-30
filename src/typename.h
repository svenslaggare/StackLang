#pragma once
#include <string>
#include <memory>

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
	TypeName make(std::string name);

	//Returns the name of the type
	std::string name() const;

	//Indicates if the current type name is an array
	bool isArray() const;

	//Returns the name of the element type. Nullptr if not an array.
	const TypeName* const elementTypeName() const;
};