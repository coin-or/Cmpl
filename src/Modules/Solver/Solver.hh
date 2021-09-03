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

#ifndef SOLVER_HH
#define SOLVER_HH


#include "../../Control/ModuleBase.hh"
#include "../../CommonData/OptModel.hh"


namespace cmpl
{



#define SOLVER_UNDEFINED 99

#define OPT_FILE_OK 100;
#define OPT_FILE_FAIL 101;

#define SOLVER_OK 102;
#define SOLVER_FAIL 103;


#if defined(_WIN32) || defined(WIN32)
    #define POPEN(f,a) = _popen(f,a)
    #define PCLOSE(f,a) = _popen(p)
#else
    #define POPEN(f,a) = popen(f,a)
    #define PCLOSE(f,a) = popen(p)
#endif



class Solver  : public ModuleBase
{


public:


    /**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
    virtual void init(MainControl *ctrl, MainData *data, const char *name);


public:

    /**
     * @brief Returns true if the solver is implemented
     * @param SolverName    solver to be checked
     * @return
     */
    bool isImplemented(string SolverName);

protected:
    /**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
    virtual void regModOptions(vector<CmdLineOptList::RegOption> &modOptReg);

    /**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
    virtual bool parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt);

    /**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
    virtual void usage(ostream& s);


protected:
    //bool _isActive;

    string _solverModule;

    string _solverName;
    map<string,string>   _solverBinNames;
    string _solverBinName;
    //int _solverStatus;

    vector <string> _implementedSolvers;

    string _instanceBaseName;
    string _instanceFileName;
    string _instanceSolName;
    string _instanceCmdName;

    string _optFileName;

    string _objName;
    string _objSense;

    struct solverOption {
        string key;
        string value;
    };

    bool _solutionPool;

    vector <solverOption> _solverOpts;

    string _solverCmdLine;

    bool _integerRelaxation;

    /**
     * @brief reads the Cmpl solver option file
     */
    void readOptFile();

    /**
     * @brief sets the full path to the solver binary
     */
    void setBinFullName();

    /**
     * @brief writes the generated Mps into the tmp folder
     */
    void writeInstanceFile(string opts);

    /**
     * @brief starts the solver as external process
     * @return          status of the solver
     */
    int solve();

    /**
     * @brief deletes all generated files in the tmp folder
     */
    void deleteTmpFiles();

    /**
     * @brief Neccessary to setting up the binary
     */
    void replaceFullBinName() ;




private:

    void getSolverNamePath(const string& str, string& name, string& path);



};

}

#endif // SOLVER_HH
