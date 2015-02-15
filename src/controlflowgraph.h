#pragma once
#include <memory>

class FunctionAST;

//Represents a control flow graph
class ControlFlowGraph {
private:

public:
	//Creates a graph from the given function
	static void createGraph(const FunctionAST* function);
};