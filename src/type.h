#pragma once
#include <string>
#include <memory>

//Represents a type
class Type {
private:
	const std::string mName;
public:
	//Creates a new type
	Type(std::string name);

	virtual ~Type() {}

	//Returns the name of the type
	const std::string name() const;

	//Determines if the current type equals the given
	bool operator==(const Type& other) const;

	//Determines if the current type not equals the given
	bool operator!=(const Type& other) const;
};

//The primitive types
enum class PrimitiveTypes { Void, Int, Bool, Float };

//Represents a primitive type
class PrimitiveType : public Type {
public:
	//Creates a new primitive type
	PrimitiveType(PrimitiveTypes type);
};

//Represents an array type
class ArrayType : public Type {
private:
	std::shared_ptr<Type> mElementType;
public:
	//Creates a new array with the given element type
	ArrayType(std::shared_ptr<Type> elementType);

	//Returns the element type
	std::shared_ptr<Type> elementType() const;
};

namespace TypeSystem {
	//Creates a primitive type from a string
	bool fromString(std::string typeName, PrimitiveTypes& type);

	//Returns a string for the given primitive type
	std::string toString(PrimitiveTypes type);

	//Creates a type from the given string
	std::shared_ptr<Type> makeType(std::string typeName);
};