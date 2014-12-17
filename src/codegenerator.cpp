#include "codegenerator.h"
#include "asthelpers.h"
#include "functionast.h"
#include "programast.h"
#include "expressionast.h"
#include <stdexcept>

//Generated function
GeneratedFunction::GeneratedFunction(std::shared_ptr<FunctionPrototypeAST> prototype)
	: mPrototype(prototype) {

}

GeneratedFunction::GeneratedFunction()
	: mPrototype(nullptr) {

}

int GeneratedFunction::numLocals() const {
	return mLocals.size();
}

int GeneratedFunction::newLocal(std::shared_ptr<Type> type) {
	int index = numLocals();
	mLocals.push_back(type);
	return index;
}

std::shared_ptr<Type> GeneratedFunction::getLocal(int index) const {
	return mLocals.at(index);
}

int GeneratedFunction::functionParameterIndex(std::string paramName) const {
	for (int i = 0; i < mPrototype->parameters().size(); i++) {
		if (mPrototype->parameters().at(i)->varName() == paramName) {
			return i;
		}
	}

	return -1;
}

void GeneratedFunction::addInstruction(const std::string& instruction) {
	mInstructions.push_back(instruction);
}

void GeneratedFunction::outputGeneratedCode(std::ostream& os) {
	bool isFirst = true;

	os << "func " << mPrototype->name() << "(";

	for (auto param : mPrototype->parameters()) {
		if (!isFirst) {
			os << " ";
		} else {
			isFirst = false;
		}

		os << param->varType();
	}

	os << ") " << mPrototype->returnType() << std::endl;
	os << "{";

	if (mLocals.size() > 0) {
		os << ".locals " << mLocals.size() << std::endl;

		for (int i = 0; i < mLocals.size(); i++) {
			os << ".local " << i << mLocals[i] << std::endl;
		}
	}

	isFirst = false;

	for (auto inst : mInstructions) {
		if (!isFirst) {
			os << std::endl;
		} else {
			isFirst = false;
		}

		os << "   " << inst;
	}

	os << std::endl << "}" << std::endl;
}

//Code generator
void CodeGenerator::generateProgram(std::shared_ptr<ProgramAST> programAST) {
	for (auto func : programAST->functions()) {
		auto& genFunc = newFunction(func->prototype());
		func->generateCode(*this, genFunc);
	}
}

GeneratedFunction& CodeGenerator::newFunction(std::shared_ptr<FunctionPrototypeAST> functionPrototype) {
	mFunctions.insert({ functionPrototype->name(), GeneratedFunction(functionPrototype) });
	return mFunctions[functionPrototype->name()];
}

GeneratedFunction& CodeGenerator::getFunction(std::string funcName) {
	return mFunctions.at(funcName);
}

void CodeGenerator::printGeneratedCode() {
	bool isFirst = true;
	for (auto func : mFunctions) {
		if (!isFirst) {
			std::cout << std::endl;
		} else {
			isFirst = false;
		}

		func.second.outputGeneratedCode(std::cout);
	}
}

void CodeGenerator::codeGenError(std::string errorMessage) {
	throw std::runtime_error(errorMessage);
}