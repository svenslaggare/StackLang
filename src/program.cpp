#include "program.h"
#include "programast.h"

Program::Program() {

}

bool Program::getFunction(std::string name, Function& function) const {
if (mFunctions.count(name) == 0) {
		function = mFunctions.at(name);
		return true;
	} else {
		return false;
	}
}

bool Program::addFunction(Function function) {
	if (mFunctions.count(function.name()) == 0) {
		mFunctions.insert({ function.name(), function });
		return true;
	} else {
		return false;
	}
}

void Program::extractFunctions(std::shared_ptr<ProgramAST> programAST) {
	for (auto func : programAST->functions()) {
		
	}
}