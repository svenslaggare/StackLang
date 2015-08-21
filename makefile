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
STACKJIT_OPTIONS=

TESTS_DIR=tests
TEST_RUNNERS_DIR=$(TESTS_DIR)/runners
TESTS=$(wildcard $(TESTS_DIR)/*.h)
_TESTS=$(TESTS:.h=.cpp)
TEST_RUNNERS=$(patsubst $(TESTS_DIR)/%,$(TEST_RUNNERS_DIR)/%,$(_TESTS))

MAIN_OBJECT=$(OBJDIR)/stacklang.o
TEST_OBJECTS=$(filter-out $(MAIN_OBJECT), $(OBJECTS))

all: $(OBJDIR) $(SOURCES) $(EXECUTABLE)

run: $(OBJDIR) $(SOURCES) $(EXECUTABLE)
	./$(EXECUTABLE) ${program} | $(STACKJIT) $(STACKJIT_OPTIONS)

compile: $(OBJDIR) $(SOURCES) $(EXECUTABLE)
	./$(EXECUTABLE) ${program}

test: $(TESTS)

$(TEST_RUNNERS_DIR):
	mkdir -p $(TEST_RUNNERS_DIR)

$(TESTS_DIR)/%.h: $(OBJDIR) $(EXECUTABLE) $(TEST_RUNNERS_DIR) FORCE
	cxxtestgen --error-printer -o $(TEST_RUNNERS_DIR)/$*-runner.cpp $@
	$(CC) $(LDFLAGS) $(TEST_OBJECTS) -o $(TEST_RUNNERS_DIR)/$* -I $(CXXTEST) $(TEST_RUNNERS_DIR)/$*-runner.cpp
	$(TEST_RUNNERS_DIR)/$*

FORCE:

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
