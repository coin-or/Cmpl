
# Makefile for cmpl module: Serialize

CXX ?= g++
CXXFLAGS ?= -DSTANDALONE -DPROTO -DCHDIR -ggdb -std=c++0x -Wall -Wno-parentheses

MODOBJS := $(patsubst %.cc,%.o,$(wildcard *.cc))

%.d: %.cc
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


cmpl : $(MODOBJS)

all : cmpl

clean:
	-rm *.o

include $(patsubst %.cc,%.d,$(wildcard *.cc))

