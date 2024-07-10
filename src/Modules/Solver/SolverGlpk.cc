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

#include "SolverGlpk.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/Solution.hh"


namespace cmpl
{
/*********** module definition **********/

MODULE_CLASS( solverGlpk, SolverGlpk )



/*********** initialization **********/

/**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
void SolverGlpk::init(MainControl *ctrl, MainData *data, const char *name)
{
    _solverModule="GLPK";
    Solver::init(ctrl, data, name);

}


/*********** handling of command line options **********/




/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void SolverGlpk::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
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
bool SolverGlpk::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (Solver::parseOption(ref, prio, opt))
        return true;

    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void SolverGlpk::usage(ostream& s)
{
    Solver::usage(s);

}

/**
     * run module function
     */
void SolverGlpk::run()
{
    GET_DATA(OptModel,om);

    if (_solverName=="GLPK" && !om->exportOnly()  && om->cols().size()>0 ) {
        _ctrl->errHandler().setExecStep("run");


        if (!om->isLinearModel())
            _ctrl->errHandler().internalError("GLPK cannot solve a nonlinear model"  );


        PROTO_OUTL("Start SolverGlpk module " << moduleName());

        setBinFullName();

        PROTO_OUTL("SolverGlpk: writing instance file " << moduleName());
        writeInstanceFile("");

        PROTO_OUTL("SolverGlpk: solving instance" << moduleName());

        GET_NEW_DATA(Solution,sol);

        string probName = StringStore::modelName(string( modp()->data()->cmplFileBase() ))+".cmpl";
        sol->prepareSolutionData(probName, _solverName, _integerRelaxation,_addConForUnusedVar, _data,this);

        generateCmdLine(sol);

        int ret = solve();

        PROTO_OUTL("SolverGlpk: finished solving  and reading solution" << moduleName());

        if (ret==0)
            readSolFile(sol);


        deleteTmpFiles();
        PROTO_OUTL("End SolverGlpk module " << moduleName());
    }


}

/**
  * @brief generates the the solver specific command line for the solver
  * @param sol   pointer to Solution object
  */
void SolverGlpk::generateCmdLine(Solution* sol) {
    _solverCmdLine="\""+_solverBinName+"\"  --freemps " + " --"+sol->objSense()+ " --write "+_instanceSolName;

    for (size_t i=0; i<_solverOpts.size(); i++)
        _solverCmdLine+= " --" + _solverOpts[i].key + " " + _solverOpts[i].value;

    _solverCmdLine+=  " " + _instanceFileName + " 2>&1";
}


/**
 * @brief reads the solver specific solution file
 * @param sol   pointer to Solution object
 */
void SolverGlpk::readSolFile(Solution* sol) {

    string line;

    unsigned long varIdx=0;
    unsigned long conIdx=0;

    try {

        ifstream  solFile( _instanceSolName, ifstream::binary) ;

        if (!solFile.is_open())
            _ctrl->errHandler().internalError(_ctrl->printBuffer("Cannot read solution file '%s'", _instanceSolName.c_str() )  ,NULL);

        SingleSolution solution;

        while ( getline( solFile, line) ) {

            line=StringStore::lrTrim(line);

            //comment or eof
            if ( StringStore::startsWith(line,"c") || StringStore::startsWith(line,"e") )
                continue;

            vector<string> solList;
            StringStore::split(line,solList);

            // obj function
            if ( StringStore::startsWith(line,"s") ) {


                if ( StringStore::lrTrim(solList[4])=="u") {
                    sol->setNrOfSolutions(0);
                    sol->setStatus("undefined");
                } else if ( StringStore::lrTrim(solList[4])=="f" ||  StringStore::lrTrim(solList[4])=="o"  ) {

                    sol->setNrOfSolutions(1);
                    sol->setStatus("normal");

                    if ( StringStore::lrTrim(solList[4])=="f" ) {
                        if ( StringStore::lrTrim(solList[1])=="bas")
                            solution.setStatus("optimal");
                        else if ( StringStore::lrTrim(solList[1])=="mip")
                            solution.setStatus("feasible");
                    }

                    if ( StringStore::lrTrim(solList[4])=="o" )
                        solution.setStatus("optimal");

                    int vPos;
                    vPos = solList.size() -1;

                    double objVal;
                    if (!StringStore::toDouble(solList[vPos],objVal))
                        _ctrl->errHandler().internalError("Internal error while reading obValue" ,NULL);
                    solution.setValue(objVal);

                } else if ( StringStore::lrTrim(solList[4])=="i") {
                    sol->setNrOfSolutions(0);
                    sol->setStatus("infeasible");
                } else if ( StringStore::lrTrim(solList[4])=="n") {
                    sol->setNrOfSolutions(0);
                    sol->setStatus("no feasible solution");
                }

                continue;
            }

            if ( StringStore::startsWith(line,"i") || StringStore::startsWith(line,"j") ) {

                // i rows - j columns

                SolutionElement solElem;
                solElem.setMarginal(0);

                double activity=0;
                double marginal=0;

                int aPos=0;
                int mPos=0;

                if (solList.size()==3) {
                    //mip
                    aPos=2;
                    mPos=-1;
                } else if (solList.size()==4) {
                    //interior-point
                    aPos=2;
                    mPos=3;
                } else if (solList.size()==5) {
                    //LP
                    aPos=3;
                    mPos=4;
                }

                if (!StringStore::toDouble(solList[aPos],activity))
                    _ctrl->errHandler().internalError("Internal error while reading activity" ,NULL);

                if (mPos>0) {
                    if (!StringStore::toDouble(solList[mPos],marginal))
                        _ctrl->errHandler().internalError("Internal error while reading mariginal" ,NULL);
                }

                solElem.setActivity(activity);
                solElem.setMarginal(marginal);

                if ( StringStore::startsWith(line,"j") ){
                    solElem.setModelElement(sol->modelVariable(varIdx));
                    if (sol->modelVariable(varIdx)->type()!="C") {
                        activity=round(activity);
                        solElem.setActivity(activity);
                    }
                    solution.setVariable(solElem);
                    varIdx++;
                }

                if ( StringStore::startsWith(line,"i") ) {
                    solElem.setModelElement(sol->modelConstraint(conIdx));
                    solution.setConstraint(solElem);
                    conIdx++;

                }
            }
        }

        sol->setSolution(solution);

    }catch (FileException& e) {
        _ctrl->errHandler().internalError(_ctrl->printBuffer("%s: solution file '%s'", e.what(), _instanceSolName.c_str()) ,&e);
    }
}


}
