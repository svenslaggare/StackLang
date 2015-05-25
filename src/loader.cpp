#include "loader.h"
#include "typechecker.h"
#include "binder.h"
#include "symboltable.h"
#include "symbol.h"
#include "type.h"
#include "helpers.h"
#include "assemblyparser.h"
#include <stdexcept>

Loader::Loader(Binder& binder, TypeChecker& typeChecker)
	: mBinder(binder), mTypeChecker(typeChecker) {

}

std::shared_ptr<Type> Loader::getType(std::string vmTypeName) {
    auto type = mTypeChecker.getType(TypeSystem::fromVMType(vmTypeName));

    if (type == nullptr) {
        throw std::runtime_error("'" + vmTypeName + "' is not a type.");
    }

    return type;
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
		auto namespaceTable = std::make_shared<SymbolTable>(outerTable, currentNamespace);
		auto namespaceSymbol = std::make_shared<NamespaceSymbol>(currentNamespace, namespaceTable);
		outerTable->add(currentNamespace, namespaceSymbol);
		return getNamespaceTable(namespaceTable, namespaces);
	}
}

//Parses the given access modifier
AccessModifiers parseAccessModifier(std::string modifier) {
	if (modifier == "public") {
		return AccessModifiers::Public;
	} else if (modifier == "private") {
		return AccessModifiers::Private;
	} else {
		throw std::runtime_error("'" + modifier + "' is not a valid access modifier.");
	}
}

void Loader::defineFunction(const AssemblyParser::Function& funcDef, std::shared_ptr<SymbolTable> funcScope) {
	std::vector<VariableSymbol> parameterSymbols;

	int i = 0;
	for (auto param : funcDef.parameters) {
		auto paramType = getType(param);

		if (paramType == nullptr) {
			throw std::runtime_error("'" + param + "' is not a type.");
		}

		parameterSymbols.push_back(VariableSymbol(
            "param_" + std::to_string(i), paramType->name(),
            VariableSymbolAttribute::FUNCTION_PARAMETER));
        
		i++;
	}

    auto funcName = funcDef.name;

	if (funcScope == nullptr) {
        auto splittedFuncName = splitFuncName(funcDef.name);
        funcScope = mBinder.symbolTable();
        funcName = splittedFuncName.at(splittedFuncName.size() - 1);
        splittedFuncName.erase(splittedFuncName.end() - 1);

        if (splittedFuncName.size() > 0) {
            funcScope = getNamespaceTable(funcScope, splittedFuncName);
        }
    }

	auto returnType = getType(funcDef.returnType);
	if (returnType == nullptr) {
		throw std::runtime_error("'" + funcDef.returnType + "' is not a type.");
	}

	if (!funcScope->addFunction(funcName, parameterSymbols, returnType->name())) {
        throw std::runtime_error("The function '" + funcName + "' is already defined.");
    }
}

void Loader::defineClass(const AssemblyParser::Struct& classDef) {
    if (mTypeChecker.findType(classDef.name) == nullptr) {
        auto classType = std::make_shared<ClassType>(classDef.name);
        mTypeChecker.addType(classType);

        std::unordered_map<std::string, Field> fields;

		auto attributes = classDef.attributes.attributes;

        for (auto field : classDef.fields) {
			auto fieldType = getType(field.type);
			auto accessModifier = AccessModifiers::Public;

			if (fieldType == nullptr) {
				throw std::runtime_error("'" + field.type + "' is not a type.");
			}

			//Check if any access modifiers are defined in the class
			if (attributes.count("FieldAccessModifiers") > 0) {
				auto fieldModifiers = attributes["FieldAccessModifiers"].values;

				if (fieldModifiers.count(field.name) > 0) {
					accessModifier = parseAccessModifier(fieldModifiers[field.name]);
				}
			}

            fields.insert({ field.name, Field(field.name, fieldType, accessModifier) });
        }

        mTypeChecker.addObject(Object(classDef.name, classType, fields));
        auto symbolTable = mBinder.symbolTable();

        if (symbolTable->find(classDef.name) == nullptr) {
            symbolTable->addClass(classDef.name, std::make_shared<SymbolTable>(symbolTable));
        } else {
            mBinder.error("The symbol '" + classDef.name + "' is already defined.");
        }
    } else {
        mTypeChecker.typeError("The class '" + classDef.name + "' is already defined.");
    }
}

void Loader::defineMemberFunction(const AssemblyParser::Function& memberDef) {
	auto className = memberDef.structName;
	auto memberName = memberDef.memberFunctionName;

	auto classSymbol = std::dynamic_pointer_cast<ClassSymbol>(mBinder.symbolTable()->find(className));

	if (classSymbol == nullptr) {
		throw std::runtime_error("There exists no class named '" + className + "'.");
	}

	if (memberName == ".constructor" && memberDef.returnType != PrimitiveType(PrimitiveTypes::Void).name()) {
		throw std::runtime_error("Constructors must have return type 'Void");
	}

	auto attributes = memberDef.attributes.attributes;
	auto accessModifier = AccessModifiers::Public;

	//Check if an access modifier are defined in function
	if (attributes.count("AccessModifier") > 0) {
		auto accessModifierAttr = attributes["AccessModifier"].values;

		if (accessModifierAttr.count("value") > 0) {
			accessModifier = parseAccessModifier(accessModifierAttr["value"]);
		}
	}

	std::vector<VariableSymbol> parameterSymbols;

	for (std::size_t i = 1; i < memberDef.parameters.size(); ++i) {
		auto param = memberDef.parameters[i];
		auto paramType = getType(param);

		if (paramType == nullptr) {
			throw std::runtime_error("'" + param + "' is not a type.");
		}

		parameterSymbols.push_back(VariableSymbol(
			"param_" + std::to_string(i), paramType->name(),
			VariableSymbolAttribute::FUNCTION_PARAMETER));
	}

	auto returnType = getType(memberDef.returnType);
	if (returnType == nullptr) {
		throw std::runtime_error("'" + memberDef.returnType + "' is not a type.");
	}

	auto added = classSymbol->symbolTable()->addMemberFunction(
		memberDef.memberFunctionName,
		parameterSymbols,
		returnType->name(),
		accessModifier);

	if (!added) {
		throw std::runtime_error("The function '" + memberDef.memberFunctionName + "' is already defined.");
	}
}

void Loader::loadAssembly(std::istream& stream) {
	//Parse
	AssemblyParser::Assembly assembly;
	auto tokens = AssemblyParser::tokenize(stream);
	AssemblyParser::parseTokens(tokens, assembly);

	//Load
	for (auto& currentStruct : assembly.structs) {
		defineClass(currentStruct);
	}

	for (auto& currentFunc : assembly.functions) {
		if (currentFunc.isMemberFunction) {
			defineMemberFunction(currentFunc);
		} else {
			defineFunction(currentFunc);
		}
	}
}