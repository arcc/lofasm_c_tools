PROJECT = lib_dedispersion.a
CC=gcc
CXX=g++
RM=rm -f
IDIR = include
IODIR = ../lofasmio/
OPTDIR = /opt/local/include
CPPFLAGS=-g -I$(IDIR) -I$(IODIR) -I$(OPTDIR)
SOURCES= $(wildcard src/*.cpp)
OBJECTS = $(addprefix lib/,$(notdir $(SOURCES:.cpp=.o)))
TESTSRC = $(wildcard tests/*.cpp)
TESTS= $(addprefix tests/,$(notdir $(TESTSRC:.cpp=)))
PROGSRC = $(wildcard prog/*.cpp)
PROG = $(addprefix prog/,$(notdir $(PROGSRC:.cpp=)))
OBJDIR = lib
LDLIBS = ../lofasmio/liblofasmio.a -lz
IOLIB = ../lofasmio/liblofasmio.a
DEDSPSLIBS = ./lib/lib_dedispersion.a
all:$(PROJECT)

$(OBJECTS): $(SOURCES)
	@mkdir -p lib
	$(CXX) $(CPPFLAGS) -c -o $@ $(@:lib/%.o=src/%.cpp) $(LDLIBS)


 $(PROJECT): $(OBJECTS)
	libtool -o $(OBJDIR)/$(PROJECT) -static $(OBJECTS)

prog:$(PROG)
$(PROG): $(IOLIB) $(DEDSPSLIBS)

tests:$(TESTS)
$(TESTS): $(IOLIB) $(DEDSPSLIBS)
# $(CXX) $(CPPFLAGS) $@.cpp $(LDLIBS) $(DEDSPSLIBS) -o $@

clean:
	$(RM) $(OBJECTS) $(OBJDIR)/$(PROJECT)
	$(RM) $(TESTS) *~
	$(RM) $(PROG) *~
