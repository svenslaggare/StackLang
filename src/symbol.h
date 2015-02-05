#pragma once
#include <string>
#include <memory>
#include <vector>

//Represents a symbol
class Symbol {
private:
	std::string mName;
	std::string mType;
public:
	//Creates a new symbol of the given type
	Symbol(std::string name, std::string type);

	//Returns the name of the symbol
	std::string name() const;

	//Returns the type of the symbol
	std::string type() const;

	//Returns the symbol as a string
	virtual std::string asString() const;
};

//Represents a variable symbol
class VariableSymbol : public Symbol {
private:
	std::string mVariableType;
	bool mIsFunctionParameter;
public:
	//Creates a new variable symbol of the given type
	VariableSymbol(std::string name, std::string variableType, bool isFunctionParameter = false);

	virtual std::string asString() const override;

	//Returns the type of the variable
	std::string variableType() const;

	//Indicates if the variable is a function parameter
	bool isFunctionParameter() const;
};

//Represents a function signature symbol
class FunctionSignatureSymbol : Symbol {
private:
	std::vector<VariableSymbol> mParameters;
	std::string mReturnType;
public:
	//Creates a new function symbol with the given parameters and return type
	FunctionSignatureSymbol(std::string name, std::vector<VariableSymbol>parameters, std::string returnType);

	virtual std::string asString() const override;

	//Returns the parameters
	const std::vector<VariableSymbol>& parameters() const;

	//Returns the return type
	std::string returnType() const;
};

//Represents a function symbol
class FunctionSymbol : public Symbol {
private:
	std::vector<std::shared_ptr<FunctionSignatureSymbol>> mOverloads;
public:
	//Creates a new function symbol with the given signature
	FunctionSymbol(std::string name, std::shared_ptr<FunctionSignatureSymbol> signature);

	virtual std::string asString() const override;

	//Returns the overloads
	const std::vector<std::shared_ptr<FunctionSignatureSymbol>>& overloads() const;

	//Adds a new overload
	bool addOverload(std::shared_ptr<FunctionSignatureSymbol> signature);

	//Tries to find an overload with the given signature
	std::shared_ptr<FunctionSignatureSymbol> findOverload(std::vector<std::string> parameterTypes) const;
};

//Represents a conversion symbol
class ConversionSymbol : public Symbol {
public:
	//Creates a new conversion symbol
	ConversionSymbol(std::string name);

	virtual std::string asString() const override;
};