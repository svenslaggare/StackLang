CC=clang++
CFLAGS=-c -std=c++11
LDFLAGS=-std=c++11

SRCDIR=src
OBJDIR=obj
EXECUTABLE=stackc

SOURCES=$(wildcard $(SRCDIR)/*.cpp)
HEADERS=$(wildcard $(SRCDIR)/*.h)

_OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS=$(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(_OBJECTS))

all: $(OBJDIR) $(SOURCES) $(EXECUTABLE)

test: $(OBJDIR) $(SOURCES) $(EXECUTABLE)
	./stackc programs/mandelbrot.sl

run: $(OBJDIR) $(SOURCES) $(EXECUTABLE)
	./stackc programs/mandelbrot.sl | ../StackJIT/stackjit -i ../StackJIT/rtlib/rtlib.sbc -nogc

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@	

clean:
	rm -rf $(OBJDIR)
	rm $(EXECUTABLE)
