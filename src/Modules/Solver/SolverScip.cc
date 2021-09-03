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


#include "SolverScip.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/Solution.hh"


namespace cmpl
{
/*********** module definition **********/

MODULE_CLASS( solverScip, SolverScip )


/*********** initialization **********/

/**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
void SolverScip::init(MainControl *ctrl, MainData *data, const char *name)
{
    _solverModule="SCIP";
    Solver::init(ctrl, data, name);
}


/*********** handling of command line options **********/


/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void SolverScip::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
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
bool SolverScip::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (Solver::parseOption(ref, prio, opt))
        return true;

    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void SolverScip::usage(ostream& s)
{
    Solver::usage(s);

}

/**
     * run module function
     */
void SolverScip::run()
{
    GET_DATA(OptModel,om);

    if (_solverName=="SCIP" && !om->exportOnly()  && om->cols().size()>0 ) {
        _ctrl->errHandler().setExecStep("run");

        if (!om->isLinearModel())
            if (om->modelProp().hasConditions())
                _ctrl->errHandler().internalError("SCIP cannot solve a nonlinear model"  );


        PROTO_OUTL("Start SolverScip module " << moduleName());

        setBinFullName();

        PROTO_OUTL("SolverScip: writing instance file " << moduleName());
        writeInstanceFile("-mps-format scip -mps-format-sos cplex -mps-format-qp cplex");

        PROTO_OUTL("SolverScip: solving instance" << moduleName());

        GET_NEW_DATA(Solution,sol);

        if (!om->isInteger()){
             LocationInfo loc(PositionInfo(POSITION_TYPE_DESCR, "(internal)"));
             _ctrl->errHandler().error(ERROR_LVL_WARN,_ctrl->printBuffer("Warning: Scip runs only (M)ILP problems, marginals are not available in solution") , loc ) ;
        }

        string probName = StringStore::modelName(string( modp()->data()->cmplFileBase() ))+".cmpl";
        sol->prepareSolutionData(probName, _solverName, _integerRelaxation, _data,this);

        generateCmdLine();

        int ret = solve();

        PROTO_OUTL("SolverScip: finished solving  and reading solution" << moduleName());

        if (ret==0)
            readSolFile(sol,om);

        deleteTmpFiles();
        PROTO_OUTL("End SolverScip module " << moduleName());
    }
}

/**
 * @brief writes the Scip option file
 */
void SolverScip::writeOptFile(){

    try {
        ofstream  optFile( _instanceCmdName.c_str() ) ;

        for (size_t i=0; i<_solverOpts.size(); i++) {
            optFile << _solverOpts[i].key << " = " << _solverOpts[i].value << endl;
        }
        optFile.close();
    }
    catch (exception& ex) {
        _ctrl->errHandler().internalError( _ctrl->printBuffer("Internal error while writing scip option file >> '%s'", string(ex.what()).c_str() ) ,NULL);
    }
}

/**
  * @brief generates the the solver specific command line for the solver
  */
void SolverScip::generateCmdLine() {
    writeOptFile();
    
    //_solverCmdLine=_solverBinName+" -c \"set load " + _instanceCmdName + "\" ";
    _solverCmdLine="\""+_solverBinName+"\"";
    _solverCmdLine+=" -c \"set load " + _instanceCmdName + "\"";
    _solverCmdLine+=" -c \"read "+_instanceFileName+"\"";
    _solverCmdLine+=" -c optimize";
    _solverCmdLine+=" -c \"write solu "+_instanceSolName+"\"";
    _solverCmdLine+=" -c quit  2>&1";

}


/**
 * @brief reads the solver specific solution file
 * @param sol   pointer to Solution object
 * @param om    pointer to ObtModel object
 */
void SolverScip::readSolFile(Solution* sol,  OptModel* om) {

    int nrOfSolutions=0;
    unsigned long lineNr=0;

    string line;

    sol->setStatus("normal");
    sol->setNrOfSolutions(nrOfSolutions);

    SingleSolution solution;

    bool isQP = false;

    if ( om->modelProp().modelType() == OptModel::ModelType::modelTypeMIQP || om->modelProp().modelType() == OptModel::ModelType::modelTypeQP )
        isQP = true;

    unsigned long rowCnt = om->rows().size();
    LinearModel *lm = om->getRowModel( isQP );

    const vector<LinearModel::Coefficient> *coeffs = lm->coeffs();

    for (unsigned long j=0; j<sol->nrOfVariables(); j++) {
        SolutionElement solElem;
        solElem.setModelElement(sol->modelVariable(j));
        solution.setVariable(solElem);
    }

    try {
        ifstream  solFile( _instanceSolName, ifstream::binary) ;

        if (!solFile.is_open())
            _ctrl->errHandler().internalError(_ctrl->printBuffer("Cannot read solution file '%s'", _instanceSolName.c_str() )  ,NULL);


        while ( getline( solFile, line)  ) {

            line=StringStore::lrTrim(line);
            vector<string> solList;

            lineNr++;
            if (lineNr==1) {
                StringStore::split(line,solList,":");
                solution.setStatus(solList[1]);
                if (  StringStore::contains(StringStore::lrTrim(solList[1]),"optimal"  )) {
                    nrOfSolutions=1;
                } else {
                    nrOfSolutions=0;
                    break;
                }
                continue;
            }

            if (lineNr==2) {
                StringStore::split(line,solList, ":");
                double objVal;
                if (!StringStore::toDouble(solList[1],objVal))
                    _ctrl->errHandler().internalError("Internal error while reading objective function value from Scip solution file");
                solution.setValue(objVal);
                continue;
            }

            if (lineNr>2) {
                StringStore::split(line,solList);
                string varName=StringStore::lrTrim(solList[0]);
                double activity;
                if (!StringStore::toDouble(solList[1],activity))
                    _ctrl->errHandler().internalError("Internal error while reading var activity value from Scip solution file");

                unsigned long idx=sol->varMpsIdxByName(varName);
                if (sol->modelVariable(idx)->type()!="C") {
                    activity=round(activity);
                 }

                solution.variable(idx)->setActivity(activity);
            }
        }

        unsigned long idx=0;
        const char *mode;
        mode = lm->mode();

        for(unsigned long i=0; i<rowCnt; i++,mode++) {
            char m = *mode;
            if (m) {
                if (i!=om->objIdx()) {
                    double conAct = solution.calculateConActivity(om, sol, coeffs[i] );

                    SolutionElement solElem;
                    solElem.setActivity(conAct);
                    solElem.setModelElement(sol->modelConstraint(idx));
                    solution.setConstraint(solElem);
                    idx++;
                }
            }
        }

        sol->setSolution(solution);

    }catch (FileException& e) {
        _ctrl->errHandler().internalError(_ctrl->printBuffer("%s: solution file '%s'", e.what(), _instanceSolName.c_str()) ,&e);
    }

    sol->setStatus("normal");
    sol->setNrOfSolutions(nrOfSolutions);
}


}
