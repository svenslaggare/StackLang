#pragma once
#include "ast.h"

#include <memory>
#include <string>

class Compiler;
class TypeChecker;
class CodeGenerator;
class Symbol;
class SymbolTable;
class VariableSymbol;

//Represents an integer expression
class IntegerExpressionAST : public ExpressionAST {
private:
	int mValue;
public:
	//Creates a new integer expression
	IntegerExpressionAST(int value);

	//Returns the value
	int value() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a bool expression
class BoolExpressionAST : public ExpressionAST {
private:
	bool mValue;
public:
	//Creates a new bool expression
	BoolExpressionAST(bool value);

	//Returns the value
	bool value() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a float expression
class FloatExpressionAST : public ExpressionAST {
private:
	float mValue;
public:
	//Creates a new float expression
	FloatExpressionAST(float value);

	//Returns the value
	float value() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a null reference expression
class NullRefExpressionAST : public ExpressionAST {
public:
	//Creates a new null ref expression
	NullRefExpressionAST();

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a char expression
class CharExpressionAST : public ExpressionAST {
private:
	char mValue;
public:
	//Creates a new char expression
	CharExpressionAST(char value);

	//Returns the value
	char value() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};


//Represents a string expression
class StringExpressionAST : public ExpressionAST {
private:
	std::string mValue;
public:
	//Creates a new string expression
	StringExpressionAST(std::string value);

	//Returns the value
	std::string value() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a variable reference expression
class VariableReferenceExpressionAST : public ExpressionAST {
private:
	std::string mName;
public:
	//Creates a new variable reference expression
	VariableReferenceExpressionAST(std::string name);

	//Returns the name of the variable
	std::string name() const;

	std::string asString() const override;

	//Returns the symbol that the variable references to. Returns null if not bound.
	std::shared_ptr<VariableSymbol> symbol() const;

	virtual void visit(VisitFn visitFn) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a variable declaration expression
class VariableDeclarationExpressionAST : public ExpressionAST, public std::enable_shared_from_this<VariableDeclarationExpressionAST> {
private:
	std::string mType;
	std::string mName;
	bool mIsFunctionParameter;
public:
	//Creates a new variable declaration expression
	VariableDeclarationExpressionAST(std::string type, std::string name, bool isFunctionParameter = false);

	//Returns the type of the variable
	std::string type() const;

	//Returns the name of the variable
	std::string name() const;

	//Indicates if the decleration is of a function parameter
	bool isFunctionParameter() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;

	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};

//Represents a cast expression
class CastExpressionAST : public ExpressionAST {
private:
	std::string mTypeName;
	std::shared_ptr<ExpressionAST> mExpression;
public:
	//Creates a new cast expression
	CastExpressionAST(std::string typeName, std::shared_ptr<ExpressionAST> expression);

	//Returns the name of the type to cast to
	std::string functionName() const;

	//The expression to cast
	std::shared_ptr<ExpressionAST> expression() const;

	std::string asString() const override;

	virtual void visit(VisitFn visitFn) const override;
	
	virtual void rewrite(Compiler& compiler) override;
	
	virtual void generateSymbols(Binder& binder, std::shared_ptr<SymbolTable> symbolTable) override;

	virtual void typeCheck(TypeChecker& checker) override;

	virtual void verify(SemanticVerifier& verifier) override;

	virtual std::shared_ptr<Type> expressionType(const TypeChecker& checker) const override; 

	virtual void generateCode(CodeGenerator& codeGen, GeneratedFunction& func) override;
};