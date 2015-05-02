#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>

class Type;

//The access modifiers
enum class AccessModifiers : char {
	Public,
	Private
};

//Returns the given access modifier as a string
std::string to_string(AccessModifiers accessModifier);

std::ostream& operator<<(std::ostream& os, const AccessModifiers& modifier);

//Represents a field in an object
class Field {
private:
	std::string mName;
	std::shared_ptr<Type> mType;
	AccessModifiers mAccessModifier;
public:
	//Creates a new field
	Field(std::string name, std::shared_ptr<Type> type, AccessModifiers accessModifier = AccessModifiers::Public);
	Field();

	//Returns the name
	std::string name() const;

	//Returns the type
	std::shared_ptr<Type> type() const;

	//Returns the access modifier
	AccessModifiers accessModifier() const;
};

//Represents an object
class Object {
private:
	std::string mName;
	std::shared_ptr<Type> mType;
	std::unordered_map<std::string, Field> mFields;
public:
	//Creates a new object
	Object(std::string name, std::shared_ptr<Type> type, std::unordered_map<std::string, Field> fields);
	Object();
	
	//Returns the name
	std::string name() const;

	//Returns the type
	std::shared_ptr<Type> type() const;

	//Returns the fields
	const std::unordered_map<std::string, Field>& fields() const;	

	//Indicates if the given field exists
	bool fieldExists(std::string name) const;

	//Returns the given field
	const Field& getField(std::string name) const;
};