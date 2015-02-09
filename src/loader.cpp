#include "loader.h"
#include "typechecker.h"
#include "binder.h"
#include "symboltable.h"
#include "symbol.h"
#include "type.h"
#include "helpers.h"

#include <stdexcept>

FunctionDefinition::FunctionDefinition(std::string name, std::vector<std::shared_ptr<Type>> parameters, std::shared_ptr<Type> returnType)
	: name(name), parameters(parameters), returnType(returnType) {

}

Loader::Loader(Binder& binder, TypeChecker& typeChecker)
	: mBinder(binder), mTypeChecker(typeChecker) {

}

std::vector<std::string> Loader::tokenize(std::istream& stream) {
    std::vector<std::string> tokens;
    std::string token;
    bool isComment = false;

    char c;
    while (stream.get(c)) {
        bool newToken = false;
        bool newIdentifier = false;

        if (!isComment && c == '#') {
            isComment = true;
            continue;
        }

        if (isComment && c == '\n') {
            isComment = false;
            continue;
        }

        if (!isComment) {
            if (isspace(c)) {
                newToken = true;
            }

            if (c == '(' || c == ')') {
                newToken = true;
                newIdentifier = true;
            }

            if (newToken) {
                if (token != "") {
                    tokens.push_back(token);
                }

                token = "";
            } else {
                token += c;
            }

            if (newIdentifier) {
                tokens.push_back(std::string{ c });
            }
        }
    }

    if (token != "") {
        tokens.push_back(token);
    }
   
    return tokens;
}

std::string nextToken(const std::vector<std::string>& tokens, int& index) {
    int left = tokens.size() - (index + 1);

    if (left > 0) {
        return tokens[++index];
    } else {
        throw std::runtime_error("Reached end of tokens.");
    }
}

FunctionDefinition Loader::parseFunctionDef(const std::vector<std::string>& tokens, int& tokenIndex) {
    auto name = nextToken(tokens, tokenIndex);

    if (nextToken(tokens, tokenIndex) != "(") {
        throw std::runtime_error("Expected '(' after function name.");
    } 

    std::vector<std::shared_ptr<Type>> parameters;

    while (true) {
        std::string param = nextToken(tokens, tokenIndex);

        if (param == ")") {
            break;
        }

        auto paramType = mTypeChecker.getType(param);

        if (paramType == nullptr) {
            throw std::runtime_error("'" + param + "' is not a type.");
        }

        parameters.push_back(paramType);
    }

    auto returnTypeName = nextToken(tokens, tokenIndex);
    auto returnType = mTypeChecker.getType(returnTypeName);

    if (returnType == nullptr) {
        throw std::runtime_error("'" + returnTypeName + "' is not a type.");
    }

    return FunctionDefinition(name, parameters, returnType);
}

std::vector<std::string> splitFuncName(std::string name) {
	std::vector<std::string> parts;

	std::string delimiter = ".";

	std::size_t pos = 0;
	std::string token;
	while ((pos = name.find(delimiter)) != std::string::npos) {
	    token = name.substr(0, pos);
	    parts.push_back(token);
	    name.erase(0, pos + delimiter.length());
	}

	parts.push_back(name);

	return parts;
}

//Returns the symbol table for the given namespace
std::shared_ptr<SymbolTable> getNamespaceTable(std::shared_ptr<SymbolTable> outerTable, std::vector<std::string> namespaces) {
	if (namespaces.size() == 0) {
		return outerTable;
	}

	std::string currentNamespace = namespaces.at(0);
	namespaces.erase(namespaces.begin());

	auto symbol = outerTable->find(currentNamespace);

	if (symbol != nullptr) {
		auto namespaceSymbol = std::dynamic_pointer_cast<NamespaceSymbol>(symbol);

		if (namespaceSymbol == nullptr) {
			throw std::runtime_error("'" + currentNamespace + "' symbol is already defined.");
		}

		return getNamespaceTable(namespaceSymbol->symbolTable(), namespaces);
	} else {
		auto namespaceTable = std::make_shared<SymbolTable>(outerTable);
		auto namespaceSymbol = std::make_shared<NamespaceSymbol>(currentNamespace, namespaceTable);
		outerTable->add(currentNamespace, namespaceSymbol);
		return getNamespaceTable(namespaceTable, namespaces);
	}
}

void Loader::defineFunction(const FunctionDefinition& funcDef) {
	std::vector<VariableSymbol> parameterSymbols;

	int i = 0;
	for (auto param : funcDef.parameters) {
		parameterSymbols.push_back(VariableSymbol("param_" + std::to_string(i), param->name(), true));
		i++;
	}

	auto splittedFuncName = splitFuncName(funcDef.name);
	auto funcScope = mBinder.symbolTable();
	auto funcName = splittedFuncName.at(splittedFuncName.size() - 1);
	splittedFuncName.erase(splittedFuncName.end() - 1);

	if (splittedFuncName.size() > 0) {
		funcScope = getNamespaceTable(funcScope, splittedFuncName);
	}

	if (!funcScope->addFunction(funcName, parameterSymbols, funcDef.returnType->name())) {
        throw std::runtime_error("The function '" + funcName + "' is already defined.");
    }
}

void Loader::loadAssembly(std::istream& stream) {
	auto tokens = tokenize(stream);

	for (int i = 0; i < tokens.size(); i++) {
        std::string current = tokens[i];

        //Parse the function definition
        if (current == "func") {
           	auto funcDef = parseFunctionDef(tokens, i);
           	defineFunction(funcDef);
        }

        //Parse external function
        if (current == "extern") {
            auto funcName = nextToken(tokens, i);

            if (nextToken(tokens, i) != "::") {
                throw std::runtime_error("Expected '::' after extern function name.");
            } 

            auto externFuncDef = parseFunctionDef(tokens, i);
            defineFunction(FunctionDefinition(funcName, externFuncDef.parameters, externFuncDef.returnType));
        }
    }
}