
# Makefile for cmpl

export CXX := g++
export CXXFLAGS := -DSTANDALONE -DPROTO -DCHDIR -ggdb -std=c++0x -Wall -Wno-parentheses
# Mögliche Flags:
#				STANDALONE		make standalone cmpl
#               PROTO           protocol file output enabled
#               CHDIR           use of chdir() and getcwd() (if not defined then directories in include are not possible)


.PHONY: cmpl clean

all : cmpl

cmpl :
	$(MAKE) -C ../src

clean :
	$(MAKE) -C ../src clean



.PHONY: tgz

tgz :
	$(MAKE) -C ../src tgz

test :
	$(MAKE) -C ../test test


