StackLang
=========

Simple object-orientend language for the [StackJIT VM](https://github.com/svenslaggare/StackJIT/).

##Compiling##
To compile a source file:
```
./stackc <source file>
```

To compile and run a source file:
```
make run program=<source file>
```

##Build##
To build:
```
make all
```
To run tests (requires [CXXTest](http://cxxtest.com/) and the StackJIT VM):
```
make test
```

##Running##
To run a compiled program, the StackJIT VM is required. It must be located in a parent directory, in the following structure:
```
Parent
|--StackLang
|   |-stackc
|--StackJIT
|   |-stackjit
```
