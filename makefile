CC=clang++
CFLAGS=-c -std=c++11
LDFLAGS=-std=c++11

SRCDIR=src
OBJDIR=obj
EXECUTABLE=stackc
FOLDERS = $(OBJDIR)/ast

SOURCES=$(wildcard $(SRCDIR)/*.cpp)
HEADERS=$(wildcard $(SRCDIR)/*.h)

SOURCES += $(wildcard $(SRCDIR)/*/*.cpp)
HEADERS += $(wildcard $(SRCDIR)/*/*.h)

_OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS=$(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(_OBJECTS))

STACKJIT=../StackJIT/stackjit
STACKJIT_OPTIONS=-i ../StackJIT/rtlib/rtlib.sbc -nogc

TESTS_DIR=tests
TEST_RUNNERS_DIR=$(TESTS_DIR)/runners
TEST_EXECUTABLE=test

all: $(OBJDIR) $(SOURCES) $(EXECUTABLE)

run: $(OBJDIR) $(SOURCES) $(EXECUTABLE)
	./$(EXECUTABLE) ${args} | $(STACKJIT) $(STACKJIT_OPTIONS)

compile: $(OBJDIR) $(SOURCES) $(EXECUTABLE)
	./$(EXECUTABLE) ${args}

%.sl: $(OBJDIR) $(SOURCES) $(EXECUTABLE)
	./$(EXECUTABLE) $*.sl

run-tests: $(TESTS_DIR)/compiler-test.h
	mkdir -p $(TEST_RUNNERS_DIR)
	cxxtestgen --error-printer -o $(TEST_RUNNERS_DIR)/compilertest_runner.cpp $(TESTS_DIR)/compiler-test.h
	$(CC) $(LDFLAGS) -o $(TEST_EXECUTABLE) -I $(CXXTEST) $(TEST_RUNNERS_DIR)/compilertest_runner.cpp
	./$(TEST_EXECUTABLE)

$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(FOLDERS)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@	

clean:
	rm -rf $(OBJDIR)
	rm $(EXECUTABLE)
	rm -rf $(TEST_RUNNERS_DIR)
	rm $(TEST_EXECUTABLE)
