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


#include "SolverHiGHS.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/Solution.hh"


namespace cmpl
{
/*********** module definition **********/

MODULE_CLASS( solverHighs, SolverHiGHS )


/*********** initialization **********/

/**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
void SolverHiGHS::init(MainControl *ctrl, MainData *data, const char *name)
{
    _solverModule="HIGHS";
    Solver::init(ctrl, data, name);
}


/*********** handling of command line options **********/


/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void SolverHiGHS::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
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
bool SolverHiGHS::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (Solver::parseOption(ref, prio, opt))
        return true;

    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void SolverHiGHS::usage(ostream& s)
{
    Solver::usage(s);

}

/**
     * run module function
     */
void SolverHiGHS::run()
{
    GET_DATA(OptModel,om);

    if (_solverName=="HIGHS" && !om->exportOnly()  && om->cols().size()>0 ) {
        _ctrl->errHandler().setExecStep("run");

        if (!om->isLinearModel())
            if (om->modelProp().hasConditions())
                _ctrl->errHandler().internalError("HiGHS cannot solve a nonlinear model"  );


        PROTO_OUTL("Start SolverHighs module " << moduleName());

        setBinFullName();

        PROTO_OUTL("SolverHighs: writing instance file " << moduleName());
        //writeInstanceFile("-mps-format highs -mps-format-sos cplex -mps-format-sos cplex");
        writeInstanceFile("-mps-format highs");


        PROTO_OUTL("SolverHighs: solving instance" << moduleName());

        GET_NEW_DATA(Solution,sol);


        string probName = StringStore::modelName(string( modp()->data()->cmplFileBase() ))+".cmpl";
        sol->prepareSolutionData(probName, _solverName, _integerRelaxation, _addConForUnusedVar, _data, this);

        generateCmdLine();

        int ret = solve();

        PROTO_OUTL("SolverHighs: finished solving  and reading solution" << moduleName());

        if (ret>=0)
            readSolFile(sol);

        deleteTmpFiles();
        PROTO_OUTL("End SolverHighs module " << moduleName());
    }
}

/**
 * @brief writes the Highs option file
 */
void SolverHiGHS::writeOptFile(){

    try {
        ofstream  optFile( _instanceCmdName.c_str() ) ;

        optFile << "solution_file = \"" << _instanceSolName <<"\"" << endl;
        optFile << "write_solution_style = 2 " << endl;
        optFile << "write_solution_to_file = True " << endl;
        optFile << "threads = 4 " << endl;
        optFile << " mip_rel_gap = 0.00001  " << endl;
        //optFile << "parallel = on " << endl;

        for (size_t i=0; i<_solverOpts.size(); i++) {
            optFile << _solverOpts[i].key << " = " << _solverOpts[i].value << endl;
        }
        optFile.close();
    }
    catch (exception& ex) {
        _ctrl->errHandler().internalError( _ctrl->printBuffer("Internal error while writing HiGHS option file >> '%s'", string(ex.what()).c_str() ) ,NULL);
    }
}

/**
  * @brief generates the the solver specific command line for the solver
  */
void SolverHiGHS::generateCmdLine() {
    writeOptFile();
    
    //_solverCmdLine=_solverBinName+" -c \"set load " + _instanceCmdName + "\" ";
    _solverCmdLine="\""+_solverBinName+"\"";
    _solverCmdLine+=" --options_file \"" + _instanceCmdName + "\"";
    _solverCmdLine+=" \"" + _instanceFileName +"\"";
}


/**
 * @brief reads the solver specific solution file
 * @param sol   pointer to Solution object
 * @param om    pointer to ObtModel object
 */
void SolverHiGHS::readSolFile(Solution* sol) {

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

    headerLines=9;

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

                if  ((lineNr==1) and ( solList.size()==0 )) {
                    sol->setNrOfSolutions(0);
                    sol->setStatus("infeasible");
                }

                if (lineNr==5) {
                    string solStat = solList[2];

                    if (solList.size() == 4 )
                        solStat += " "+solList[3];

                    if ( (StringStore::contains(solStat,"EMPTY")) or
                         (StringStore::contains(solStat,"INFEASIBLE")) or
                         (StringStore::contains(solStat,"UNBOUNDED")) or
                         (StringStore::contains(solStat,"UNDEFINED")) ) {
                        sol->setNrOfSolutions(0);
                    } else {
                        sol->setNrOfSolutions(1);
                    }

                    solution.setStatus(solStat);
                }

                if (lineNr==8) {
                    if (solList.size()>=6) {
                        double objVal;
                        if (!StringStore::toDouble(solList[solList.size()-1],objVal))
                            _ctrl->errHandler().internalError("Internal error while reading obValue" ,NULL);
                        solution.setValue(objVal);
                    } else {
                        _ctrl->errHandler().internalError("Internal error while reading obValue" ,NULL);
                    }

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

                if (solList.size()==3) {
                    aPos=2;
                    mPos=-1;

                } else if (solList.size()==5) {
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
                if (lineNr>(headerLines+sol->nrOfConstraints()+sol->nrOfVariables()-1)) {
                    headerSection=false;
                    varSection=false;
                    conSection=false;
                }

            }
        }
        sol->setSolution(solution);

    }
    catch (FileException& e) {
        _ctrl->errHandler().internalError(_ctrl->printBuffer("%s: solution file '%s'", e.what(), _instanceSolName.c_str()) ,&e);
    }
}


}
