#pragma once

class Binder;
class TypeChecker;

//Defines builtin features of the language
namespace StackLang {
	namespace Builtin {
		//Adds the builtin features to the given binder & typechecker
		void add(Binder& binder, TypeChecker& typeChecker);
	}
}