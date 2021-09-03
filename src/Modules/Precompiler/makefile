
# Makefile for cmpl module: Precompiler

CXX ?= g++
CXXFLAGS ?= -DSTANDALONE -DPROTO -DCHDIR -ggdb -std=c++0x -Wall -Wno-parentheses

MODOBJS := $(patsubst %.cc,%.o,$(wildcard *.cc))

%.d: %.cc
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


cmpl : $(MODOBJS) _PreScanner.o

all : cmpl


#_PreScanner.cc : PreScanner.l
#	flex -o _PreScanner.cc PreScanner.l


clean:
	-rm *.o
#	-rm _PreScanner.cc


include $(patsubst %.cc,%.d,$(wildcard *.cc))


