# Makefile fuer cmpl

CXX = g++
CXXFLAGS = -w -DPROTO -DCHDIR -DLANG_EN #-ggdb
# Possible flags:
#		PROTO		protocol file
#		CHDIR		use of chdir() and getcwd() (nessecary)
#		LANG_EN		error messages in english in english (alternative LANG_DE)


OUTPUT = ../../bin/cmpl

INCLUDES = gen.h genstart.h gencode.h gentext.h	\
			Main.hh FileIO.hh Compiler.hh SetIter.hh

OBJS = _CMPL_Parser.o _CMPL_Scanner.o Main.o Compiler.o Interpreter.o FileIO.o Start.o SetIter.o \
			IntFuncBase.o IntFuncCode.o IntFuncEnv.o IntFuncValue.o IntFuncDef.o \
			IntFuncIndex.o IntFuncOper.o IntFuncCtrl.o IntFuncLine.o IntFuncTerm.o

LIBS =

CMPLMAKE = makefile


all : $(OBJS) $(CMPLMAKE)
		   $(CXX) -o $(OUTPUT) $(OBJS) $(LIBS)


_CMPL_Parser.cc _CMPL_Parser.hh : CMPL_Parser.yy $(INCLUDES) $(CMPLMAKE) 
	bison -o _CMPL_Parser.cc CMPL_Parser.yy

_CMPL_Scanner.cc : CMPL_Scanner.l _CMPL_Parser.hh $(INCLUDES) $(CMPLMAKE)
	flex -o _CMPL_Scanner.cc CMPL_Scanner.l


_CMPL_Parser.o		: _CMPL_Parser.cc _CMPL_Parser.hh $(INCLUDES) $(CMPLMAKE)
_CMPL_Scanner.o		: _CMPL_Scanner.cc _CMPL_Parser.hh $(INCLUDES) $(CMPLMAKE)

Start.o			: Start.cc $(INCLUDES) $(CMPLMAKE)
Main.o			: Main.cc Main.hh Interpreter.hh $(INCLUDES) $(CMPLMAKE)
Compiler.o		: Compiler.cc Compiler.hh $(INCLUDES) $(CMPLMAKE)
Interpreter.o		: Interpreter.cc Interpreter.hh IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)
FileIO.o		: FileIO.cc FileIO.hh $(INCLUDES) $(CMPLMAKE)

SetIter.o		: SetIter.cc SetIter.hh $(INCLUDES) $(CMPLMAKE)

IntFuncBase.o		: IntFuncBase.cc IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)
IntFuncCode.o		: IntFuncCode.cc IntFuncCode.hh IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)
IntFuncEnv.o		: IntFuncEnv.cc IntFuncEnv.hh IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)
IntFuncValue.o		: IntFuncValue.cc IntFuncValue.hh IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)
IntFuncDef.o		: IntFuncDef.cc IntFuncDef.hh IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)
IntFuncIndex.o		: IntFuncIndex.cc IntFuncIndex.hh IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)
IntFuncOper.o		: IntFuncOper.cc IntFuncOper.hh IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)
IntFuncCtrl.o		: IntFuncCtrl.cc IntFuncCtrl.hh IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)
IntFuncLine.o		: IntFuncLine.cc IntFuncLine.hh IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)
IntFuncTerm.o		: IntFuncTerm.cc IntFuncTerm.hh IntFuncBase.hh $(INCLUDES) $(CMPLMAKE)



clean :
		rm $(OBJS)
