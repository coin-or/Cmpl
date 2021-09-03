
# Makefile for all cmpl modules

CXX ?= g++
CXXFLAGS ?= -DSTANDALONE -DPROTO -DCHDIR -ggdb -std=c++0x -Wall -Wno-parentheses


MODULES :=  Serialize Transfer Precompiler Compiler Interpreter OutModel Solver SolutionReport

all : modules


.PHONY: modules $(MODULES)

modules : $(MODULES)

$(MODULES):
	$(MAKE) -C $@


.PHONY: clean

clean :
	for dir in $(MODULES); do $(MAKE) -C $$dir clean; done

