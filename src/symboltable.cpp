#include "symboltable.h"
#include "ast.h"
#include "symbol.h"

SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> outer, std::string name)
	: mName(name), mOuter(outer) {

}

std::string SymbolTable::name() const {
	return mName;
}

bool SymbolTable::add(std::string name, std::shared_ptr<Symbol> symbol) {
	if (mInner.count(name) == 0) {
		mInner.insert({ name, symbol });
		return true;
	}

	return false;
}

void SymbolTable::add(const SymbolTable& symbolTable) {
	for (auto symbol : symbolTable.inner()) {
		add(symbol.first, symbol.second);
	} 
}

bool SymbolTable::addFunction(std::string name, std::vector<VariableSymbol> parameters, std::string returnType) {
	auto signature = std::make_shared<FunctionSignatureSymbol>(name, parameters, returnType);
	
	if (mInner.count(name) > 0) {
		auto func = std::dynamic_pointer_cast<FunctionSymbol>(mInner.at(name));

		if (func != nullptr) {
			return func->addOverload(signature);
		} else {
			return false;
		}
	} else {
		auto func = std::make_shared<FunctionSymbol>(name, signature);
		mInner.insert({ name, func });
		return true;
	}
}

void SymbolTable::newFunction(std::string name, const std::vector<std::pair<std::string, std::string>>& parameters, std::string returnType) {
	std::vector<VariableSymbol> parameterSymbols;

	for (auto param : parameters) {
		parameterSymbols.push_back(VariableSymbol(param.second, param.first, true));
	}

	addFunction(name, parameterSymbols, returnType);
}

std::shared_ptr<Symbol> SymbolTable::find(std::string name) const {
	//First check in the inner
	if (mInner.count(name) > 0) {
		return mInner.at(name);
	}

	//Else in the outer
	if (mOuter != nullptr) {
		return mOuter->find(name);
	} else {
		return nullptr;
	}
}

void SymbolTable::set(std::string name, std::shared_ptr<Symbol> symbol) {
	if (mInner.count(name) > 0) {
		mInner[name] = symbol;
	}
}

const std::map<std::string, std::shared_ptr<Symbol>>& SymbolTable::inner() const {
	return mInner;
}

std::shared_ptr<SymbolTable> SymbolTable::outer() const {
	return mOuter;
}

std::shared_ptr<SymbolTable> SymbolTable::newInner(std::shared_ptr<SymbolTable> outer) {
	return std::make_shared<SymbolTable>(outer);
}

void printSymbolTable(std::ostream& os, const SymbolTable& symbolTable, int indentLevel = 0) {
	for (auto symbol : symbolTable.inner()) {
		for (int i = 0; i < indentLevel; i++) {
			os << "\t";
		}

		os << symbol.first << ": " << symbol.second->type() << std::endl;
	}
}

std::ostream& operator<<(std::ostream& os, const SymbolTable& symbolTable) {
	printSymbolTable(os, symbolTable);
	return os;
}