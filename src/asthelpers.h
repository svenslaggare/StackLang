#pragma once
#include <memory>

class FunctionPrototypeAST;
class AbstractSyntaxTree;

namespace ASTHelpers {	
	//Tries to convert the given ast into a function prototype
	std::shared_ptr<FunctionPrototypeAST> asPrototype(std::shared_ptr<AbstractSyntaxTree> ast);

	//Searches for the first AST that satisfies the predicate. Result only binded if true.
	bool findAST(std::shared_ptr<AbstractSyntaxTree> ast, std::function<bool (std::shared_ptr<AbstractSyntaxTree> ast)> predicate, std::shared_ptr<AbstractSyntaxTree>& result);
}