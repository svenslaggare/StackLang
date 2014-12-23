#include "standardlibrary.h"
#include <string>
#include "binder.h"

void StandardLibrary::add(Binder& binder) {
	binder.addFunction("println", { { "Int", "x" } }, "Void");
	binder.addFunction("print", { { "Int", "x" } }, "Void");
	binder.addFunction("printchar", { { "Int", "x" } }, "Void");
}