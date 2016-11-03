SOURCES = $(wildcard *.cpp)
EXECUTABLE = $(patsubst %.cpp,%,$(SOURCES))
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
CC=gcc
CXX=g++
RM=rm -f
IDIR = ../include
IODIR = ../../lofasmio-1.0/
CPPFLAGS=-g -I$(IDIR) -I$(IODIR)
LDLIBS = ../lib/lib_lofasm_dedispersion.a ../../lofasmio-1.0/liblofasmio.a -lz

all:$(EXECUTABLE)
#:$(SOURCES)
  #$(CXX) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $<
clean:
	$(RM) $(OBJECTS) $(EXECUTABLE) *~
