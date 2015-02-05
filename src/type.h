#pragma once
#include <string>
#include <memory>
#include <map>

//Represents a type
class Type {
private:
	const std::string mName;
	const bool mIsReferenceType;
public:
	//Creates a new type
	Type(std::string name, bool isReferenceType = false);

	virtual ~Type() {}

	//Returns the name of the type
	const std::string name() const;

	//Indicates if the type is a reference type
	const bool isReferenceType() const;

	//Determines if the current type equals the given
	bool operator==(const Type& other) const;

	//Determines if the current type not equals the given
	bool operator!=(const Type& other) const;

	//Returns the VM type
	virtual std::string vmType() const;
};

//Represents a type to be inferred
class AutoType : public Type {
public:
	//Creates a new auto type
	AutoType();
};

//The primitive types
enum class PrimitiveTypes { Void, Int, Bool, Float };

//Represents a primitive type
class PrimitiveType : public Type {
public:
	//Creates a new primitive type
	PrimitiveType(PrimitiveTypes type);
};

//Represents a reference type
class ReferenceType : public Type {
public:
	//Creates a new reference type
	ReferenceType(std::string name);

	virtual std::string vmType() const override = 0;
};

//Represents a null reference type
class NullReferenceType : public ReferenceType {
public:
	//Creates a new null ref type
	NullReferenceType();

	virtual std::string vmType() const override;
};

//Represents an array type
class ArrayType : public ReferenceType {
private:
	std::shared_ptr<Type> mElementType;
public:
	//Creates a new array with the given element type
	ArrayType(std::shared_ptr<Type> elementType);

	//Returns the element type
	std::shared_ptr<Type> elementType() const;

	virtual std::string vmType() const override;
};

namespace TypeSystem {
	//Returns the default types
	std::map<std::string, std::shared_ptr<Type>> defaultTypes();

	//Creates a primitive type from a string
	bool fromString(std::string typeName, PrimitiveTypes& type);

	//Returns a string for the given primitive type
	std::string toString(PrimitiveTypes type);

	//Creates a type from the given string
	std::shared_ptr<Type> makeType(std::string typeName);
};