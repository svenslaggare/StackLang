#include "namespace.h"
#include "helpers.h"

Namespace::Namespace(std::vector<std::string> parts)
	: mParts(parts), mName(getName("::")), mVMName(getName(".")) {
	
}

std::string Namespace::getName(std::string sep) {
	return Helpers::join<std::string>(
		mParts,
		[](std::string part) { return part; },
		sep);
}

const std::vector<std::string>& Namespace::parts() const {
	return mParts;
}

std::string Namespace::name() const {
	return mName;
}

std::string Namespace::vmName() const {
	return mVMName;
}