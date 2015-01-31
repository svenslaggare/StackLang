#pragma once
#include <map>
#include <memory>
#include <string>

class Type;
class ProgramAST;
class OperatorContainer;

//Represents a type checker
class TypeChecker {
private:
	std::map<std::string, std::shared_ptr<Type>> mTypes;
	const OperatorContainer& mOperators;
	std::multimap<std::shared_ptr<Type>, std::shared_ptr<Type>> mExplicitConversions;
public:
	//Creates a new type checker
	TypeChecker(const OperatorContainer& operators, std::map<std::string, std::shared_ptr<Type>> types);

	//Returns the defined operators
	const OperatorContainer& operators() const;

	//Returns the given type. Nullptr if not found.
	std::shared_ptr<Type> getType(std::string typeName) const;

	//Adds the given type
	bool addType(std::shared_ptr<Type> type);

	//Indicates if the given type exists
	bool typeExists(std::string name) const;

	//Indicates that a type error has occured
	void typeError(std::string message);

	//Asserts that a type with the given name exists
	bool assertTypeExists(std::string name, bool allowAuto = true);

	//Asserts that the types are equal
	bool assertSameType(const Type& expected, const Type& actual, std::string errorMessage = "");

	//Defines an explicit conversion
	void defineExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType);

	//Indicates if its an explicit conversion between the given types
	bool existsExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType) const;
};