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


#include "SolverGurobi.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/Solution.hh"


namespace cmpl
{
/*********** module definition **********/

MODULE_CLASS( solverGurobi, SolverGurobi )



/*********** initialization **********/

/**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
void SolverGurobi::init(MainControl *ctrl, MainData *data, const char *name)
{
    _solverModule="GUROBI";
    Solver::init(ctrl, data, name);
 }


/*********** handling of command line options **********/


/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void SolverGurobi::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
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
bool SolverGurobi::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (Solver::parseOption(ref, prio, opt))
        return true;

    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void SolverGurobi::usage(ostream& s)
{
    Solver::usage(s);

}

/**
     * run module function
     */
void SolverGurobi::run()
{
    GET_DATA(OptModel,om);

    if (_solverName=="GUROBI" && !om->exportOnly()  && om->cols().size()>0 ) {
        _ctrl->errHandler().setExecStep("run");


        if (!om->isLinearModel())
            if (om->modelProp().hasConditions())
                _ctrl->errHandler().internalError("Gurobi cannot solve a nonlinear model within Cmpl"  );


        PROTO_OUTL("Start SolverGurobi module " << moduleName());

        setBinFullName();

        PROTO_OUTL("SolverGurobi: writing instance file " << moduleName());
        writeInstanceFile("-mps-format gurobi -mps-format-sos cplex -mps-format-qp cplex ");

        PROTO_OUTL("SolverGurobi: solving instance" << moduleName());

        GET_NEW_DATA(Solution,sol);

        string probName = StringStore::modelName(string( modp()->data()->cmplFileBase() ))+".cmpl";
        sol->prepareSolutionData(probName, _solverName, _integerRelaxation,_addConForUnusedVar, _data,this);

        generateCmdLine();

        int ret = solve();

        PROTO_OUTL("SolverGurobi: finished solving  and reading solution" << moduleName());

        if (ret==0)
           readSolFile(sol,om);

        deleteTmpFiles();
        PROTO_OUTL("End SolverGurobi module " << moduleName());
    }
}


/**
  * @brief generates the the solver specific command line for the solver
  */
void SolverGurobi::generateCmdLine() {

    string solPool = ( _solutionPool ? "1" :"0");

    _solverCmdLine="\""+_solverBinName+"\" "+solPool+" " + StringStore::replaceAll(_instanceFileName," ","%") + " " + StringStore::replaceAll(_instanceSolName," ","%");

    for (size_t i=0; i<_solverOpts.size(); i++)
        _solverCmdLine+= " " + _solverOpts[i].key + "=" + _solverOpts[i].value;

     _solverCmdLine+= " 2>&1";
}

/**
 * @brief reads a key and a val from an Xml string
 * @param valString     Xml string
 * @return
 */
string SolverGurobi::readXmlVal(string valString) {
    string ret;
    vector <string> valList;
    StringStore::split(StringStore::lrTrim(valString),valList,"=");
    if (valList.size()==0)
        _ctrl->errHandler().internalError( _ctrl->printBuffer("internal error while reading Cplex solution file: %s", valString.c_str()) ,NULL);
    ret = valList[1];
    StringStore::cleanDoubleQuotes(ret);
    return ret;

}

/**
 * @brief reads the solver specific solution file
 * @param sol   pointer to Solution object
 * @param om    pointer to ObtModel object
 */
void SolverGurobi::readSolFile(Solution* sol,  OptModel* om) {

    string line;

    unsigned long varIdx;
    unsigned long conIdx;

    unsigned long lineNr=0;
    bool headerSection=false;
    bool conSection=false;
    bool varSection=false;

    bool continueReading=true;

    int nrOfSolutions=0;

    SingleSolution* solution;

    bool isQP = false;

    if ( om->modelProp().modelType() == OptModel::ModelType::modelTypeMIQP || om->modelProp().modelType() == OptModel::ModelType::modelTypeQP )
        isQP = true;

    unsigned long rowCnt = om->rows().size();
    LinearModel *lm = om->getRowModel( isQP );
    const vector<LinearModel::Coefficient> *coeffs = lm->coeffs();

    try {

        ifstream  solFile( _instanceSolName, ifstream::binary) ;

        if (!solFile.is_open())
            _ctrl->errHandler().internalError(_ctrl->printBuffer("Cannot read solution file '%s'", _instanceSolName.c_str() )  ,NULL);

        while ( getline( solFile, line) && continueReading ) {

            line=StringStore::lrTrim(line);

            lineNr++;
            if (lineNr==1) {
                if (!StringStore::startsWith(line,"<?xml"))
                    _ctrl->errHandler().internalError(_ctrl->printBuffer("Wrong file type for file '%s'", _instanceSolName.c_str() )  ,NULL);
                continue;
            }

            if (lineNr==2 && !StringStore::startsWith(line,"<CmplGurobiSolutions"))
                _ctrl->errHandler().internalError(_ctrl->printBuffer("Wrong file type for file '%s'", _instanceSolName.c_str() )  ,NULL);


            if (StringStore::startsWith(line,"<solution")) {
                solution = new SingleSolution();

                varIdx=0;
                conIdx=0;

                headerSection=false;
                conSection=false;
                varSection=false;

                continue;
            }


            if (StringStore::startsWith(line,"</solution") ) {

                if(solution->constraints()->size()==0) {
                    vector<double> vars;
                    for (auto& var: *solution->variables())
                        vars.push_back(var.activity());

                    const char *mode;
                    mode = lm->mode();
                    unsigned long idx=0;

                    for(unsigned long i=1; i<rowCnt; i++, mode++) {
                        char m = *mode;
                        if (m) {
                            double conAct = solution->calculateConActivity(om, sol, coeffs[i]);

                            SolutionElement solElem;
                            solElem.setMarginal(0);
                            solElem.setActivity(conAct);
                            solElem.setModelElement(sol->modelConstraint(idx));
                            solution->setConstraint(solElem);
                            idx++;
                        }
                    }
               }

                sol->setSolution(*solution);
                delete solution;

                headerSection=false;
                conSection=false;
                varSection=false;
            }

            if (StringStore::startsWith(line,"<header")) {

                headerSection=true;
                conSection=false;
                varSection=false;

            }

            if (StringStore::startsWith(line,"<constraints")) {

                headerSection=false;
                conSection=true;
                varSection=false;
                continue;
            }

            if (StringStore::startsWith(line,"</constraints")) {

                headerSection=false;
                conSection=false;
                varSection=false;
                continue;
            }

            if (StringStore::startsWith(line,"<variables")) {
                headerSection=false;
                conSection=false;
                varSection=true;
                continue;
            }

            if (StringStore::startsWith(line,"</variables")) {
                headerSection=false;
                conSection=false;
                varSection=false;
                continue;
            }

            if (headerSection) {

                vector<string> varXmlVals;
                StringStore::split(line,varXmlVals);

                string valStr=readXmlVal(varXmlVals[1]);
                double objVal;
                if (!StringStore::toDouble(valStr,objVal))
                    _ctrl->errHandler().internalError("Internal error while reading objective function value from Gurobi solution file" ,NULL);

                nrOfSolutions++;
                solution->setValue(objVal);

                string objStatus=StringStore::replaceAll(readXmlVal(varXmlVals[2]),"_"," ");
                solution->setStatus(objStatus);

                continue;
            }

            if (varSection || conSection) {

                SolutionElement solElem;
                solElem.setMarginal(0);

                double activity=0;
                double marginal=0;

                vector<string> varXmlVals;
                StringStore::split(line,varXmlVals);

                string valStr=readXmlVal(varXmlVals[2]);
                if (!StringStore::toDouble(valStr,activity))
                    _ctrl->errHandler().internalError("Internal error while reading activity from Gurobi solution file" ,NULL);
                solElem.setActivity(activity);

                if (sol->hasMarginal()) {
                    string valStr=readXmlVal(varXmlVals[3]);
                    if (!StringStore::toDouble(valStr,marginal))
                        _ctrl->errHandler().internalError("Internal error while reading marginal from gurobi solution file" ,NULL);
                    solElem.setMarginal(marginal);
                }

                if (varSection) {
                    solElem.setModelElement(sol->modelVariable(varIdx));
                    if (sol->modelVariable(varIdx)->type()!="C") {
                        activity=round(activity);
                        solElem.setActivity(activity);
                    }
                    solution->setVariable(solElem);
                    varIdx++;
                } else {
                    solElem.setModelElement(sol->modelConstraint(conIdx));
                    solution->setConstraint(solElem);
                    conIdx++;

                }
                continue;
            }
        }

    }catch (FileException& e) {
        _ctrl->errHandler().internalError(_ctrl->printBuffer("%s: solution file '%s'", e.what(), _instanceSolName.c_str()) ,&e);
    }

    sol->setStatus("normal");
    sol->setNrOfSolutions(nrOfSolutions);
}



}
