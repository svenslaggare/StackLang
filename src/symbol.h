#pragma once
#include "namespace.h"
#include "object.h"
#include <string>
#include <memory>
#include <vector>

class SymbolTable;

//Represents a symbol
class Symbol {
private:
	std::string mName;
	std::string mType;
	std::string mScopeName;
public:
	//Creates a new symbol of the given type
	Symbol(std::string name, std::string type);

	//Returns the name of the symbol
	std::string name() const;

	//Returns the type of the symbol
	std::string type() const;

	//The name of the scope that the symbol is defined in
	std::string scopeName() const;

	//Returns the symbol as a string
	virtual std::string asString() const;

	friend class SymbolTable;
};

//The attributes for variable symbols
enum class VariableSymbolAttribute {
	NONE,
	FUNCTION_PARAMETER,
	FIELD,
	THIS_REFERENCE
};

//Represents a variable symbol
class VariableSymbol : public Symbol {
private:
	std::string mVariableType;
	VariableSymbolAttribute mAttribute;
public:
	//Creates a new variable symbol of the given type
	VariableSymbol(std::string name, std::string variableType, VariableSymbolAttribute attribute = VariableSymbolAttribute::NONE);

	virtual std::string asString() const override;

	//Returns the type of the variable
	std::string variableType() const;

	//Return the attribute
	VariableSymbolAttribute attribute() const;
};

//Represents a function signature symbol
class FunctionSignatureSymbol : public Symbol {
private:
	std::vector<VariableSymbol> mParameters;
	std::string mReturnType;
	AccessModifiers mAccessModifier;
public:
	//Creates a new function symbol with the given parameters and return type
	FunctionSignatureSymbol(std::string name, std::vector<VariableSymbol> parameters,
							std::string returnType, AccessModifiers accessModifier = AccessModifiers::Public);

	virtual std::string asString() const override;

	//Returns the parameters
	const std::vector<VariableSymbol>& parameters() const;

	//Returns the return type
	std::string returnType() const;

	//Returns the access modifier
	AccessModifiers accessModifier() const;
};

//Represents a function symbol
class FunctionSymbol : public Symbol {
private:
	std::vector<std::shared_ptr<FunctionSignatureSymbol>> mOverloads;
	Namespace mDefinedNamespace;
	bool mIsMember;
public:
	//Creates a new function symbol with the given signature
	FunctionSymbol(std::string name,
				   std::shared_ptr<FunctionSignatureSymbol> signature,
				   Namespace definedNamespace = {}, bool isMember = false);

	//Returns the namespace that the function is defined in
	const Namespace& definedNamespace() const;

	//Indicates if the current function is a member function
	bool isMember() const;

	virtual std::string asString() const override;

	//Returns the overloads
	const std::vector<std::shared_ptr<FunctionSignatureSymbol>>& overloads() const;

	//Adds a new overload
	bool addOverload(std::shared_ptr<FunctionSignatureSymbol> signature);

	//Tries to find an overload with the given signature
	std::shared_ptr<FunctionSignatureSymbol> findOverload(std::vector<std::string> parameterTypes) const;
};

//Represents a namespace symbol
class NamespaceSymbol : public Symbol {
private:
	std::shared_ptr<SymbolTable> mSymbolTable;
public:
	//Creates a new namespace with the given symbols
	NamespaceSymbol(std::string name, std::shared_ptr<SymbolTable> symbolTable);

	//Returns the symbol table
	std::shared_ptr<SymbolTable> symbolTable() const;
};

//Represents a class symbol
class ClassSymbol : public Symbol {
private:
	std::shared_ptr<SymbolTable> mSymbolTable;
	Namespace mDefinedNamespace;
public:
	//Creates a new class with the given symbols
	ClassSymbol(std::string name, std::shared_ptr<SymbolTable> symbolTable, Namespace definedNamespace = {});

	//Returns the symbol table
	std::shared_ptr<SymbolTable> symbolTable() const;

	//Returns the namespace that the class is defined in
	const Namespace& definedNamespace() const;

	//Returns the full name
	std::string fullName() const;
};