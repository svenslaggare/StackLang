#include "ast.h"
#include <stdexcept>

std::ostream& operator<<(std::ostream& os, const AbstractSyntaxTree& ast) {
	os << ast.asString();
	return os;
}