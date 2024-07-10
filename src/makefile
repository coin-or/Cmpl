
# Makefile for cmpl

export CXX ?= g++
#export CXXFLAGS ?= -DSTANDALONE -DPROTO -DCHDIR -ggdb -std=c++0x -Wall -Wno-parentheses -lpthread
export CXXFLAGS ?= -DSTANDALONE -DPROTO -DCHDIR  -std=c++0x -Wall -Wno-parentheses -lpthread
# Mögliche Flags:
#				STANDALONE		make standalone cmpl
#               PROTO           protocol file output enabled
#               CHDIR           use of chdir() and getcwd() (if not defined then directories in include are not possible)


CMPLMAKE := makefile

OUTPUT = ../bin/cmpl

# INCLUDES := $(wildcard Control/*.hh)

OBJS := $(patsubst %.cc,%.o,$(wildcard *.cc))
LIBS :=

SUBDIRS1 := Control CommonData
SUBDIRS2 := Modules Extensions

%.d: %.cc
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


$(OUTPUT) : $(OBJS) subobjs
	$(CXX) -o $(OUTPUT) $(OBJS) $(SUBDIRS1:%=%/*.o) $(SUBDIRS2:%=%/*/*.o) $(LIBS) -lpthread  

all : $(OUTPUT)



.PHONY: subobjs $(SUBDIRS1) $(SUBDIRS2)

subobjs : $(SUBDIRS1) $(SUBDIRS2)

$(SUBDIRS1) $(SUBDIRS2):
	$(MAKE) -C $@


include $(patsubst %.cc,%.d,$(wildcard *.cc))



.PHONY: clean

clean :
	-rm *.o
	for dir in $(SUBDIRS1) $(SUBDIRS2); do $(MAKE) -C $$dir clean; done


.PHONY: tgz

tgz :
	$(MAKE) -C .. tgz

test :
	$(MAKE) -C ../test test

