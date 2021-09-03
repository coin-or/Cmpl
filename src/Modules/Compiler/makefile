
# Makefile for cmpl module: Compiler

CXX ?= g++
CXXFLAGS ?= -DSTANDALONE -DPROTO -DCHDIR -ggdb -std=c++0x -Wall -Wno-parentheses

MODOBJS := $(patsubst %.cc,%.o,$(wildcard *.cc))

%.d: %.cc
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


cmpl : $(MODOBJS) _CMPL_Parser.o _CMPL_Scanner.o

all : cmpl

#_CMPL_Parser.cc _CMPL_Parser.hh : CMPL_Parser.yy
#	bison -o _CMPL_Parser.cc CMPL_Parser.yy
#	rm location.hh position.hh
#	cp location_own.hh location.hh

#_CMPL_Scanner.cc : CMPL_Scanner.l _CMPL_Parser.hh
#	flex -o _CMPL_Scanner.cc CMPL_Scanner.l


clean:
	-rm *.o
#	-rm _CMPL_Parser.cc _CMPL_Scanner.cc


include $(patsubst %.cc,%.d,$(wildcard *.cc))

