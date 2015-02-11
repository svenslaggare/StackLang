#pragma once
#include <memory>

class FunctionPrototypeAST;
class AbstractSyntaxTree;

//Contains helper functions for AST's
namespace ASTHelpers {	
	//Tries to convert the given ast into a function prototype
	std::shared_ptr<FunctionPrototypeAST> asPrototype(std::shared_ptr<AbstractSyntaxTree> ast);
}