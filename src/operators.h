#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>

//Represents an operator
class Operator {
private:
	bool mIsTwoChars;
	char mOp1;
	char mOp2;
public:
	//Creates a new single-character operator
	explicit Operator(char op1);

	//Creates a new two-characters operator
	Operator(char op1, char op2);

	//Indicates if the current operator consists of two characters
	bool isTwoChars() const;

	//Returns the first op character
	char op1() const;

	//Returns the second op character. If not a two-character operator the functions throws an exception.
	char op2() const;

	bool operator<(const Operator& rhs) const;
	bool operator==(const Operator& rhs) const;
	bool operator!=(const Operator& rhs) const;

	//Returns the current operator as a string
	std::string asString() const;
};

class Type;

//Represents an operator container
class OperatorContainer {
private:
	std::map<Operator, int> mBinaryOperators;
	std::set<Operator> mUnaryOperators;
	std::set<char> mAssignmentOperators;
	std::map<Operator, std::shared_ptr<Type>> mBinaryOpReturnTypes;
public:
	//Creates a new operator container
	OperatorContainer(std::map<Operator, int> binaryOperators, std::set<Operator> unaryOperators,
					  std::set<char> assignmentOperators, std::map<Operator, std::shared_ptr<Type>> binaryOpReturnTypes);

	//Returns the binary operators
	std::vector<Operator> binaryOperators() const;

	//Returns the precedence for the binary operators
	const std::map<Operator, int>& binaryOperatorPrecedences() const;

	//Returns the unary opertators
	const std::set<Operator>& unaryOperators() const;

	//Return the return types for binary operators. Only operators that return something different than the operands is listed.
	const std::map<Operator, std::shared_ptr<Type>>& binaryOpReturnTypes() const;

	//Returns the precedence for the given operator
	int getPrecedence(Operator op) const;

	//Indicates if the given binary operator is defined
	bool isBinaryDefined(Operator op) const;

	//Indicates if the given unary operator is defined
	bool isUnaryDefined(Operator op) const;
};