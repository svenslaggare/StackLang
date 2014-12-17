#include "binder.h"
#include "programast.h"
#include <stdexcept>
#include <vector>
#include "expressionast.h"
#include "functionast.h"

Binder::Binder(std::shared_ptr<Program> program)
	: mProgram(program), mSymbolTable(std::make_shared<SymbolTable>(SymbolTable())) {

}

void Binder::addFunction(std::string name, const std::vector<std::pair<std::string, std::string>>& parameters, std::string returnType) {
	std::vector<std::shared_ptr<VariableDeclerationExpressionAST>> parameterDecls;

	for (auto param : parameters) {
		parameterDecls.push_back(std::make_shared<VariableDeclerationExpressionAST>(VariableDeclerationExpressionAST(param.first, param.second)));
	}

	mSymbolTable->add(name, std::make_shared<FunctionPrototypeAST>(FunctionPrototypeAST(name, parameterDecls, returnType)));
}

void Binder::generateSymbolTable(std::shared_ptr<ProgramAST> programAST) {
	programAST->generateSymbols(*this, mSymbolTable);
}

std::shared_ptr<Program> Binder::program() const {
	return mProgram;
}

std::shared_ptr<SymbolTable> Binder::symbolTable() const {
	return mSymbolTable;
}

void Binder::error(std::string message) {
	throw std::runtime_error(message);
}