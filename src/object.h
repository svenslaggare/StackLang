#pragma once
#include <memory>
#include <string>
#include <unordered_map>

class Type;

//The access modifiers
enum class AccessModifiers {
	Public,
	Private
};

//Represents a field in an object
class Field {
private:
	std::string mName;
	std::shared_ptr<Type> mType;
public:
	//Creates a new field
	Field(std::string name, std::shared_ptr<Type> type);
	Field();

	//Returns the name
	std::string name() const;

	//Returns the type
	std::shared_ptr<Type> type() const;
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