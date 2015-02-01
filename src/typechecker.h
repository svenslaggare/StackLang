#pragma once
#include <map>
#include <memory>
#include <string>

class Type;
class ProgramAST;
class OperatorContainer;

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
	std::map<std::string, std::shared_ptr<Type>> mTypes;
	const OperatorContainer& mOperators;
	std::multimap<std::shared_ptr<Type>, ExplicitConversion> mExplicitConversions;
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
	void defineExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType, ExplicitConversionFunction conversionFunc);

	//Indicates if its an explicit conversion between the given types
	bool existsExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType) const;

	//Returns an explicit conversion between the given types. Exception if no conversion exists.
	const ExplicitConversion& getExplicitConversion(std::shared_ptr<Type> fromType, std::shared_ptr<Type> toType) const;
};