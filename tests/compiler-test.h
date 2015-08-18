#include <stdio.h>
#include <string>
#include <iostream>
#include <initializer_list>
#include <algorithm> 
#include <functional> 
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
std::string compileAndRun(std::string programName, std::initializer_list<std::string> libs = {}) {
    std::string libsStr = "";

    for (auto lib : libs) {
        libsStr += " -i " + lib;
    }

	std::string invokePath =
        "./stackc programs/" + programName + ".sl"
        + " | ../StackJIT/stackjit"
        + " -i ../StackJIT/rtlib/rtlib.sbc"
        + libsStr
        + " 2>&1";

	return executeCmd(invokePath.data());
}

//Compiles the given program
std::string compile(std::string programName, std::initializer_list<std::string> libs = {}) {
    std::string libsStr = "";

    for (auto lib : libs) {
        libsStr += " -i " + lib;
    }

    std::string invokePath =
        "./stackc programs/" + programName + ".sl"
        + libsStr
        + " 2>&1";

    return executeCmd(invokePath.data());
}

static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
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

    return rtrim(ltrim(stripedString));
}

//Tests to compile and run programs
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
        TS_ASSERT_EQUALS(compileAndRun("namespaces/using1"), "5\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("namespaces/using2"), "5\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("namespaces/using3"), "5\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("namespaces/usingclass1"), "6\n");
        TS_ASSERT_EQUALS(compileAndRun("namespaces/usingclass2"), "6\n");
        TS_ASSERT_EQUALS(compileAndRun("namespaces/usingclass3"), "6\n");
        TS_ASSERT_EQUALS(compileAndRun("namespaces/usingclass4"), "4\n");
        TS_ASSERT_EQUALS(compileAndRun("namespaces/usingclass5"), "0\n");
    }

    void testClasses() {
        TS_ASSERT_EQUALS(compileAndRun("classes/simple1"), "6\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/simple2"), "4.47214\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/simple3"), "1.00499\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/simple4"), "4.47214\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/simple5"), "6\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/simple6"), "3\n");

        TS_ASSERT_EQUALS(compileAndRun("classes/constructor1"), "1\n2\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/constructor2"), "1\n2\n0\n");

        TS_ASSERT_EQUALS(compileAndRun("classes/array1"), "6\n");

        TS_ASSERT_EQUALS(compileAndRun("classes/loaded1", { "rtlib/vector.sbc" }), "1\n2\n0\n");

        TS_ASSERT_EQUALS(compileAndRun("classes/namespace1"), "5\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/namespace2"), "Hello, World!\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/namespace3"), "Hello, World!\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/namespace4"), "Hello, World!\n0\n");

        TS_ASSERT_EQUALS(stripErrorMessage(compile("classes/access1")), "what():  Cannot access private field of class Point.");
        TS_ASSERT_EQUALS(stripErrorMessage(compile("classes/access2")), "what():  Cannot access private field of class Point.");
        TS_ASSERT_EQUALS(compileAndRun("classes/access3"), "0\n");
        TS_ASSERT_EQUALS(stripErrorMessage(compile("classes/access4")), "what():  Cannot call private function of class Point.");
        TS_ASSERT_EQUALS(compileAndRun("classes/access5"), "0\n");

        TS_ASSERT_EQUALS(compileAndRun("classes/fieldarray1"), "Hello, World!\n0\n");
        TS_ASSERT_EQUALS(compileAndRun("classes/fieldarray2"), "String 1\n0\n");

        TS_ASSERT_EQUALS(
            stripErrorMessage(compile("classes/loaded2", { "programs/classes/point1.sbc" })),
            "what():  Cannot access private field of class Point.");

        TS_ASSERT_EQUALS(
            stripErrorMessage(compile("classes/loaded3", { "programs/classes/point2.sbc" })),
            "what():  Cannot call private function of class Point.");
    }
};