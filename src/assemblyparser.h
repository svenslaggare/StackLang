#pragma once
#include <vector>
#include <string>
#include <unordered_map>

//The assembly parser
namespace AssemblyParser {
	//The list of op codes
	enum class OpCodes : unsigned char {
		NOP,
		PUSH_INT,
		PUSH_FLOAT,
		PUSH_CHAR,
		POP,
		ADD,
		SUB,
		MUL,
		DIV,
		PUSH_TRUE,
		PUSH_FALSE,
		AND,
		OR,
		NOT,
		CONVERT_INT_TO_FLOAT,
		CONVERT_FLOAT_TO_INT,
		COMPARE_EQUAL,
		COMPARE_NOT_EQUAL,
		COMPARE_GREATER_THAN,
		COMPARE_GREATER_THAN_OR_EQUAL,
		COMPARE_LESS_THAN,
		COMPARE_LESS_THAN_OR_EQUAL,
		LOAD_LOCAL,
		STORE_LOCAL,
		CALL,
		CALL_INSTANCE,
		RET,
		LOAD_ARG,
		BRANCH,
		BRANCH_EQUAL,
		BRANCH_NOT_EQUAL,
		BRANCH_GREATER_THAN,
		BRANCH_GREATER_THAN_OR_EQUAL,
		BRANCH_LESS_THAN,
		BRANCH_LESS_THAN_OR_EQUAL,
		PUSH_NULL,
		GARBAGE_COLLECT,
		NEW_ARRAY,
		LOAD_ELEMENT,
		STORE_ELEMENT,
		LOAD_ARRAY_LENGTH,
		NEW_OBJECT,
		LOAD_FIELD,
		STORE_FIELD,
		LOAD_STRING
	};

	//Represents a parsed but not bound instruction.
	struct Instruction {
		//The op code
		OpCodes opCode;

		//Primitive values
		float floatValue;
		int intValue;
		char charValue;
		std::string strValue;

		//Used by call instructions
		std::vector<std::string> parameters;

		//Used by the object instructions
		std::string calledStructType;

		//Creates a new instruction
		Instruction();

		//Creates a new instruction with the given op code
		static Instruction make(OpCodes opCode);

		//Creates a new instruction with an int as value
		static Instruction makeWithInt(OpCodes opCode, int value);

		//Creates a new instruction with a float as value
		static Instruction makeWithFloat(OpCodes opCode, float value);

		//Creates a new instruction with a char as value
		static Instruction makeWithChar(OpCodes opCode, char value);

		//Creates a new instruction with a string as the value
		static Instruction makeWithStr(OpCodes opCode, std::string value);

		//Creates a new call instruction
		static Instruction makeCall(std::string funcName, std::vector<std::string> parameters);

		//Creates a new call instance instruction
		static Instruction makeCallInstance(std::string structType, std::string funcName, std::vector<std::string> parameters);

		//Creates a new create object instruction
		static Instruction makeNewObject(std::string structType, std::vector<std::string> parameters);
	};

	//Represents an attribute
	struct Attribute {
		std::string name;
		std::unordered_map<std::string, std::string> values;
	};

	//Represents an attributes container
	struct AttributeContainer {
		std::unordered_map<std::string, Attribute> attributes;
	};

	//Represents a function
	struct Function {
		std::string name;
		std::string returnType;
		std::vector<std::string> parameters;

		bool isMemberFunction;
		std::string structName;
		std::string memberFunctionName;

		bool isExternal;

		std::vector<Instruction> instructions;
		std::vector<std::string> localTypes;

		AttributeContainer attributes;

		//Creates a new function
		Function();

		//Sets the number of locals
		void setNumLocals(int num);

		//Returns the number of locals
		std::size_t numLocals() const;
	};

	//Represents a field in a structure
	struct Field {
		std::string name;
		std::string type;
	};

	//Represents a structure
	struct Struct {
		std::string name;
		std::vector<Field> fields;

		AttributeContainer attributes;

		//Creates a new struct
		Struct();
	};

	//Represents an assembly
	struct Assembly {
		std::vector<Function> functions;
		std::vector<Struct> structs;
	};

	//The maximum number of supported arguments
	const int MAXIMUM_NUMBER_OF_ARGUMENTS = 6;

	//Tokenizes from the given stream
	std::vector<std::string> tokenize(std::istream& stream);

	//Parses the given tokens into the given assembly
	void parseTokens(const std::vector<std::string>& tokens, Assembly& assembly);
}