#pragma once
#include <memory>
#include <string>
#include <vector>

class Type;

//Represents a function parameter
class Parameter {
private:
	std::shared_ptr<Type> mType;
	std::string mName;
public:
	Parameter(std::shared_ptr<Type> type, std::string name);

	//Returns the type of the parameter
	std::shared_ptr<Type> type();

	//Returns the name of the parameter
	std::string name() const;
};

//Represents a function
class Function {
private:
	std::string mName;
	std::vector<Parameter> mParameters;
	std::shared_ptr<Type> mReturnType;
public:
	//Creates a new function
	Function(std::string name, std::vector<Parameter> parameters, std::shared_ptr<Type> returnType);

	Function();

	//Returns the name of the function
	std::string name() const;

	//Returns the parameters
	const std::vector<Parameter>& parameters() const;

	//Returns the return type
	std::shared_ptr<Type> returnType() const;
};