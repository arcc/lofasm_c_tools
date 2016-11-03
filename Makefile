PROJECT = lib_lofasm_dedispersion.a
CC=gcc
CXX=g++
RM=rm -f
IDIR = include
IODIR = ../lofasmio-1.0/
CPPFLAGS=-g -I$(IDIR) -I$(IODIR)
SOURCES= $(wildcard src/*.cpp)
OBJECTS = $(addprefix lib/,$(notdir $(SOURCES:.cpp=.o)))
OBJDIR = lib
LDLIBS = ../lofasmio-1.0/liblofasmio.a -lz
all:$(PROJECT)

$(OBJECTS): $(SOURCES)
	$(CXX) $(CPPFLAGS) -c -o $@ $(@:lib/%.o=src/%.cpp) $(LDLIBS)


 $(PROJECT): $(OBJECTS)
	libtool -o $(OBJDIR)/$(PROJECT) -static $(OBJECTS)

clean:
	$(RM) $(OBJECTS) $(OBJDIR)/$(PROJECT)
