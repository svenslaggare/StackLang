#include "codegenerator.h"
#include "asthelpers.h"
#include "functionast.h"
#include "programast.h"
#include "expressionast.h"
#include "type.h"
#include "typechecker.h"
#include <stdexcept>

//Function parameter
FunctionParameter::FunctionParameter(std::string name, std::shared_ptr<Type> type)
	: name(name), type(type) {

}

FunctionParameter::FunctionParameter()
	: name(""), type(nullptr) {

}

//Generated function
GeneratedFunction::GeneratedFunction(std::string functionName, std::vector<FunctionParameter> parameters, std::shared_ptr<Type> returnType)
	: mFunctionName(functionName), mParameters(parameters), mReturnType(returnType) {

}

GeneratedFunction::GeneratedFunction()
	: mFunctionName(""), mReturnType(nullptr) {

}

int GeneratedFunction::numLocals() const {
	return mLocals.size();
}

const std::map<std::string, std::pair<int, std::shared_ptr<Type>>>& GeneratedFunction::locals() const {
	return mLocals;
}

int GeneratedFunction::newLocal(std::string name, std::shared_ptr<Type> type) {
	int index = numLocals();
	mLocals.insert({ name, { index, type } });
	return index;
}

Local GeneratedFunction::getLocal(std::string name) const {
	return mLocals.at(name);
}

int GeneratedFunction::functionParameterIndex(std::string paramName) const {
	for (int i = 0; i < mParameters.size(); i++) {
		if (mParameters.at(i).name == paramName) {
			return i;
		}
	}

	return -1;
}

void GeneratedFunction::addInstruction(const std::string& instruction) {
	mInstructions.push_back(instruction);
}

int GeneratedFunction::numInstructions() const {
	return mInstructions.size();
}

std::string& GeneratedFunction::instruction(int index) {
	return mInstructions.at(index);
}

void GeneratedFunction::addReturnBranch(int index) {
	mReturnBranches.push_back(index);
}

void GeneratedFunction::outputGeneratedCode(std::ostream& os) {
	bool isFirst = true;

	os << "func " << mFunctionName << "(";

	for (auto param : mParameters) {
		if (!isFirst) {
			os << " ";
		} else {
			isFirst = false;
		}

		os << param.type->vmType();
	}

	os << ") " << mReturnType->vmType() << std::endl;
	os << "{";

	if (mLocals.size() > 0) {
		os << std::endl << "   .locals " << mLocals.size() << std::endl;

		for (auto local : mLocals) {
			os << "   .local " << local.second.first << " " << local.second.second->vmType() << std::endl;
		}
	}

	//Handle return
	int returnInst = mInstructions.size();
	auto genInstructions = mInstructions;

	if (mReturnType->name() != "Void") {
		genInstructions.push_back("LDLOC " + std::to_string(getLocal(CodeGenerator::returnValueLocal).first));
	}

	genInstructions.push_back("RET");

	for (auto returnBranch : mReturnBranches) {
		genInstructions[returnBranch] += " " + std::to_string(returnInst);
	}

	isFirst = false;
	int i = 0;
	for (auto inst : genInstructions) {
		if (!isFirst) {
			os << std::endl;
		} else {
			isFirst = false;
		}

		os << "   " << inst;
		i++;
	}

	os << std::endl << "}" << std::endl;
}

//Code generator
CodeGenerator::CodeGenerator(const TypeChecker& typeChecker)
	: mTypeChecker(typeChecker) {

}

const TypeChecker& CodeGenerator::typeChecker() const {
	return mTypeChecker;
}

void CodeGenerator::generateProgram(std::shared_ptr<ProgramAST> programAST) {
	programAST->visitFunctions([&](std::shared_ptr<FunctionAST> func) {
		auto& genFunc = newFunction(func->prototype());
		func->generateCode(*this, genFunc);
	});
}

GeneratedFunction& CodeGenerator::newFunction(std::shared_ptr<FunctionPrototypeAST> functionPrototype) {
	std::vector<FunctionParameter> parameters;

	for (auto param : functionPrototype->parameters()) {
		parameters.push_back(FunctionParameter(param->varName(), mTypeChecker.findType(param->varType())));
	}

	mFunctions.push_back(GeneratedFunction(functionPrototype->fullName("."), parameters, mTypeChecker.findType(functionPrototype->returnType())));
	auto& newFunc = mFunctions[mFunctions.size() - 1];

	if (functionPrototype->returnType() != "Void") {
		newFunc.newLocal(CodeGenerator::returnValueLocal, mTypeChecker.findType(functionPrototype->returnType()));
	}

	return newFunc;
}

void CodeGenerator::printGeneratedCode() {
	bool isFirst = true;
	for (auto func : mFunctions) {
		if (!isFirst) {
			std::cout << std::endl;
		} else {
			isFirst = false;
		}

		func.outputGeneratedCode(std::cout);
	}
}

void CodeGenerator::codeGenError(std::string errorMessage) {
	throw std::runtime_error(errorMessage);
}

std::string CodeGenerator::returnValueLocal = "$return_value$";