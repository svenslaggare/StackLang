#include "helpers.h"
#include "symbol.h"
#include "symboltable.h"

std::vector<std::string> Helpers::splitString(std::string str, std::string delimiter) {
	std::vector<std::string> parts;

	std::size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos) {
	    token = str.substr(0, pos);
	    parts.push_back(token);
	    str.erase(0, pos + delimiter.length());
	}

	parts.push_back(str);

	return parts;
}

std::string Helpers::replaceString(std::string str, std::string search, std::string replace) {
	std::size_t index = 0;
	while (true) {
	     index = str.find(search, index);
	     if (index == std::string::npos) {
	     	break;
	     }

	     str.replace(index, search.size(), replace);
	}

	return str;
}

//Finds the symbol for the given type
std::shared_ptr<Symbol> Helpers::findSymbolInNamespace(std::shared_ptr<SymbolTable> symbolTable, std::string typeName) {
	if (typeName.find("::") != std::string::npos) {
		//Split the function name
		std::vector<std::string> parts = Helpers::splitString(typeName, "::");

		//Find the namespace
		std::shared_ptr<SymbolTable> namespaceTable = symbolTable;
		for (std::size_t i = 0; i < parts.size() - 1; i++) {
			auto part = parts[i];
			auto innerTable = std::dynamic_pointer_cast<NamespaceSymbol>(namespaceTable->find(part));

			if (innerTable == nullptr) {
				return nullptr;
			}

			namespaceTable = innerTable->symbolTable();
		}

		return namespaceTable->find(parts[parts.size() - 1]);		
	} else {
		return symbolTable->find(typeName);
	}
}