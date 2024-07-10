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

#include "SolverCbc.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/Solution.hh"

#include <iomanip>

namespace cmpl
{
/*********** module definition **********/

// defines the module "SolveCbc" that is implemented in class "SolveCbc".
// to register this module you have to insert "MODULE_CLASS_REG(1, SolverCbc)" to file "modules.reg"
MODULE_CLASS( solverCbc, SolverCbc )



/*********** initialization **********/

/**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
void SolverCbc::init(MainControl *ctrl, MainData *data, const char *name)
{
    _solverModule="CBC";
    Solver::init(ctrl, data, name);

}


/*********** handling of command line options **********/




/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void SolverCbc::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
{
    Solver::regModOptions(modOptReg);

}

/**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
bool SolverCbc::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (Solver::parseOption(ref, prio, opt))
        return true;

    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void SolverCbc::usage(ostream& s)
{
    Solver::usage(s);

}

/**
     * run module function
     */
void SolverCbc::run()
{
    GET_DATA(OptModel,om);


    if (_solverName=="CBC" && !om->exportOnly() && om->cols().size()>0) {

        _ctrl->errHandler().setExecStep("run");

        if (!om->isLinearModel())
            if (om->modelProp().hasConditions())
                _ctrl->errHandler().internalError("CBC cannot solve a nonlinear model"  );

        PROTO_OUTL("Start SolverCbc module " << moduleName());

        setBinFullName();

        PROTO_OUTL("SolverCbc: writing instance file " << moduleName());
        writeInstanceFile(" -mps-format-sos cplex ");

        PROTO_OUTL("SolverCbc: solving instance" << moduleName());

        GET_NEW_DATA(Solution,sol);

        string probName = StringStore::modelName(string( modp()->data()->cmplFileBase() ))+".cmpl";
        sol->prepareSolutionData(probName, _solverName, _integerRelaxation,  _addConForUnusedVar, _data,this);

        generateCmdLine(sol);

        solve();

        PROTO_OUTL("SolverCbc: finished solving  and reading solution" << moduleName());

        readSolFile(sol);

        deleteTmpFiles();
        PROTO_OUTL("End SolverCbc module " << moduleName());
    }

}

/**
      * @brief generates the the solver specific command line for the solver
      * @param sol   pointer to Solution object
      */

void SolverCbc::generateCmdLine(Solution* sol) {
    _solverCmdLine="\""+_solverBinName+"\" "+ _instanceFileName;

    for (size_t i=0; i<_solverOpts.size(); i++)
        _solverCmdLine+= " -" + _solverOpts[i].key + " " + _solverOpts[i].value;

    _solverCmdLine+= " -" + sol->objSense() + " -solve -gsolu "+ _instanceSolName + " 2>&1";
}



/**
     * @brief reads the solver specific solution file
     * @param sol   pointer to Solution object
     */
void SolverCbc::readSolFile(Solution *sol) {

    unsigned long lineNr;
    string line;

    bool varSection;
    bool conSection;
    bool headerSection;

    unsigned headerLines;

    lineNr=0;
    headerSection=true;
    conSection=false;
    varSection=false;

    unsigned long varIdx=0;
    unsigned long conIdx=0;

    CmplOutput(cout, "Retrieving solution" );

    try {

        ifstream  solFile( _instanceSolName, ifstream::binary) ;

        if (!solFile.is_open())
            _ctrl->errHandler().internalError(_ctrl->printBuffer("Cannot read solution file '%s'", _instanceSolName.c_str() )  ,NULL);

        SingleSolution solution;

        while ( getline( solFile, line) ) {

            lineNr++;

            line=StringStore::lrTrim(line);
            vector<string> solList;

            StringStore::split(line,solList);

            if (headerSection) {
                if (lineNr==1) {
                    if (_solverName=="GLPK") {
                        headerLines=2;
                    } else {
                        headerLines=3;
                    }

                    continue;
                }

                if (lineNr==2) {

                    if (solList[0]=="2" || solList[0]=="5"  ) {
                        sol->setNrOfSolutions(1);
                        sol->setStatus("normal");
                        solution.setStatus("optimal");

                    } else if (solList[0]=="1") {
                        sol->setNrOfSolutions(0);
                        sol->setStatus("undefined");
                    } else {
                        sol->setNrOfSolutions(0);
                        sol->setStatus("infeasible");
                    }

                    if (sol->nrOfSolutions()>0) {

                        int valPos=0;
                        if (solList.size()==3) {
                            valPos=2;
                        } else {
                            valPos=1;
                        }

                        double objVal;
                        if (!StringStore::toDouble(solList[valPos],objVal))
                            _ctrl->errHandler().internalError("Internal error while reading obValue" ,NULL);

                        solution.setValue(objVal);

                    } else {
                        break;
                    }
                    continue;
                }

                if (lineNr>headerLines) {
                    headerSection=false;
                    varSection=false;
                    conSection=true;
                }
            }

            if ( conSection || varSection ) {

                SolutionElement solElem;
                solElem.setMarginal(0);

                double activity=0;
                double marginal=0;

                int aPos=0;
                int mPos=0;

                if (solList.size()==1) {
                    aPos=0;
                    mPos=-1;

                } else if (solList.size()==2) {
                    aPos=0;
                    mPos=1;

                } else if (solList.size()==3) {
                    aPos=1;
                    mPos=2;
                }

                if (!StringStore::toDouble(solList[aPos],activity))
                    _ctrl->errHandler().internalError("Internal error while reading activity" ,NULL);

                if (mPos>0) {
                    if (!StringStore::toDouble(solList[mPos],marginal))
                        _ctrl->errHandler().internalError("Internal error while reading mariginal" ,NULL);
                }

                solElem.setActivity(activity);
                solElem.setMarginal(marginal);

                if (conSection) {
                    solElem.setModelElement(sol->modelConstraint(conIdx));
                    solution.setConstraint(solElem);
                    conIdx++;
                } else {
                    solElem.setModelElement(sol->modelVariable(varIdx));

                    if (sol->modelVariable(varIdx)->type()!="C") {
                        activity=round(activity);
                        solElem.setActivity(activity);
                    }

                    solution.setVariable(solElem);
                    varIdx++;
                }

                if (lineNr>headerLines+sol->nrOfConstraints()-1) {
                    headerSection=false;
                    varSection=true;
                    conSection=false;
                }
                continue;
            }
        }
        sol->setSolution(solution);

    }
    catch (FileException& e) {
        _ctrl->errHandler().internalError(_ctrl->printBuffer("%s: solution file '%s'", e.what(), _instanceSolName.c_str()) ,&e);
    }
}


}
