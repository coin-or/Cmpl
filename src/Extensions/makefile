
# Makefile for all cmpl extensions

CXX ?= g++
CXXFLAGS ?= -DSTANDALONE -DPROTO -DCHDIR -ggdb -std=c++0x -Wall -Wno-parentheses

EXTENSIONS := DefSymbols DataImport Remodel SolverSel

all : extensions


.PHONY: extensions $(EXTENSIONS)

extensions : $(EXTENSIONS)

$(EXTENSIONS):
	$(MAKE) -C $@


.PHONY: clean

clean :
	for dir in $(EXTENSIONS); do $(MAKE) -C $$dir clean; done

