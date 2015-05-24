#include "assemblyparser.h"
#include <cctype>
#include <string>
#include <iostream>
#include <unordered_map>

AssemblyParser::Instruction::Instruction() {

}

AssemblyParser::Instruction AssemblyParser::Instruction::make(AssemblyParser::OpCodes opCode) {
	AssemblyParser::Instruction inst;
	inst.opCode = opCode;
	return inst;
}

AssemblyParser::Instruction AssemblyParser::Instruction::makeWithInt(AssemblyParser::OpCodes opCode, int value) {
	AssemblyParser::Instruction inst;
	inst.intValue = value;
	inst.opCode = opCode;
	return inst;
}

AssemblyParser::Instruction AssemblyParser::Instruction::makeWithFloat(AssemblyParser::OpCodes opCode, float value) {
	AssemblyParser::Instruction inst;
	inst.floatValue = value;
	inst.opCode = opCode;
	return inst;
}

AssemblyParser::Instruction AssemblyParser::Instruction::makeWithChar(AssemblyParser::OpCodes opCode, char value) {
	AssemblyParser::Instruction inst;
	inst.charValue = value;
	inst.opCode = opCode;
	return inst;
}

AssemblyParser::Instruction AssemblyParser::Instruction::makeWithStr(AssemblyParser::OpCodes opCode, std::string value) {
	AssemblyParser::Instruction inst;
	inst.strValue = value;
	inst.opCode = opCode;
	return inst;
}

AssemblyParser::Instruction AssemblyParser::Instruction::makeCall(std::string funcName, std::vector<std::string> parameters) {
	AssemblyParser::Instruction inst;
	inst.strValue = funcName;
	inst.parameters = parameters;
	inst.opCode = AssemblyParser::OpCodes::CALL;
	return inst;
}

AssemblyParser::Instruction AssemblyParser::Instruction::makeCallInstance(std::string structType, std::string funcName, std::vector<std::string> parameters) {
	AssemblyParser::Instruction inst;
	inst.calledStructType = structType;
	inst.strValue = funcName;
	inst.parameters = parameters;
	inst.opCode = AssemblyParser::OpCodes::CALL_INSTANCE;
	return inst;
}

AssemblyParser::Instruction AssemblyParser::Instruction::makeNewObject(std::string structType, std::vector<std::string> parameters) {
	AssemblyParser::Instruction inst;
	inst.calledStructType = structType;
	inst.strValue = ".constructor";
	inst.parameters = parameters;
	inst.opCode = AssemblyParser::OpCodes::NEW_OBJECT;
	return inst;
}

AssemblyParser::Function::Function()
	: isMemberFunction(false), isExternal(false) {

}

void AssemblyParser::Function::setNumLocals(int num) {
	localTypes.resize(num);
}

std::size_t AssemblyParser::Function::numLocals() const {
	return localTypes.size();
}

AssemblyParser::Struct::Struct() {

}

namespace {
	std::string toLower(std::string str) {
		std::string newStr { "" };
		int length = str.length();

		for (int i = 0; i < length; i++) {
			newStr += std::tolower(str[i]);
		}

		return newStr;
	}

	void assertTokenCount(const std::vector<std::string>& tokens, int index, int count) {
		int left = tokens.size() - (index + 1);

		if (left < count) {
			throw std::runtime_error("Expected '" + std::to_string(count) + "' tokens.");
		}
	}

	std::unordered_map<std::string, AssemblyParser::OpCodes> noOperandsInstructions
		{
			{ "nop", AssemblyParser::OpCodes::NOP },
			{ "pop", AssemblyParser::OpCodes::POP },
			{ "add", AssemblyParser::OpCodes::ADD },
			{ "sub", AssemblyParser::OpCodes::SUB },
			{ "mul", AssemblyParser::OpCodes::MUL },
			{ "div", AssemblyParser::OpCodes::DIV },
			{ "pushtrue", AssemblyParser::OpCodes::PUSH_TRUE },
			{ "pushfalse", AssemblyParser::OpCodes::PUSH_FALSE },
			{ "and", AssemblyParser::OpCodes::AND },
			{ "or", AssemblyParser::OpCodes::OR },
			{ "not", AssemblyParser::OpCodes::NOT },
			{ "convinttofloat", AssemblyParser::OpCodes::CONVERT_INT_TO_FLOAT },
			{ "convfloattoint", AssemblyParser::OpCodes::CONVERT_FLOAT_TO_INT },
			{ "cmpeq", AssemblyParser::OpCodes::COMPARE_EQUAL },
			{ "cmpne", AssemblyParser::OpCodes::COMPARE_NOT_EQUAL },
			{ "cmpgt", AssemblyParser::OpCodes::COMPARE_GREATER_THAN },
			{ "cmpge", AssemblyParser::OpCodes::COMPARE_GREATER_THAN_OR_EQUAL },
			{ "cmplt", AssemblyParser::OpCodes::COMPARE_LESS_THAN },
			{ "cmple", AssemblyParser::OpCodes::COMPARE_LESS_THAN_OR_EQUAL },
			{ "pushnull", AssemblyParser::OpCodes::PUSH_NULL },
			{ "ldlen", AssemblyParser::OpCodes::LOAD_ARRAY_LENGTH },
			{ "ret", AssemblyParser::OpCodes::RET },
			{ "gc", AssemblyParser::OpCodes::GARBAGE_COLLECT },
		};

	std::unordered_map<std::string, AssemblyParser::OpCodes> branchInstructions
		{
			{ "beq", AssemblyParser::OpCodes::BRANCH_EQUAL },
			{ "bne", AssemblyParser::OpCodes::BRANCH_NOT_EQUAL },
			{ "bgt", AssemblyParser::OpCodes::BRANCH_GREATER_THAN },
			{ "bge", AssemblyParser::OpCodes::BRANCH_GREATER_THAN_OR_EQUAL },
			{ "blt", AssemblyParser::OpCodes::BRANCH_LESS_THAN },
			{ "ble", AssemblyParser::OpCodes::BRANCH_LESS_THAN_OR_EQUAL }
		};

	std::unordered_map<std::string, AssemblyParser::OpCodes> strOperandInstructions
		{
			{ "newarr", AssemblyParser::OpCodes::NEW_ARRAY },
			{ "stelem", AssemblyParser::OpCodes::STORE_ELEMENT },
			{ "ldelem", AssemblyParser::OpCodes::LOAD_ELEMENT },
			{ "stfield", AssemblyParser::OpCodes::STORE_FIELD },
			{ "ldfield", AssemblyParser::OpCodes::LOAD_FIELD }
		};

	std::string nextToken(const std::vector<std::string>& tokens, std::size_t& index) {
		int left = tokens.size() - (index + 1);

		if (left > 0) {
			return tokens[++index];
		} else {
			throw std::runtime_error("Reached end of tokens.");
		}
	}

	void parseFunctionDef(const std::vector<std::string>& tokens, std::size_t& tokenIndex, AssemblyParser::Function& func) {
		func.name = nextToken(tokens, tokenIndex);

		if (nextToken(tokens, tokenIndex) != "(") {
			throw std::runtime_error("Expected '(' after function name.");
		}

		while (true) {
			std::string param = nextToken(tokens, tokenIndex);

			if (param == ")") {
				break;
			}

			func.parameters.push_back(param);
		}

		func.returnType = nextToken(tokens, tokenIndex);
	}

	void readCallParameters(const std::vector<std::string>& tokens, std::size_t& tokenIndex, std::vector<std::string>& parameters) {
		while (true) {
			auto param = nextToken(tokens, tokenIndex);

			if (param == ")") {
				break;
			}

			parameters.push_back(param);
		}
	}

	void parseAttribute(const std::vector<std::string>& tokens, std::size_t& tokenIndex, AssemblyParser::AttributeContainer& container) {
		auto attributeName = nextToken(tokens, tokenIndex);

		if (nextToken(tokens, tokenIndex) != "(") {
			throw std::runtime_error("Expected '(' after attribute name");
		}

		if (container.attributes.count(attributeName) > 0) {
			throw std::runtime_error("The attribute '" + attributeName + "' is already defined.'");
		}

		AssemblyParser::Attribute attribute;
		attribute.name = attributeName;

		while (true) {
			auto key = nextToken(tokens, tokenIndex);

			if (key == ")") {
				break;
			}

			if (nextToken(tokens, tokenIndex) != "=") {
				throw std::runtime_error("Expected '=' after name in attribute.");
			}

			auto value = nextToken(tokens, tokenIndex);

			if (attribute.values.count(key) == 0) {
				attribute.values.insert({ key, value });
			} else {
				throw std::runtime_error(
					"The key '" + key + "' is already defined in the attribute '" + attributeName + "'.");
			}
		}

		container.attributes.insert({ attributeName, attribute });
	}
}

std::vector<std::string> AssemblyParser::tokenize(std::istream& stream) {
	std::vector<std::string> tokens;
	std::string token;
	bool isComment = false;
	bool isString = false;
	bool escape = false;

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
			if (!escape) {
				if (c == '\\') {
					escape = true;
					continue;
				}

				if (c == '"') {
					if (isString) {
						tokens.push_back(token);
						token = "";
					}

					isString = !isString;
					continue;
				}
			}

			if (isspace(c) && !isString) {
				newToken = true;
			}

			if (c == '(' || c == ')') {
				newToken = true;
				newIdentifier = true;
			}

			if (c == '@') {
				newToken = true;
				newIdentifier = true;
			}

			if (c == '=') {
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
				tokens.push_back(std::string { c });
			}
		}

		if (escape) {
			escape = false;
		}
	}

	if (token != "") {
		tokens.push_back(token);
	}

	return tokens;
}

void AssemblyParser::parseTokens(const std::vector<std::string>& tokens, AssemblyParser::Assembly& assembly) {
	bool isFunc = false;
	bool isFuncBody = false;
	bool localsSet = false;

	AssemblyParser::Function currentFunc;

	bool isStruct = false;
	bool isStructBody = false;

	AssemblyParser::Struct currentStruct;

	for (std::size_t i = 0; i < tokens.size(); i++) {
		std::string current = tokens[i];
		std::string currentToLower = toLower(current);

		//Bodies
		if (isFuncBody) {
			if (currentToLower == "@") {
				parseAttribute(tokens, i, currentFunc.attributes);
				continue;
			}

			if (currentToLower == "pushint") {
				int value = stoi(nextToken(tokens, i));
				currentFunc.instructions.push_back(Instruction::makeWithInt(AssemblyParser::OpCodes::PUSH_INT, value));
				continue;
			}

			if (currentToLower == "pushfloat") {
				float value = stof(nextToken(tokens, i));
				currentFunc.instructions.push_back(Instruction::makeWithFloat(AssemblyParser::OpCodes::PUSH_FLOAT, value));
				continue;
			}

			if (currentToLower == "pushchar") {
				char value = (char)stoi(nextToken(tokens, i));
				currentFunc.instructions.push_back(Instruction::makeWithChar(AssemblyParser::OpCodes::PUSH_CHAR, value));
				continue;
			}

			if (noOperandsInstructions.count(currentToLower) > 0) {
				currentFunc.instructions.push_back(Instruction::make(noOperandsInstructions[currentToLower]));
			}

			if (strOperandInstructions.count(currentToLower) > 0) {
				std::string value = nextToken(tokens, i);
				currentFunc.instructions.push_back(Instruction::makeWithStr(strOperandInstructions[currentToLower], value));
				continue;
			}

			if (currentToLower == ".locals") {
				if (!localsSet) {
					int localsCount = stoi(nextToken(tokens, i));

					if (localsCount >= 0) {
						localsSet = true;
						currentFunc.setNumLocals(localsCount);
					} else {
						throw std::runtime_error("The number of locals must be >= 0.");
					}

					continue;
				} else {
					throw std::runtime_error("The locals has already been set.");
				}
			}

			if (currentToLower == ".local") {
				if (localsSet) {
					int localIndex = stoi(nextToken(tokens, i));
					auto localType = nextToken(tokens, i);

					if (localIndex >= 0 && localIndex < (int)currentFunc.numLocals()) {
						currentFunc.localTypes.at(localIndex) = localType;
					} else {
						throw std::runtime_error("Invalid local index.");
					}

					continue;
				} else {
					throw std::runtime_error("The locals must been set.");
				}
			}

			if (currentToLower == "ldloc" || currentToLower == "stloc") {
				if (!localsSet) {
					throw std::runtime_error("The locals must be set.");
				}

				int local = stoi(nextToken(tokens, i));
				auto opCode = currentToLower == "ldloc" ? AssemblyParser::OpCodes::LOAD_LOCAL : AssemblyParser::OpCodes::STORE_LOCAL;

				if (local >= 0 && local < (int)currentFunc.numLocals()) {
					currentFunc.instructions.push_back(Instruction::makeWithInt(opCode, local));
					continue;
				} else {
					throw std::runtime_error("The local index is out of range.");
				}
			}

			if (currentToLower == "call" || currentToLower == "callinst") {
				bool isInstance = currentToLower == "callinst";

				std::string funcName = nextToken(tokens, i);
				std::string structType = "";

				if (isInstance) {
					auto structNamePos = funcName.find("::");

					if (structNamePos != std::string::npos) {
						structType = funcName.substr(0, structNamePos);
					} else {
						throw std::runtime_error("Expected '::' in called member function.");
					}

					funcName = funcName.substr(structNamePos + 2);
				}

				if (nextToken(tokens, i) != "(") {
					throw std::runtime_error("Expected '(' after called function.");
				}

				std::vector<std::string> parameters;
				readCallParameters(tokens, i, parameters);

				if (isInstance) {
					currentFunc.instructions.push_back(Instruction::makeCallInstance(structType, funcName, parameters));
				} else {
					currentFunc.instructions.push_back(Instruction::makeCall(funcName, parameters));
				}

				continue;
			}

			if (currentToLower == "ldarg") {
				int argNum = stoi(nextToken(tokens, i));

				if (argNum >= 0 && argNum < (int)currentFunc.parameters.size()) {
					currentFunc.instructions.push_back(Instruction::makeWithInt(AssemblyParser::OpCodes::LOAD_ARG, argNum));
				} else {
					throw std::runtime_error("The argument index is out of range.");
				}
				continue;
			}

			if (currentToLower == "newobj") {
				std::string funcName = nextToken(tokens, i);
				std::string structType = "";

				auto structNamePos = funcName.find("::");

				if (structNamePos != std::string::npos) {
					structType = funcName.substr(0, structNamePos);
				} else {
					throw std::runtime_error("Expected '::' after the type in a new object instruction.");
				}

				funcName = funcName.substr(structNamePos + 2);

				if (funcName != ".constructor") {
					throw std::runtime_error("Expected call to constructor.");
				}

				if (nextToken(tokens, i) != "(") {
					throw std::runtime_error("Expected '(' after called function.");
				}

				std::vector<std::string> parameters;
				readCallParameters(tokens, i, parameters);
				currentFunc.instructions.push_back(Instruction::makeNewObject(structType, parameters));
				continue;
			}

			if (currentToLower == "br") {
				int target = stoi(nextToken(tokens, i));
				currentFunc.instructions.push_back(Instruction::makeWithInt(AssemblyParser::OpCodes::BRANCH, target));
				continue;
			}

			if (branchInstructions.count(currentToLower) > 0) {
				int target = stoi(nextToken(tokens, i));
				currentFunc.instructions.push_back(Instruction::makeWithInt(branchInstructions[currentToLower], target));
				continue;
			}

			if (currentToLower == "ldstr") {
				auto str = nextToken(tokens, i);
				currentFunc.instructions.push_back(Instruction::makeWithStr(AssemblyParser::OpCodes::LOAD_STRING, str));
			}
		}

		if (isStructBody) {
			if (currentToLower == "@") {
				parseAttribute(tokens, i, currentStruct.attributes);
				continue;
			}

			if (currentToLower == "}") {
				assembly.structs.push_back(currentStruct);
				isStruct = false;
				isStructBody = false;
				continue;
			}

			Field field;
			field.name = current;
			field.type = nextToken(tokens, i);

			currentStruct.fields.push_back(field);
		}

		//Definitions
		bool isFuncDef = false;
		bool isExternFunc = false;
		bool isMemberDef = false;

		if (!isFunc && !isStruct && !isExternFunc) {
			if (currentToLower == "func") {
				isFuncDef = true;
				isFunc = true;
			} else if (currentToLower == "struct") {
				currentStruct = {};
				currentStruct.name = nextToken(tokens, i);
				isStruct = true;;
			} else if (currentToLower == "extern") {
				isExternFunc = true;
			} else if (currentToLower == "member") {
				isMemberDef = true;
			} else {
				throw std::runtime_error("Invalid identifier '" + current + "'");
			}
		}

		if (isFunc && !isFuncDef && currentToLower == "{") {
			isFuncBody = true;
		}

		if (isStruct && currentToLower == "{") {
			isStructBody = true;
		}

		//Parse the function definition
		if (isFuncDef) {
			currentFunc = {};
			parseFunctionDef(tokens, i, currentFunc);

			if (currentFunc.name.find("::") != std::string::npos) {
				throw std::runtime_error("'::' is only allowed in member functions.");
			}

			int numArgs = currentFunc.parameters.size();

			if (!(numArgs >= 0 && numArgs <= MAXIMUM_NUMBER_OF_ARGUMENTS)) {
				throw std::runtime_error(
					"Maximum " + std::to_string(MAXIMUM_NUMBER_OF_ARGUMENTS) + " arguments are supported.");
			}

			localsSet = false;
		}

		//Parse external function
		if (isExternFunc) {
			currentFunc = {};
			parseFunctionDef(tokens, i, currentFunc);
			currentFunc.isExternal = true;

			assembly.functions.push_back(currentFunc);
		}

		//Parse the member function
		if (isMemberDef) {
			currentFunc = {};
			parseFunctionDef(tokens, i, currentFunc);

			auto funcName = currentFunc.name;

			//Get the struct name
			auto structNamePos = funcName.find("::");

			if (structNamePos == std::string::npos) {
				throw std::runtime_error("Expected '::' in member function name.");
			}

			auto structTypeName = funcName.substr(0, structNamePos);
			auto memberFunctionName = funcName.substr(structNamePos + 2);

			//Add the implicit this reference
			currentFunc.parameters.insert(currentFunc.parameters.begin(), "Ref.Struct." + structTypeName);

			int numArgs = currentFunc.parameters.size();

			if (numArgs >= 0 && numArgs <= MAXIMUM_NUMBER_OF_ARGUMENTS) {
				currentFunc.structName = structTypeName;
				currentFunc.memberFunctionName = memberFunctionName;
			} else {
				throw std::runtime_error(
					"Maximum " + std::to_string(MAXIMUM_NUMBER_OF_ARGUMENTS) + " arguments are supported.");
			}

			localsSet = false;
			isFunc = true;
		}

		if (isFuncBody && currentToLower == "}") {
			isFuncBody = false;
			isFunc = false;
			assembly.functions.push_back(currentFunc);
		}
	}
}