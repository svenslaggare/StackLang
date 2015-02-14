#include "operators.h"

//Operator
Operator::Operator(char op1)
	: mOp1(op1), mOp2(0), mIsTwoChars(false) {

}

Operator::Operator(char op1, char op2)
	: mOp1(op1), mOp2(op2), mIsTwoChars(true) {

}

bool Operator::isTwoChars() const {
	return mIsTwoChars;
}

char Operator::op1() const {
	return mOp1;
}

char Operator::op2() const {
	if (!mIsTwoChars) {
		throw std::logic_error("The current operator isn't a two-char operator.");
	}

	return mOp2;
}

bool Operator::operator<(const Operator& rhs) const {
	if (mIsTwoChars < rhs.mIsTwoChars) {
		return true;
	} else if (mIsTwoChars > rhs.mIsTwoChars) {
		return false;
	} else {
		if (mIsTwoChars) {
			if (mOp1 < rhs.mOp1) {
				return true;
			} else if (mOp1 > rhs.mOp1) {
				return false;
			} else {
				return mOp2 < rhs.mOp2;
			}
		} else {
			return mOp1 < rhs.mOp1;
		}
	}
}

bool Operator::operator==(const Operator& rhs) const {
	return mIsTwoChars == rhs.mIsTwoChars && mOp1 == rhs.mOp1 && mOp2 == rhs.mOp2;
}

bool Operator::operator!=(const Operator& rhs) const {
	return !((*this) == rhs);
}

Operator::Hash_t Operator::HASH = [](const Operator& op) {
	return op.mOp1 + op.mOp2 + op.mIsTwoChars;
};

std::string Operator::asString() const {
	return mIsTwoChars ? (std::string("") + mOp1 + mOp2) : std::string("") + mOp1;
}

//OperatorContainer
OperatorContainer::OperatorContainer(
	std::map<Operator, int> binaryOperators,
	std::set<Operator> unaryOperators,
	std::unordered_set<char> assignmentOperators,
	std::map<Operator, std::shared_ptr<Type>> binaryOpReturnTypes)
	: mBinaryOperators(binaryOperators),
	  mUnaryOperators(unaryOperators),
	  mAssignmentOperators(assignmentOperators),
	  mBinaryOpReturnTypes(binaryOpReturnTypes) {
	
	for (auto op : mBinaryOperators) {
		mBinaryOperatorChars.push_back(op.first);
		mOperatorChars.insert(op.first.op1());

		if (op.first.isTwoChars()) {
			mOperatorChars.insert(op.first.op2());
			mTwoCharOpChars.insert(op.first.op2());
		}
	}

	for (auto op : mUnaryOperators) {
		mOperatorChars.insert(op.op1());
	}
}

const std::unordered_set<char>& OperatorContainer::operatorChars() const {
	return mOperatorChars;
}

const std::vector<Operator>& OperatorContainer::binaryOperators() const {
	return mBinaryOperatorChars;
}

const std::map<Operator, int>& OperatorContainer::binaryOperatorPrecedences() const {
	return mBinaryOperators;
}

const std::unordered_set<char>& OperatorContainer::assignmentOperators() const {
	return mAssignmentOperators;
}

const std::set<Operator>& OperatorContainer::unaryOperators() const {
	return mUnaryOperators;
}

const std::map<Operator, std::shared_ptr<Type>>& OperatorContainer::binaryOpReturnTypes() const { 
	return mBinaryOpReturnTypes;
}

const std::unordered_set<char> & OperatorContainer::twoCharOpChars() const {
	return mTwoCharOpChars;
}

int OperatorContainer::getPrecedence(Operator op) const {
	if (mBinaryOperators.count(op) > 0) {
		return mBinaryOperators.at(op); 
	}

	return -1;
}

bool OperatorContainer::isBinaryDefined(Operator op) const {
	return mBinaryOperators.count(op) > 0;
}

bool OperatorContainer::isUnaryDefined(Operator op) const {
	return mUnaryOperators.count(op) > 0;
}