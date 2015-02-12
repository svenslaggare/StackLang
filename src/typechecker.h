#pragma once
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include "object.h"

class Type;
class ProgramAST;
class OperatorContainer;

class Binder;
class CodeGenerator;
class GeneratedFunction;

using ExplicitConversionFunction = std::function<void (CodeGenerator&, GeneratedFunction&)>;

//Represents an explict conversion
class ExplicitConversion {
private:
	std::shared_ptr<Type> mFromType;
	std::shared_ptr<Type> mToType;
	ExplicitConversionFunction mConversionGenerator;
public:
	//Creates a new explict conversion
	ExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType, ExplicitConversionFunction conversionGenerator);

	//Returns the from type
	std::shared_ptr<Type> fromType() const;

	//Returns the to type
	std::shared_ptr<Type> toType() const;

	//Applies the conversion
	void applyConversion(CodeGenerator& codeGen, GeneratedFunction& func) const;
};

//Represents a type checker
class TypeChecker {
private:
	Binder& mBinder;

	std::map<std::string, std::shared_ptr<Type>> mTypes;
	const OperatorContainer& mOperators;
	std::multimap<std::shared_ptr<Type>, ExplicitConversion> mExplicitConversions;
	std::unordered_map<std::string, Object> mObjects;

	//Tries to construct the type if doesn't exist
	bool tryMakeType(std::string name);
public:
	//Creates a new type checker
	TypeChecker(Binder& binder, const OperatorContainer& operators, std::map<std::string, std::shared_ptr<Type>> types);

	//Returns the binder
	Binder& binder();

	//Returns the defined operators
	const OperatorContainer& operators() const;

	//Returns the given type. Nullptr if not found.
	std::shared_ptr<Type> findType(std::string typeName) const;

	//Returns the given type. It not found it will try to construct it else nullptr.
	std::shared_ptr<Type> getType(std::string typeName);

	//Adds the given type
	bool addType(std::shared_ptr<Type> type);

	//Indicates if the given type exists
	bool typeExists(std::string name) const;

	//Indicates that a type error has occured
	void typeError(std::string message);

	//Asserts that a type with the given name exists
	bool assertTypeExists(std::string name, bool allowAuto = true);

	//Asserts that the given type is not void
	bool assertNotVoid(const Type& type, std::string errorMessage = "");

	//Asserts that the types are equal
	bool assertSameType(const Type& expected, const Type& actual, std::string errorMessage = "", bool customError = false);

	//Defines an explicit conversion
	void defineExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType, ExplicitConversionFunction conversionFunc);

	//Indicates if its an explicit conversion between the given types
	bool existsExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType) const;

	//Returns an explicit conversion between the given types. Exception if no conversion exists.
	const ExplicitConversion& getExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType) const;

	//Adds the given object
	void addObject(const Object& object);

	//Indicates if the given object exists
	bool objectExists(std::string name) const;

	//Returns the given object
	const Object& getObject(std::string name) const;
};