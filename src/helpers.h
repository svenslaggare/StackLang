#pragma once
#include <functional>
#include <vector>
#include <memory>
#include "symboltable.h"

class Symbol;
class SymbolTable;

//Contain helper methods
namespace Helpers {
	//Joins the given values using the given seperator
	template <class T>
	std::string join(const std::vector<T>& values, std::function<std::string(T)> toString, std::string sep) {
		bool isFirst = true;
		std::string str = "";

		for (auto val : values) {
			if (!isFirst) {
				str += sep;
			} else {
				isFirst = false;
			}

			str += toString(val);
		}

		return str;
	}

	//Splits the given string by the given delimiter
	std::vector<std::string> splitString(std::string str, std::string delimiter);

	//Replaces all the occurrences in the given string
	std::string replaceString(std::string str, std::string search, std::string replace);

	//Finds a symbol defined a namespace
	std::shared_ptr<Symbol> findSymbolInNamespace(std::shared_ptr<SymbolTable> symbolTable, std::string name);
};