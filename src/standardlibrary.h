#pragma once

class Binder;

//Defines the interface to the standard library
namespace StandardLibrary {
	//Adds the standard library to the given binder
	void add(Binder& binder);
}