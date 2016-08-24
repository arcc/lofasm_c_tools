SOURCES = $(wildcard *.cpp)
EXECUTABLE = $(patsubst %.cpp,%,$(SOURCES))
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
CC=gcc
CXX=g++
RM=rm -f
IDIR = ../include
CPPFLAGS=-g -I$(IDIR)
LDLIBS = ../lib/lib_lofasm_dedispersion.a

all:$(EXECUTABLE)
#:$(SOURCES)
#	$(CXX) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $<
clean:
	$(RM) $(OBJECTS) $(EXECUTABLE) *~
