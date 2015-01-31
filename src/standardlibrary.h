#pragma once

class Binder;
class TypeChecker;

//Defines the interface to the standard library
namespace StandardLibrary {
	//Adds the standard library to the given binder
	void add(Binder& binder, TypeChecker& typeChecker);
}