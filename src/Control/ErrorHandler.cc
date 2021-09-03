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


#include "ErrorHandler.hh"
#include "FileBase.hh"
#include "MainControl.hh"
#include "MainData.hh"


namespace cmpl
{
	/* **************** initialization ************* */

	/**
	 * constructor
	 */
	ErrorHandler::ErrorHandler()
	{
		_errOut = NULL;
        _cmplMsgOut = NULL;

		_curModule = NULL;
		_execStep = NULL;

		_errorLevelName = new const char*[6] {
			"none",
			"warning",
			"error",
			"error",
			"error",
			"fatal error",
		};

	}

	/**
	 * destructor
	 */
	ErrorHandler::~ErrorHandler()
	{
		delete _errorLevelName;
	}

	/**
	 * initialize
	 * @param ctrl			main object
	 */
	void ErrorHandler::init(MainControl *ctrl)
	{
		_ctrl = ctrl;
		_ctrl->data()->resetErrors();
	}


    /* **************** error output ************* */

	/**
	 * output error message and possible original exception.
	 * @param level         one of ERROR_LVL_*
	 * @param cont			error entry continues the previous entry
	 * @param msg           error message
	 * @param loc           error location
	 * @param orgExc        original exception or NULL
	 * @param fatal         flag whether to throw FatalException / -1: according to level
	 */
	void ErrorHandler::error(int level, bool cont, const char *msg, const LocationInfo& loc, exception* orgExc, int fatal)
	{

        try {
            LockGlobalGuard(true, LockGlobalGuard::errLock);

            if (!cont && level == ERROR_LVL_WARN && MAX_WARN_COUNT >= 0 && _ctrl->data()->warnCnt() > MAX_WARN_COUNT)
                return;

            _ctrl->data()->addError(level, cont, msg, orgExc, _curModule, _execStep, loc);

            if (_errOut || (!_errOut && !_cmplMsgOut) || ((_errOut || _cmplMsgOut) && ERROR_LVL_FATAL)) {
                LockGlobalGuard(true, LockGlobalGuard::coutLock);
                ostream *oOut = openOutput();
    
                if (!cont) {
                    *oOut << _errorLevelName[level] << " (module " << _curModule;
                    if (_execStep)
                        *oOut << ": " << _execStep;
                    *oOut << "): ";
                }
                else {
                    *oOut << "                ";
                }

                *oOut << msg;
                if (orgExc)
                    *oOut << ": " << orgExc->what();

                *oOut << "; at " << loc << endl;

                if (!cont) {
                    if (level == ERROR_LVL_WARN) {
                        if (MAX_WARN_COUNT >= 0 && _ctrl->data()->warnCnt() > MAX_WARN_COUNT) {
                            *oOut << "to much warnings, suppress further warnings" << endl;
                        }
                    }
                    else {
                        if (MAX_ERROR_COUNT >= 0 && _ctrl->data()->errorCnt() > MAX_ERROR_COUNT) {
                            *oOut << "to much errors, giving up" << endl;

                            if (level < ERROR_LVL_CANCEL)
                                level = ERROR_LVL_CANCEL;

                            if (_ctrl->data()->errorLevelMax() < ERROR_LVL_CANCEL)
                                _ctrl->data()->setErrorLevelMax(ERROR_LVL_CANCEL);
                        }
                    }
                }

                closeOutput();
            }
        }
        catch (exception& e) {
            LockGlobalGuard(true, LockGlobalGuard::coutLock);
            cerr << "Fatal cmpl error: Exception in error handling: " << e.what() << endl;
            level = ERROR_LVL_FATAL;
        }

        if (fatal > 0 || (fatal < 0 && level == ERROR_LVL_FATAL))
            throw FatalException(100, _curModule, _execStep, (orgExc ? orgExc->what() : NULL));

        if (level >= ERROR_LVL_CANCEL)
            throw ModuleException();
	}


    /**
     * write error messages to cmplMsg file
     */
    void ErrorHandler::writeCmplMsg() {

        if (_cmplMsgOut) {

            try {

                _ctrl->CmplOutput(cout,"Writing CmplMessages to file > "+ _cmplMsgOut->fileNameReplaced()  );
                ostream *oOut = _cmplMsgOut->open();


                *oOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
                *oOut << "<CmplMessages version=\"1.2\">" << endl;

                *oOut << "\t<general>"<< endl;

                *oOut << "\t\t<instanceName>" << _ctrl->modp()->data()->cmplFileBase()  << ".cmpl</instanceName>" << endl;

                if (_ctrl->data()->errorCnt() > 0) {
                    *oOut <<"\t\t<generalStatus>error</generalStatus>"<< endl;
                    *oOut << "\t\t<message>cmpl finished with errors</message>" << endl;
                }
                else if (_ctrl->data()->warnCnt() > 0){
                    *oOut << "\t\t<generalStatus>warning</generalStatus>" << endl;
                    *oOut << "\t\t<message>cmpl finished with warnings</message>" << endl;
                }
                else {
                    *oOut << "\t\t<generalStatus>normal</generalStatus>" << endl;
                    *oOut << "\t\t<message>cmpl finished normal</message>" << endl;
                }

                *oOut << "\t\t<cmplVersion>" << CMPL_VERSION << "</cmplVersion>" << endl;
                *oOut << "\t</general>" << endl;

                if ( (_ctrl->data()->errorCnt() + _ctrl->data()->warnCnt())  > 0)
                {

                    *oOut << "\t<messages numberOfMessages=\""<< (_ctrl->data()->errorCnt() + _ctrl->data()->warnCnt()) << "\">" << endl;


                    for (MainData::ErrorEntry e: *_ctrl->data()->errorList())
                    {
                        *oOut << "\t\t<message";

                        *oOut << " type =\"" << _errorLevelName[e.lvl] <<"\"";

                        *oOut << " module =\"";
                        if (!e.cont)
                            *oOut << e.module <<"\"";
                        else
                            *oOut << "\"";

                        *oOut << " location=\"" << *e.loc << "\"";

                        string msgString = e.msg;
                        msgString = StringStore::encodeXml(msgString);

                        *oOut << " description=\"" << msgString << "\"";

                        *oOut << "/>" << endl;
                    }

                    *oOut << "\t</messages>" << endl;
                }

                *oOut << "</CmplMessages>" << endl;
                _cmplMsgOut->close();


            }

            catch (exception& e) {
                cerr << "Fatal cmpl error: Exception in error handling: " << e.what() << endl;
            }
        }
    }





    /* **************** internal functions ************* */

	/**
	 * opens the output for an error
	 * @return      stream for writing
	 */
    ostream* ErrorHandler::openOutput()
	{
		try {
			if (_errOut)
                return _errOut->open(ios_base::app);
		}
		catch (FileException& e) {
			cerr << e.what() << ": error output file '" << e.filename() << "': using stderr instead";
		}
		catch (exception& e) {
			cerr << "error: open error output file: " << e.what() << ": using stderr instead";
		}

		return &cerr;
	}

	/**
	 * close the output for an error
	 */
	void ErrorHandler::closeOutput()
	{
		if (_errOut)
			_errOut->close();
		else
			cerr.flush();
	}

	/**
	 * sets the current module and exec step
	 * @param curModule         name of current executed module / NULL: don't set module
	 * @param execStep          current step of execution within the module
	 * @param except            throw exception, if any error at least of this error level occured up to here
	 */
	void ErrorHandler::setCurModule(const char *curModule, const char *execStep, int except)
	{
		if (_ctrl->data()->errorLevelMax() >= except)
			throw ModuleException();

		if (curModule)
			_curModule = curModule;

		_execStep = execStep;
	}


	/* **************** FatalException ************* */

	/**
	 * constructor
	 * @param exitCode          exit Code
	 * @param curModule         name of current executed module
	 * @param execStep          current step of execution within the module
	 * @param orgExc            what() from original exception
	 */
	FatalException::FatalException(int exitCode, const char *curModule, const char *execStep, const char *orgExc)
	{
		_exitCode = exitCode;
		_curModule = curModule;
		_execStep = execStep;

		if (orgExc && *orgExc) {
			string s1("cmpl fatal exception: ");
			_what = s1 + orgExc;
		}
		else {
			_what = "cmpl fatal exception";
		}
	}
}

