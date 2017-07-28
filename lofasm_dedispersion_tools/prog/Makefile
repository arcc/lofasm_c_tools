PREFIX = $(HOME)
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include
SOURCES = $(wildcard *.cpp)
EXECUTABLE = $(patsubst %.cpp,%,$(SOURCES))
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
CC=gcc
CXX=g++
RM=rm -f
IDIR = ../include
IODIR = ../../lofasmio/
CPPFLAGS=-g -I$(IDIR) -I$(IODIR)
LDLIBS = ../lib/lib_lofasm_dedispersion.a ../../lofasmio/liblofasmio.a -lz

all:$(EXECUTABLE)
#:$(SOURCES)
  #$(CXX) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $<

install:
	mkdir -p $(BINDIR);
	cp $(EXECUTABLE) $(BINDIR)
clean:
	$(RM) $(OBJECTS) $(EXECUTABLE) *~
