#include "controlflowgraph.h"
#include "ast/ast.h"
#include "ast/functionast.h"
#include "ast/statementast.h"
#include "ast/blockast.h"
#include <iostream>
#include <vector>

struct BasicBlock {
	std::vector<const AbstractSyntaxTree*> asts;
};

void ControlFlowGraph::createGraph(const FunctionAST* function) {
	std::vector<BasicBlock> basicBlocks;
	BasicBlock currentBlock;

	for (auto block : basicBlocks) {
		std::cout << "---Start block---" << std::endl;

		for (auto ast : block.asts) {
			std::cout << *ast << std::endl;
		}

		std::cout << "---End block---" << std::endl;
	}
}