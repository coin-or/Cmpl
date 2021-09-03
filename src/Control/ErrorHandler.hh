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


#ifndef ERRORHANDLER_HH
#define ERRORHANDLER_HH

#include <string>

#include "LocationInfo.hh"


using namespace std;


namespace cmpl
{
	class MainControl;
	class FileOutput;


// possible values for ErrorHandler::_errorLevel
#define ERROR_LVL_WARN		1			///< warning
#define ERROR_LVL_EASY		2			///< continue execution (but other modules may decide not to run after such an error)
#define ERROR_LVL_NORMAL	3			///< after such an error try to finish the current _execStep in the current module to check for more errors, but then cancel execution of the current module
#define ERROR_LVL_CANCEL	4			///< cancel execution of the current module (most other modules should decide not to run after such an error)
#define ERROR_LVL_FATAL		5			///< cancel execution of the whole program


// max error counts before giving up
#define MAX_ERROR_COUNT		100			///< after this error count throw an error ERROR_LVL_CANCEL / -1: never
#define MAX_WARN_COUNT		100			///< after this warning count suppress warnings / -1: never


    /**
     * <code>ErrorHandler</code> implements the error handling.
	 * It exists only one instance of this class.
     */
    class ErrorHandler
    {

    private:
		MainControl *_ctrl;				///< main object of execution

		FileOutput *_errOut;		///< info for error output / NULL: output to cerr
		const char **_errorLevelName;	///< names of ERROR_LVL_*

        FileOutput *_cmplMsgOut;

		const char *_curModule;			///< name of current executed module
		const char *_execStep;			///< current step of execution within the module

        //int _errCnt;					///< error count
		//int _warnCnt;					///< warning count


    public:
        /**
         * constructor
         */
        ErrorHandler();

        /**
         * destructor
         */
        ~ErrorHandler();

		/**
		 * initialize
		 * @param ctrl          main object
		 */
		void init(MainControl *ctrl);


		/**
		 * sets info for error output
		 */
		inline void setErrOut(FileOutput *errOut)											{ _errOut = errOut; }


        /**
         * sets info for cmplMsg output
         */
        inline void setCmplMsgOut(FileOutput *cmplMsgOut)				{ _cmplMsgOut = cmplMsgOut; }


        /**
         * write error messages to cmplMsg file
         */
        void writeCmplMsg();


		/**
		 * sets the current module
		 * @param curModule			name of current executed module
		 */
		inline void setCurModule(const char *curModule)										{ setCurModule(curModule, "init", ERROR_LVL_FATAL); }

		/**
		 * sets the current exec step
		 * @param execStep			current step of execution within the module
		 * @param except			throw module exception, if any error at least of this error level occured up to here
		 */
		inline void setExecStep(const char *execStep, int except = ERROR_LVL_NORMAL)		{ setCurModule(NULL, execStep, except); }

		/**
		 * get the name of current module
		 */
		inline const char *curModule() const												{ return _curModule; }

		/**
		 * get the name of the current exec step
		 */
		inline const char *execStep() const													{ return _execStep; }


		/****** error output ****/

		/**
		 * output error message and possible original exception.
		 * @param level			one of ERROR_LVL_*
		 * @param cont			error entry continues the previous entry
		 * @param msg			error message
		 * @param loc			error location
		 * @param orgExc		original exception or NULL
		 * @param fatal			flag whether to throw FatalException / -1: according to level
		 */
		void error(int level, bool cont, const char *msg, const LocationInfo& loc, exception *orgExc = NULL, int fatal = -1);

		/**
		 * output error message and possible original exception.
		 * @param level			one of ERROR_LVL_*
		 * @param cont			error entry continues the previous entry
		 * @param msg			error message
		 * @param pos			error position
		 * @param orgExc		original exception or NULL
		 * @param fatal			flag whether to throw FatalException / -1: according to level
		 */
		void error(int level, bool cont, const char *msg, const PositionInfo& pos, exception *orgExc = NULL, int fatal = -1)		{ LocationInfo loc(pos); error(level, cont, msg, loc, orgExc, fatal); }

		/**
		 * output error message and possible original exception.
		 * @param level			one of ERROR_LVL_*
		 * @param msg			error message
		 * @param loc			error location
		 * @param orgExc		original exception or NULL
		 * @param fatal			flag whether to throw FatalException / -1: according to level
		 */
		void error(int level, const char *msg, const LocationInfo& loc, exception *orgExc = NULL, int fatal = -1)					{ error(level, false, msg, loc, orgExc, fatal); }

		/**
		 * output error message and possible original exception.
		 * @param level			one of ERROR_LVL_*
		 * @param msg			error message
		 * @param pos			error position
		 * @param orgExc		original exception or NULL
		 * @param fatal			flag whether to throw FatalException / -1: according to level
		 */
		void error(int level, const char *msg, const PositionInfo& pos, exception *orgExc = NULL, int fatal = -1)					{ LocationInfo loc(pos); error(level, false, msg, loc, orgExc, fatal); }

		/**
		 * output two joined error messages
		 * @param level			one of ERROR_LVL_*
		 * @param msg1			error message #1
		 * @param loc1			error location #1
		 * @param msg2			error message #2
		 * @param loc2			error location #2
		 */
		void error(int level, const char *msg1, const LocationInfo& loc1, const char *msg2, const LocationInfo& loc2)				{ error(level, false, msg1, loc1); error(level, true, msg2, loc2); }

		/**
		 * output three joined error messages
		 * @param level			one of ERROR_LVL_*
		 * @param msg1			error message #1
		 * @param loc1			error location #1
		 * @param msg2			error message #2
		 * @param loc2			error location #2
		 * @param msg3			error message #3
		 * @param loc3			error location #3
		 */
		void error(int level, const char *msg1, const LocationInfo& loc1, const char *msg2, const LocationInfo& loc2, const char *msg3, const LocationInfo& loc3)
																													{ error(level, false, msg1, loc1); error(level, true, msg2, loc2); error(level, true, msg3, loc3); }

		/**
		 * output internal fatal error without location information.
		 * @param msg			error message
		 * @param orgExc		original exception or NULL
		 */
		void internalError(const char *msg, exception *orgExc = NULL)												{ LocationInfo loc(PositionInfo(POSITION_TYPE_DESCR, "(internal)")); error(ERROR_LVL_FATAL, false, msg, loc, orgExc); }


	
	private:
		/**
		 * opens the output stream for an error
		 * @return		stream for writing
		 */
		ostream* openOutput();

		/**
		 * close the output stream for an error
		 */
		void closeOutput();

		/**
		 * sets the current module and exec step
		 * @param curModule			name of current executed module / NULL: don't set module
		 * @param execStep			current step of execution within the module
		 * @param except			throw module exception, if any error at least of this error level occured up to here
		 */
		void setCurModule(const char *curModule, const char *execStep, int except);
    };


    /**
     * <code>ModuleException</code> is the exception thrown to cancel execution of the current module
     */
	class ModuleException : public exception
	{
	public:
		/**
		 * destructor
		 */
		virtual ~ModuleException() throw()		{ }

		/**
		 * gets exception info
		 */
		virtual const char *what() const throw()				{ return "cancel execution of the current module"; }
	};


    /**
     * <code>FatalException</code> is the exception thrown to cancel the whole program after a fatal error is handled by <code>ErrorHandler</code>.
     */
	class FatalException : public exception
	{
	private:
		int _exitCode;					///< exit code
		string _what;					///< exception info
		const char *_curModule;			///< name of current executed module
		const char *_execStep;			///< current step of execution within the module

	public:
        /**
         * constructor
		 * @param exitCode			exit code
		 * @param curModule			name of current executed module
		 * @param execStep			current step of execution within the module
		 * @param orgExc			what() from original exception
         */
		FatalException(int exitCode, const char *curModule, const char *execStep = NULL, const char *orgExc = NULL);

        /**
         * copy constructor
         * @param ex                source exception
         */
        FatalException(const FatalException& ex): _exitCode(ex._exitCode), _what(ex._what), _curModule(ex._curModule), _execStep(ex._execStep)    { }

		/**
		 * destructor
		 */
        virtual ~FatalException() throw()                   { }

        /**
         * gets exit code
         */
        inline int exitCode()                               { return _exitCode; }

		/**
		 * gets exception info
		 */
		virtual const char *what() const throw()			{ return _what.c_str(); }
	};
}

#endif // ERRORHANDLER_HH
