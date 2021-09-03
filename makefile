
# Makefile for cmpl

export CXX := g++
#export CXX := clang

export CXXFLAGS := -std=c++14 -DSTANDALONE -DPROTO -DCHDIR -ggdb -w 

#export CXXFLAGS := -DSTANDALONE -DPROTO -DCHDIR -ggdb -std=c++14 -Wall -Wno-parentheses
#export CXXFLAGS := -DSTANDALONE -DPROTO -DCHDIR -ggdb -std=c++0x -Wall -Wno-parentheses

# Mögliche Flags:
#				STANDALONE		make standalone cmpl
#               PROTO           protocol file output enabled
#               CHDIR           use of chdir() and getcwd() (if not defined then directories in include are not possible)


SUBDIRS1 := src/Control src/CommonData
SUBDIRS2 := src/Modules src/Extensions


.PHONY: cmpl clean

all : cmpl

cmpl :
	$(MAKE) -C src

clean :
	$(MAKE) -C src clean



.PHONY: tgz

tgz :
	tar -czvf bak/cmpl2-`date +%Y%m%d`.tgz makefile cmpl2.* \
		src/*.{cc,hh,d} src/makefile src/modules.reg bin/makefile bin/.modules src/CommonData/data.reg \
		$(SUBDIRS1:%=%/*.{cc,hh,d}) $(SUBDIRS1:%=%/makefile) $(SUBDIRS2:%=%/makefile) $(SUBDIRS2:%=%/*/*.{cc,hh,d}) $(SUBDIRS2:%=%/*/makefile) \
		src/Modules/Precompiler/*.l src/Modules/Compiler/*.{yy,l} \
		test/{makefile,test-all,test-cmpl} test/*.{cmpl,comp.mps,comp.stdout} \
		doc/*.od?

test :
	$(MAKE) -C test test

