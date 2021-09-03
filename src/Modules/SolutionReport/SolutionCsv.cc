#include "SolutionCsv.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/Solution.hh"

#include <iomanip>

namespace cmpl
{
/*********** module definition **********/

// defines the module "SolveCbc" that is implemented in class "SolveCbc".
// to register this module you have to insert "MODULE_CLASS_REG(1, SolverCbc)" to file "modules.reg"
MODULE_CLASS( solutionReportCsv, SolutionCsv )



/*********** initialization **********/

/**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
void SolutionCsv::init(MainControl *ctrl, MainData *data, const char *name)
{
    SolutionReport::init(ctrl, data, name);

    _SolutionCsv=NULL;
}


/*********** handling of command line options **********/


#define OPTION_SOLCSV_CSV       		100



/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void SolutionCsv::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
{
    SolutionReport::regModOptions(modOptReg);

    REG_CMDL_OPTION( OPTION_SOLCSV_CSV, "solutionCsv", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );

}

/**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
bool SolutionCsv::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (SolutionReport::parseOption(ref, prio, opt))
        return true;

    switch (ref) {


    case OPTION_SOLCSV_CSV:

        if (!(opt->neg())) {
            _SolutionCsv = new FileOutput();
            _SolutionCsv->setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_SOLUTION_SOLCSV, true);
            if (opt->size() > 0)
                _SolutionCsv->setLocSrc(opt->argPos(0));
            else
                _SolutionCsv->setLocSrc(opt->loc(true));
        }
        return true;

    }


    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void SolutionCsv::usage(ostream& s)
{
    SolutionReport::usage(s);
    //toDO
}

/**
     * run module function
     */
void SolutionCsv::run()
{
    _ctrl->errHandler().setExecStep("run");

    PROTO_OUTL("Start SolutionCsv module " << moduleName());

    GET_DATA(Solution,sol);

    if (sol) {

        if (_SolutionCsv) {
            CmplOutput(cout,"Writing solution to CSV file > "  +_SolutionCsv->fileNameReplaced());
            solutionReport(sol,_SolutionCsv);
       }


    }


    PROTO_OUTL("End SolutionCsv module " << moduleName());
}


void SolutionCsv::solutionReport(Solution *sol, FileOutput *file) {
    try {


        PROTO_OUTL("write solution >> " << file->fileName());

        ostream *ostr = file->open();
        writeSolReport(sol, *ostr);
        file->close();
    }

    catch (FileException& e) {
        _ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("%s: write model to file '%s'", e.what(), e.filename().c_str()), file->locSrc());
    }
    catch (exception& ex) {
        _ctrl->errHandler().error(ERROR_LVL_NORMAL, "write model as MPS file error", file->locSrc(), &ex);
    }
}


void SolutionCsv::writeSolReport(Solution *sol, ostream& ostr) {

    PROTO_OUTL("Writing CSV solution report");

    ostr <<  "Problem;" <<  sol->problemName() << endl;

    ostr << "Nr. of variables;" << sol->nrOfVariables() << endl;
    ostr <<  "Nr. of constraints;" << sol->nrOfConstraints() << endl;
    ostr <<  "Objective name;" << sol->objName() << endl;
    if (sol->nrOfSolutions()>1) {
        ostr <<  "Nr. of solutions;" << sol->nrOfSolutions() << endl;
    }
    ostr << "Solver name;" << sol->solver() << endl;

    ostr << "Display variables;" ;

    if (_ignoreVars)
        ostr << "ignore all variables ";
    else {

        if (_ignoreZeros)
            ostr << "nonzero variables ";

        ostr << "(";
        if (_displayVarList.size()==0)
            ostr << "all";
        else {
            for (int j=0; j<_displayVarList.size(); j++) {
                if (j>0) ostr << " ";
                ostr << _displayVarList[j];
            }
        }

        if (!_ignoreGeneratedElements)
            ostr << " generatedElements";

        ostr << ")";
    }

    ostr << endl;

    ostr << "Display constraints;" ;

    if (_ignoreCons)
        ostr << "ignore all constraints ";
    else {

        if (_ignoreZeros)
            ostr << "nonzero constraints ";

        ostr << "(";
        if (_displayConList.size()==0)
            ostr << "all";
        else {
            for (int j=0; j<_displayConList.size(); j++) {
                if (j>0) ostr << " ";
                ostr << _displayConList[j];
            }
        }

        if (!_ignoreGeneratedElements)
            ostr << " generatedElements";

        ostr << ")";
    }

    ostr << endl;


    if (sol->nrOfSolutions()==0) {
        ostr <<  "No solution has been found " << endl ;
    }

    for (int i=0; i<sol->nrOfSolutions(); i++) {

        if (sol->nrOfSolutions()>1) {
            ostr <<  "Solution nr.;" << i+1 << endl;
        }
        ostr <<  "Objective status;" << sol->solution(i)->status() << endl;
        ostr <<  "Objective value ;" << sol->solution(i)->value() << ";(" << sol->objSense() << "!)" << endl;

        if (!_ignoreVars) {
            ostr << "Variables;" <<  endl;

            ostr <<  "Name;";
            ostr <<  "Type;";
            ostr <<   "Activity;";
            ostr <<  "Lower bound;";
            ostr <<  "Upper bound;";
            ostr <<  "Marginal" << endl;

            if (_displayVarList.size()==0) {
                for (unsigned long j=0; j<sol->nrOfVariables(); j++ ) {
                    if (!(sol->solution(i)->variable(j)->activity()==0.0 && _ignoreZeros) ) {
                        writeVarValues(sol, i,j, ostr) ;
                    }
                }
            } else {
                for (int k=0; k<_displayVarList.size(); k++) {
                    int starPos=_displayVarList[k].find_first_of("*");
                    string tmpStr=_displayVarList[k].substr(0,starPos);

                    for (unsigned long j=0; j<sol->nrOfVariables(); j++ ) {
                        if ( (starPos>-1 && StringStore::startsWith(  sol->solution(i)->variable(j)->name() ,tmpStr) ) ||
                             (starPos==-1 &&  sol->solution(i)->variable(j)->name()==tmpStr) ) {

                            if (!(sol->solution(i)->variable(j)->activity()==0.0 && _ignoreZeros) ) {
                                writeVarValues(sol, i,j, ostr) ;
                            }
                        }
                    }
                }
            }

        }

        if (!_ignoreCons) {
            ostr << "Constraints " <<  endl;

            ostr <<  "Name;";
            ostr <<  "Type;";
            ostr <<   "Activity;";
            ostr <<  "Lower bound;";
            ostr <<  "Upper bound;";
            ostr <<  "Marginal" << endl;

            if (_displayConList.size()==0) {
                for (unsigned long j=0; j<sol->nrOfConstraints(); j++ ) {
                    if ( !(sol->solution(i)->constraint(j)->activity()==0.0 && _ignoreZeros))  {
                        writeConValues(sol, i,j, ostr) ;
                    }
                }
            } else {
                for (int k=0; k<_displayConList.size(); k++) {
                    int starPos=_displayConList[k].find_first_of("*");
                    string tmpStr=_displayConList[k].substr(0,starPos);

                    for (unsigned long j=0; j<sol->nrOfConstraints(); j++ ) {
                        if ( (starPos>-1 && StringStore::startsWith(  sol->solution(i)->constraint(j)->name() ,tmpStr) ) ||
                             (starPos==-1 &&  sol->solution(i)->constraint(j)->name()==tmpStr) ) {
                            if (!(sol->solution(i)->constraint(j)->activity()==0.0 && _ignoreZeros) ) {
                                writeConValues(sol, i,j, ostr) ;
                            }
                        }
                    }
                }
            }
        }
    }
}


void SolutionCsv::writeVarValues(Solution *sol, unsigned long i, unsigned long j , ostream& ostr) {

    if ( !_ignoreGeneratedElements || (_ignoreGeneratedElements && !StringStore::startsWith(sol->solution(i)->variable(j)->name(), "__") ) ) {

        ostr <<  sol->solution(i)->variable(j)->name() << ";";
        ostr << sol->solution(i)->variable(j)->type() << ";";
        ostr << ( sol->solution(i)->variable(j)->type()=="C" ? sol->solution(i)->variable(j)->activity() : round(sol->solution(i)->variable(j)->activity() )) << ";";

        ostr << sol->solution(i)->variable(j)->lowerBound() << ";";
        ostr << sol->solution(i)->variable(j)->upperBound() << ";";
        if (sol->hasMarginal())
            ostr <<  right<< sol->solution(i)->variable(j)->marginal() ;
        else
            ostr  << "-" ;
        ostr << endl;
    }
}

void SolutionCsv::writeConValues(Solution *sol, unsigned long i, unsigned long j , ostream& ostr) {

    if ( !_ignoreGeneratedElements || (_ignoreGeneratedElements && !StringStore::startsWith(sol->solution(i)->constraint(j)->name(), "__") ) ) {

        ostr << sol->solution(i)->constraint(j)->name() << ";";
        ostr << sol->solution(i)->constraint(j)->type() << ";";
        ostr << sol->solution(i)->constraint(j)->activity() << ";" ;


        ostr  << sol->solution(i)->constraint(j)->lowerBound() << ";";

        ostr  << sol->solution(i)->constraint(j)->upperBound() << ";";
        if (sol->hasMarginal())
            ostr << sol->solution(i)->constraint(j)->marginal() ;
        else
            ostr  << "-" ;
        ostr << endl;
    }
}


}





