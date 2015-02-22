#include <stdio.h>
#include <string>
#include <iostream>
#include <cxxtest/TestSuite.h>

//Executes the given command
std::string executeCmd(const char* cmd) {
    auto pipe = popen(cmd, "r");

    if (!pipe) {
		return "ERROR";
    }

    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
    	if (fgets(buffer, 128, pipe) != nullptr) {
    		result += buffer;
    	}
    }

    pclose(pipe);
    return result;
}

//Compiles and runs the given program
std::string compileAndRun(std::string programName) {
	std::string invokePath = "./stackc programs/" + programName + ".sl | ../StackJIT/stackjit -nd -i ../StackJIT/rtlib/rtlib.sbc 2>&1";
	return executeCmd(invokePath.data());
}

//Strips error messages of unnecessary information
std::string stripErrorMessage(std::string errorMessage) {
    std::string delimiter = "\n";
    std::size_t pos = 0;
    std::string token;

    std::string stripedString = "";

    int line = 0;

    while ((pos = errorMessage.find(delimiter)) != std::string::npos) {
        token = errorMessage.substr(0, pos);
        errorMessage.erase(0, pos + delimiter.length());

        if (line != 0 && errorMessage.length() > 0) {
            stripedString += token + "\n";
        }

        line++;
    }

    return stripedString;
}

class CompilerTestSuite : public CxxTest::TestSuite {
public:
    void testBasic() {
        TS_ASSERT_EQUALS(compileAndRun("basic/basic1"), "0\n");
        TS_ASSERT_EQUALS(compileAndRun("basic/basic2"), "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("basic/basic3"), "4\n");
        TS_ASSERT_EQUALS(compileAndRun("basic/basic4"), "0\n");

        TS_ASSERT_EQUALS(compileAndRun("basic/or1"), "4\n5\n1\n");
        TS_ASSERT_EQUALS(compileAndRun("basic/and1"), "4\n0\n");
    }

    void testFunctions() {
        TS_ASSERT_EQUALS(compileAndRun("functions/rec1"), "120\n");
        TS_ASSERT_EQUALS(compileAndRun("functions/rec2"), "21\n");
    }

    void testArrays() {
        TS_ASSERT_EQUALS(compileAndRun("arrays/simple1"), "4\n");
        TS_ASSERT_EQUALS(compileAndRun("arrays/simple2"), "4\n");
        TS_ASSERT_EQUALS(compileAndRun("arrays/simple3"), "4\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("arrays/multidim1"), "5\n5\n1337\n");
    }

    void testStrings() {
        TS_ASSERT_EQUALS(compileAndRun("strings/simple1"), "Fello, World!\n0\n");
    }

    void testNamespaces() {
        TS_ASSERT_EQUALS(compileAndRun("namespaces/simple1"), "0\n");
        TS_ASSERT_EQUALS(compileAndRun("namespaces/simple2"), "0\n");
    }

    void testClasses() {
        TS_ASSERT_EQUALS(compileAndRun("classes/simple1"), "6\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/simple2"), "4.47214\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/simple3"), "1.00499\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/simple4"), "4.47214\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/simple5"), "6\n");

        TS_ASSERT_EQUALS(compileAndRun("classes/constructor1"), "1\n2\n0\n");
    }
};