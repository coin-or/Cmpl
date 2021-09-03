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

#include "Solver.hh"

#include "../../Control/MainControl.hh"
#include "../../Control/FileBase.hh"

#include "../../Control/MainData.hh"
#include "../../CommonData/OptModel.hh"

#include <cstdlib>
#include <iomanip>
#include <cstdio>



namespace cmpl
{

/**
 * @brief initialize modul, to call immediately after the constructor.
 * @param ctrl  pointer to MainControl object
 * @param data  pointer to MainData object
 * @param name  module name (or alias)
 */
void Solver::init(MainControl *ctrl, MainData *data, const char *name)
{
    ModuleBase::init(ctrl, data, name);

    _solverName="CBC";
    _solverBinName = "";

#ifdef _WIN32
    _instanceBaseName = FileBase::getTmpPath()+FileBase::getTmpFileName("_cmpl_",100000);

#else
    char tmpFilename[L_tmpnam];
    std::tmpnam(tmpFilename);
    _instanceBaseName = tmpFilename;
#endif

    _instanceFileName = _instanceBaseName+".mps";
    _instanceSolName = _instanceBaseName+".sol";
    _instanceCmdName = _instanceBaseName+".cmd";

    _solverCmdLine="";

    _objName="";
    _objSense="";

    _optFileName = "cmpl.opt";

    _solutionPool=false;

    _integerRelaxation=false;

    readOptFile();

}


#define OPTION_SOLVER_SOLVERNAME	10
#define OPTION_SOLVER_OPTION        20
#define OPTION_SOLVER_DISPLAY       30
#define OPTION_SOLVER_INTEGERRELAXATION          40



/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void Solver::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
{
    ModuleBase::regModOptions(modOptReg);



    REG_CMDL_OPTION( OPTION_SOLVER_SOLVERNAME, "solver", 1, 1, CMDL_OPTION_NEG_NO_ARG, false );

    REG_CMDL_OPTION( OPTION_SOLVER_OPTION, "opt", 2, 200, CMDL_OPTION_NEG_NO_ARG, false );

    REG_CMDL_OPTION( OPTION_SOLVER_DISPLAY, "display", 1, 200, CMDL_OPTION_NEG_NO_ARG, false );

    REG_CMDL_OPTION( OPTION_SOLVER_INTEGERRELAXATION, "int-relax", 0, 0, CMDL_OPTION_NEG_DELIV, false );



}

/**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
bool Solver::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (ModuleBase::parseOption(ref, prio, opt))
        return true;

    switch (ref) {

    case OPTION_SOLVER_DISPLAY:
        if (!opt->neg()) {
            string dispOpt = (*opt)[0];
            if (StringStore::upperCase(dispOpt)=="SOLUTIONPOOL")
                _solutionPool=true;
        }

        return true;

    case OPTION_SOLVER_SOLVERNAME:
        if (!opt->neg()) {
            string solverName = (*opt)[0];
            _solverName=StringStore::upperCase(solverName);
            if (!isImplemented(_solverName))
                _ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("Solver '%s' is not supported", solverName.c_str()), opt->loc(true) );
        }

        return true;

    case  OPTION_SOLVER_OPTION:
        if (!opt->neg()) {
            string solverName = (*opt)[0];
            solverName = StringStore::lrTrim(StringStore::upperCase(solverName));

            if (solverName==_solverModule) {

                for (size_t i=1; i<opt->size();i++) {
                    string option=(*opt)[i];
                    vector <string> optList;
                    StringStore::split(option,optList,"=");
                    solverOption solOpt;
                    solOpt.key=optList[0];
                    if (optList.size()>1)
                        solOpt.value=optList[1];
                    else
                        solOpt.value="";
                   _solverOpts.push_back(solOpt);
                }
            }
        }
        return true;

    case OPTION_SOLVER_INTEGERRELAXATION:
        _integerRelaxation=true;
        return true;
    }
    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void Solver::usage(ostream& s)
{
    ModuleBase::usage(s);

    s << "  -solver <solverName>                       specifies the Solver to be invoked" << endl;
    s << "  -opt <solverName> [<option>[=<val>] ...]   specifies the Solver to be invoked" << endl;
}


/**
* @brief reads the Cmpl solver option file
*/
void Solver::readOptFile() {
    char dirSepChar = (_ctrl->binFullName() ? FileBase::getDirSepChar(_ctrl->binFullName()->c_str()) : '\0');
    string fileName(FileBase::replFileInPath(_ctrl->binFullName(), &_optFileName, dirSepChar));

    try {
        FileInput inFile;
        istream *inStr = NULL;

        inFile.setFile(_data, IO_MODE_FILE, &fileName, NULL, true);
        inStr = inFile.open();

        int line = 0;
        string lineStr, s;

        while (getline(*inStr, lineStr)) {
            line++;

            if (!StringStore::startsWith(StringStore::lTrim(lineStr),"#") && (!StringStore::lrTrim(lineStr).empty())) {

                string solver, solverPath;
                getSolverNamePath(lineStr, solver ,solverPath);
                solver=StringStore::upperCase(solver);

                _implementedSolvers.push_back(solver);
                _solverBinNames[solver]=solverPath;
            }
        }
        inFile.close();
    }
    catch (FileException& e) {
        _ctrl->errHandler().internalError(_ctrl->printBuffer("%s: option file '%s'", e.what(), fileName.c_str()) ,&e);
    }
}

/**
 * @brief sets the full path to the solver binary
 */
void Solver::setBinFullName(){
    if (!isImplemented(_solverName))
        _ctrl->errHandler().internalError( _ctrl->printBuffer("Solver '%s' is not supported", _solverName.c_str()) );

    _solverBinName=_solverBinNames[_solverName];
    replaceFullBinName();
}

/**
 * @brief Returns true if the solver is implemented
 * @param SolverName    solver to be checked
 * @return
 */
bool Solver::isImplemented(string solverName){
    return (find(_implementedSolvers.begin(), _implementedSolvers.end(), solverName) != _implementedSolvers.end() );
}

/**
 * @brief writes the generated Mps into the tmp folder
 */
void Solver::writeInstanceFile(string opts) {
    deleteTmpFiles();

    CmdLineOptList clOpts;
    PositionInfo pos(POSITION_TYPE_DESCR, "(internal)");

    opts += _ctrl->printBuffer(" -fms %s", _instanceFileName.c_str());

    if (_isSilent)
        opts += "  -silent";

    if (_integerRelaxation)
        opts += " -int-relax";

    clOpts.addOption(opts, pos);
    _ctrl->runModuleFunc("writeMps", &clOpts);

    if (!FileBase::exists(_instanceFileName)) {
        _ctrl->errHandler().internalError( _ctrl->printBuffer("MPS file %s does not exist", _instanceFileName.c_str())  );
    }

}

/**
 * @brief deletes all generated files in the tmp folder
 */
void Solver::deleteTmpFiles() {
    if (FileBase::exists(_instanceFileName))
        remove(_instanceFileName.c_str());

    if (FileBase::exists(_instanceSolName))
        remove(_instanceFileName.c_str());

    if (FileBase::exists(_instanceCmdName))
        remove(_instanceCmdName.c_str());
}

/**
 * @brief Neccessary to setting up the binary
 */
void Solver::replaceFullBinName() {
    char dirSepChar = (_ctrl->binFullName() ? FileBase::getDirSepChar(_ctrl->binFullName()->c_str()) : '\0');

    string sep;
    sep.push_back(dirSepChar);

    if (!StringStore::startsWith(_solverBinName,sep)  && !(_solverBinName.substr(1,1)==":"))
        _solverBinName= FileBase::replFileInPath(_ctrl->binFullName(), &_solverBinName, dirSepChar);
}

/**
 * @brief starts the solver as external process
 * @return          status of the solver
 */
int Solver::solve() {

    int buffSize = 1024;
    char buffer[buffSize];
    int ret;

    //cout << _solverBinName << endl;
    if (!FileBase::exists(_solverBinName)) {
        _ctrl->errHandler().internalError( _ctrl->printBuffer("Binary for solver %s does not exist: '%s'", _solverName.c_str(), _solverBinName.c_str()) );
    }

    if (_solverCmdLine.empty()) {
        _ctrl->errHandler().internalError(_ctrl->printBuffer("Internal error, no cmd line for solver '%s' specified", _solverName.c_str())  );
    }

    CmplOutput(cout, "Solving instance using " + _solverName   );

#if defined(_WIN32) || defined(WIN32)
    _solverCmdLine="\""+_solverCmdLine+"\"";
#endif

    try {
        FILE* pipe = popen(_solverCmdLine.c_str(), "r");

        if (!pipe)
        {
            _ctrl->errHandler().internalError(_ctrl->printBuffer("Cannot execute solver '%s'", _solverName.c_str()) );
        }

        while (fgets(buffer, 1024, pipe) != NULL) {
            if (!_isSilent) {
                cout << buffer ;//<< endl;
                cout << flush ;
            }

        }

        if (!_isSilent)
            cout << endl <<endl;;

        ret=pclose(pipe);

        if (ret==-1) {
            _ctrl->errHandler().internalError(_ctrl->printBuffer("Solver '%s' finished with failures", _solverName.c_str()));
        }

    } catch (exception const& e) {
        _ctrl->errHandler().internalError(_ctrl->printBuffer("Solver '%s' finished with failures: %s", _solverName.c_str(), e.what()));
    }

    return ret;
}

void Solver::getSolverNamePath(const string& str, string& name, string& path) {
    size_t pos = str.find_first_of(WHITE_SPACES);

    if (pos==std::string::npos)
        _ctrl->errHandler().internalError(_ctrl->printBuffer("Internal error in cmpl.opt for entry <'%s'>", str.c_str())  );

    name = str.substr(0,pos);
    path = str.substr(pos+1);

    name = StringStore::lrTrim(name);
    path =StringStore::lrTrim(path);
}


}
