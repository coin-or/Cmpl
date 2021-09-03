
/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010, 2011
 *  Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany and Thomas Schleiff - Halle(Saale),
 *  Germany
 *
 *  Coliop3 and CMPL are projects of the Technical University of
 *  Applied Sciences Wildau and the Institute for Operations Research
 *  and Business Management at the Martin Luther University
 *  Halle-Wittenberg.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  CMPL is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CMPL is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/


#include "Compiler.hh"
#include "CompilerContext.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/ExternDataInfo.hh"
#include "../../CommonData/CmplSourceInput.hh"
#include "../../CommonData/IntCode.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "compile" that is implemented in class "Compiler".
	// to register this module you have to insert "MODULE_CLASS_REG(1, compile)" to file "modules.reg"
	MODULE_CLASS( compile, Compiler )



	/*********** initialization **********/

	/**
	 * destructor
	 */
	Compiler::~Compiler()
	{
		if (_inputFile)
			delete _inputFile;

		if (_externFile)
			delete _externFile;

		if (_syntaxXmlFile)
			delete _syntaxXmlFile;

		if (_outputCode)
			delete _outputCode;
	}

	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl			main object
	 * @param data			data object
	 * @param name			module name (or alias)
	 */
	void Compiler::init(MainControl *ctrl, MainData *data, const char *name)
	{
		ModuleBase::init(ctrl, data, name);

		_inputFile = NULL;
		_externFile = NULL;
		_syntaxXmlFile = NULL;

        _intOutOfRangeMode = INT_OPER_OORANGE_ERROR;

		_traceScanning = false;
		_traceParsing = false;
		_outputCode = NULL;
	}


	/*********** handling of command line options **********/

#define OPTION_COMPILER_INFILE				10
#define OPTION_COMPILER_INEXTINFO			11

#define OPTION_COMPILER_SYNTAXXML			20
#define OPTION_COMPILER_CHECKONLY			25

#define OPTION_COMPILER_ALLOW_NEWPUBLIC		30
#define OPTION_COMPILER_ALLOW_NEWCONST		31

#define OPTION_COMPILER_COMPAT_LINENAMEPREF	40
#define OPTION_COMPILER_COMPAT_CBSYMBOLS	41
#define OPTION_COMPILER_COMPAT_CBHASSIGN	42

#define OPTION_COMPILER_WARN_INSERTEOS		50

#define OPTION_COMPILER_INT_OORANGE_MODE	60

#define OPTION_COMPILER_TRACESCANNER		80
#define OPTION_COMPILER_TRACEPARSER			81
#define OPTION_COMPILER_OUTCODE				82


	/**
	 * register command line options options for delivery to this module
	 * @param modOptReg		vector to register in
	 */
	void Compiler::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);

		REG_CMDL_OPTION( OPTION_COMPILER_INFILE, "i-pre", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
		REG_CMDL_OPTION( OPTION_COMPILER_INEXTINFO, "i-extern", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );

		REG_CMDL_OPTION( OPTION_COMPILER_SYNTAXXML, "syntax-xml", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
		REG_CMDL_OPTION( OPTION_COMPILER_CHECKONLY, "check-only", 0, 0, CMDL_OPTION_NEG_DELIV, true );

		REG_CMDL_OPTION( OPTION_COMPILER_ALLOW_NEWPUBLIC, "allow-new-public", 0, 0, CMDL_OPTION_NEG_DELIV, true );
		REG_CMDL_OPTION( OPTION_COMPILER_ALLOW_NEWCONST, "allow-new-const", 0, 0, CMDL_OPTION_NEG_DELIV, true );

		REG_CMDL_OPTION( OPTION_COMPILER_COMPAT_LINENAMEPREF, "compat-linenamepref", 0, 0, CMDL_OPTION_NEG_DELIV, true );
		REG_CMDL_OPTION( OPTION_COMPILER_COMPAT_CBSYMBOLS, "compat-cbsymbols", 0, 0, CMDL_OPTION_NEG_DELIV, true );
		REG_CMDL_OPTION( OPTION_COMPILER_COMPAT_CBHASSIGN, "compat-cbh-assign", 0, 0, CMDL_OPTION_NEG_DELIV, true );

		REG_CMDL_OPTION( OPTION_COMPILER_WARN_INSERTEOS, "warn-insert-eos", 0, 0, CMDL_OPTION_NEG_DELIV, true );

        REG_CMDL_OPTION( OPTION_COMPILER_INT_OORANGE_MODE, "int-out-of-range", 1, 1, CMDL_OPTION_NEG_ERROR, true );

#if PROTO
		REG_CMDL_OPTION( OPTION_COMPILER_TRACESCANNER, "trace-scanner", 0, 0, CMDL_OPTION_NEG_DELIV, true );
		REG_CMDL_OPTION( OPTION_COMPILER_TRACEPARSER, "trace-parser", 0, 0, CMDL_OPTION_NEG_DELIV, true );
		REG_CMDL_OPTION( OPTION_COMPILER_OUTCODE, "o-code", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
#endif
	}

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref			reference number of option registration, should be used for discriminate the options
	 * @param prio			priority value of option
	 * @param opt			option
	 * @return				true if option is used by the module
	 */
	bool Compiler::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
		if (ModuleBase::parseOption(ref, prio, opt))
			return true;

		switch (ref) {
			case OPTION_COMPILER_INFILE:
				if (_inputFile) {
					delete _inputFile;
					_inputFile = NULL;
				}
				if (!(opt->neg())) {
					_inputFile = new FileInput();
					_inputFile->setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_OPRE, true);
					if (opt->size() > 0)
						_inputFile->setLocSrc(opt->argPos(0));
					else
						_inputFile->setLocSrc(opt->loc(true));
				}
				return true;

			case OPTION_COMPILER_INEXTINFO:
				if (_externFile) {
					delete _externFile;
					_externFile = NULL;
				}
				if (!(opt->neg())) {
					_externFile = new FileInput();
                    _externFile->setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_EXTDATA, true);
					if (opt->size() > 0)
						_externFile->setLocSrc(opt->argPos(0));
					else
						_externFile->setLocSrc(opt->loc(true));
				}
				return true;

			case OPTION_COMPILER_SYNTAXXML:
				if (_syntaxXmlFile) {
					delete _syntaxXmlFile;
					_syntaxXmlFile = NULL;
				}
				if (!(opt->neg())) {
					_syntaxXmlFile = new FileOutput();
					_syntaxXmlFile->setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_SYNTAX, true);
					if (opt->size() > 0)
						_syntaxXmlFile->setLocSrc(opt->argPos(0));
					else
						_syntaxXmlFile->setLocSrc(opt->loc(true));
				}
				return true;

			case OPTION_COMPILER_CHECKONLY:
				_checkOnly = !opt->neg();
				return true;

			case OPTION_COMPILER_ALLOW_NEWPUBLIC:
				_allowNewPublic = !opt->neg();
				return true;

			case OPTION_COMPILER_ALLOW_NEWCONST:
				_allowNewConst = !opt->neg();
				return true;

			case OPTION_COMPILER_COMPAT_LINENAMEPREF:
		        _compatLineNamePref = !opt->neg();
				return true;

			case OPTION_COMPILER_COMPAT_CBSYMBOLS:
				_compatCBSymbols = !opt->neg();
				return true;

			case OPTION_COMPILER_COMPAT_CBHASSIGN:
				_compatCBHAssign = !opt->neg();
				return true;

			case OPTION_COMPILER_WARN_INSERTEOS:
				_warnCBInsertEOS = !opt->neg();
				return true;

            case OPTION_COMPILER_INT_OORANGE_MODE:
                switch ((*opt)[0][0]) {
                    case 'i':
                    case 'I':
                        _intOutOfRangeMode = INT_OPER_OORANGE_IGNORE;
                        break;
                    case 'e':
                    case 'E':
                        _intOutOfRangeMode = INT_OPER_OORANGE_ERROR;
                        break;
                    case 'c':
                    case 'C':
                        _intOutOfRangeMode = INT_OPER_OORANGE_CONVERT;
                        break;
                    default:
                        ERRHANDLER.error(ERROR_LVL_EASY, "unknown mode for handling of integer out of range", opt->argPos(0));
                }
                return true;

#if PROTO
			case OPTION_COMPILER_TRACESCANNER:
				_traceScanning = !opt->neg();
				return true;

			case OPTION_COMPILER_TRACEPARSER:
				_traceParsing = !opt->neg();
				return true;

			case OPTION_COMPILER_OUTCODE:
				if (_outputCode) {
					delete _outputCode;
					_outputCode = NULL;
				}
				if (!(opt->neg())) {
					_outputCode = new FileOutput();
					if (opt->size() > 0) {
						_outputCode->setFile(_data, IO_MODE_FILE, &((*opt)[0]), NULL, true);
						_outputCode->setLocSrc(opt->argPos(0));
					}
					else {
						_outputCode->setFile(_data, IO_MODE_STD_IO, NULL, NULL, true);
						_outputCode->setLocSrc(opt->loc(true));
					}
				}
				return true;
#endif
		}

		return false;
	}

	/**
	 * writes usage info for the module to stream
	 * @param s				stream to write to
	 */
	void Compiler::usage(ostream& s)
	{
		ModuleBase::usage(s);

		s << "  -i-pre <file>                 input precompiler result from <file>" << endl;
		s << "  -i-extern <file>              input extern data info from <file>" << endl;

		s << "  -syntax-xml [<file>]          output syntax structure of cmpl input as xml to <file>" << endl;
		s << "  -check-only                   only syntax check, no generating of intermediary code" << endl;

		s << "  -allow-new-public             allow new-definition when it permanently discards the previous symbol" << endl;
		s << "  -allow-new-const              allow new-definition when the previous symbol is const-defined" << endl;

		s << "  -compat-linenamepref          allow undefined name as line name prefix for compatibility" << endl;
		s << "  -compat-cbsymbols             allow undefined name as code block symbol for compatibility" << endl;
		s << "  -compat-cbh-assign            allow assign operator in code block header for compatibility" << endl;

		s << "  -warn-insert-eos              output warning if an end-of-statement token is inserted after a codeblock" << endl;

        s << "  -int-out-of-range             mode for handling integer values out of range (ignore/error/convert)" << endl;

#if PROTO
		s << "  -trace-scanner                output compiler scanner trace to stderr" << endl;
		s << "  -trace-parser                 output compiler parser trace to stderr" << endl;
		s << "  -o-code [<file>]              protocol output of generated intermediary code to <file> or to stdout" << endl;
#endif
	}

	/**
	 * run module function
	 */
	void Compiler::run()
	{
		PROTO_OUTL("Start run compiler module " << moduleName());

		_ctrl->errHandler().setExecStep("input files");

		// read files
		if (_inputFile) {
			readInputFile(*_inputFile);
			delete _inputFile;
			_inputFile = NULL;
		}

		if (_externFile) {
			readExternFile(*_externFile);
			delete _externFile;
			_externFile = NULL;
		}

		// if no cmpl input then nothing to do
		GET_DATA(CmplSourceInput, inp);
		if (inp && !(inp->contents().empty())) {
			// start parser
			_ctrl->errHandler().setExecStep("parser");
			CompilerContext context(this);
			context.parse();
		}

		// output syntax structure (in the "parser" exec step because it should be written also when errors occurred)
		if (_syntaxXmlFile) {
			writeSyntaxXmlFile(*_syntaxXmlFile);
			delete _syntaxXmlFile;
			_syntaxXmlFile = NULL;
		}

#if PROTO
		// protocol output of generated intermediary code (in the "parser" exec step because it should be written also when errors occurred)
		if (_outputCode) {
			writeCodeProtocol(*_outputCode);
			delete _outputCode;
			_outputCode = NULL;
		}
#endif

		// output files
		_ctrl->errHandler().setExecStep("output files");

		PROTO_OUTL("End run compiler module " << moduleName());
	}


	/****** error output ****/

	/**
	 * output error message and possible original exception; disable code generation if error.
	 * @param level			one of ERROR_LVL_*
	 * @param msg			error message
	 * @param loc			error location
	 * @param orgExc		original exception or NULL
	 * @param fatal			flag whether to throw FatalException / -1: according to level
	 */
    void Compiler::error(int level, const char *msg, const LocationInfo& loc, exception *orgExc, int fatal)
	{
		if (level > ERROR_LVL_WARN)
			_checkOnly = true;
		
		ctrl()->errHandler().error(level, msg, loc, orgExc, fatal);
	}

	/**
	 * output error message and possible original exception; disable code generation if error.
	 * @param level			one of ERROR_LVL_*
	 * @param msg			error message
	 * @param pos			error position
	 * @param orgExc		original exception or NULL
	 * @param fatal			flag whether to throw FatalException / -1: according to level
	 */
    void Compiler::error(int level, const char *msg, const PositionInfo& pos, exception *orgExc, int fatal)
	{
		if (level > ERROR_LVL_WARN)
			_checkOnly = true;
		
		ctrl()->errHandler().error(level, msg, pos, orgExc, fatal);
	}

	/**
	 * output two joined error messages; disable code generation if error.
	 * @param level			one of ERROR_LVL_*
	 * @param msg1			error message #1
	 * @param loc1			error location #1
	 * @param msg2			error message #2
	 * @param loc2			error location #2
	 */
	void Compiler::error(int level, const char *msg1, LocationInfo& loc1, const char *msg2, LocationInfo& loc2)
	{
		if (level > ERROR_LVL_WARN)
			_checkOnly = true;
		
		ctrl()->errHandler().error(level, msg1, loc1, msg2, loc2);
	}

	/**
	 * output internal fatal error without location information; disable code generation if error.
	 * @param msg			error message
	 * @param orgExc		original exception or NULL
	 */
	void Compiler::internalError(const char *msg, exception *orgExc)
	{
		_checkOnly = true;
		ctrl()->errHandler().internalError(msg, orgExc);
	}


	/************** input and output of data **********/

	/**
	 * read input file to data object
	 * @param file          file to read
	 */
	void Compiler::readInputFile(FileInput& file)
	{
		try {
			PROTO_OUTL("read compiler input file '" << file.fileNameReplaced() << "'");

			istream *inStr = file.open();

			GET_NEW_DATA(CmplSourceInput, inp);

			// read whole file
			getline(*inStr, inp->contents(), '\0');

			file.close();
		}
		catch (FileException& e) {
			error(ERROR_LVL_FATAL, _ctrl->printBuffer("%s: compiler input file '%s'", e.what(), e.filename().c_str()), file.locSrc());
		}
		catch (exception& ex) {
			error(ERROR_LVL_FATAL, "compiler input file error", file.locSrc(), &ex);
		}
	}

	/**
	 * read extern data info file to data object
	 * @param file          file to read
	 */
	void Compiler::readExternFile(FileInput& file)
	{
		try {
			PROTO_OUTL("read compiler extern data info file '" << file.fileNameReplaced() << "'");

			istream *inStr = file.open();

			GET_NEW_DATA(ExternDataInfo, edi);

			edi->readFrom(*inStr, _data);

			file.close();
		}
		catch (FileException& e) {
			error(ERROR_LVL_FATAL, _ctrl->printBuffer("%s: read compiler extern data info file '%s'", e.what(), e.filename().c_str()), file.locSrc());
		}
		catch (exception& ex) {
			error(ERROR_LVL_FATAL, "read compiler extern data info file error", file.locSrc(), &ex);
		}
	}

	/**
	 * write syntax structure info file as xml
	 * @param file			file to write
	 */
	void Compiler::writeSyntaxXmlFile(FileOutput& file)
	{
		try {
			PROTO_OUTL("write syntax structure xml info file '" << file.fileNameReplaced() << "'");

			ostream *ostr = file.open();

			GET_DATA(SyntaxElements, se);
			if (se)
				se->writeXML(*ostr, 4);

			file.close();
		}
		catch (FileException& e) {
			error(ERROR_LVL_NORMAL, _ctrl->printBuffer("%s: write syntax structure xml info file '%s'", e.what(), e.filename().c_str()), file.locSrc());
		}
		catch (exception& ex) {
			error(ERROR_LVL_NORMAL, "write syntax structure xml info file error", file.locSrc(), &ex);
		}
	}

	/**
	 * protocol output of generated intermediary code
	 * @param file			file to write
	 */
	void Compiler::writeCodeProtocol(FileOutput& file)
	{
		try {
			PROTO_OUTL("protocol output of generated intermediary code to '" << file.fileNameReplaced() << "'");

			ostream *ostr = file.open();

			GET_DATA(IntCode, code);
			if (code) {
				*ostr << "Start output of generated intermediary code" << endl;
				for (int i = 0; i < code->subCnt(); i++)
					code->at(i)->protoOut(this, i, ostr);
				*ostr << "End output of generated intermediary code" << endl;
			}

			file.close();
		}
		catch (FileException& e) {
			error(ERROR_LVL_NORMAL, _ctrl->printBuffer("%s: protocol output of generated intermediary code to '%s'", e.what(), e.filename().c_str()), file.locSrc());
		}
		catch (exception& ex) {
			error(ERROR_LVL_NORMAL, "protocol output of generated intermediary code error", file.locSrc(), &ex);
		}
	}
}


