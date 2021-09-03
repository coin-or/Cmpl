#include "SolutionAscii.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/Solution.hh"

#include <iomanip>

namespace cmpl
{
/*********** module definition **********/

// defines the module "SolveCbc" that is implemented in class "SolveCbc".
// to register this module you have to insert "MODULE_CLASS_REG(1, SolverCbc)" to file "modules.reg"
MODULE_CLASS( solutionReportAscii, SolutionAscii )



/*********** initialization **********/

/**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
void SolutionAscii::init(MainControl *ctrl, MainData *data, const char *name)
{
    SolutionReport::init(ctrl, data, name);

    _solutionAscii=NULL;
    _solutionStdOut=NULL;

    _isSolStd=false;
    _isFileOut=false;

}


/*********** handling of command line options **********/

#define OPTION_SOLASCII_STD            	100
#define OPTION_SOLASCII_STDIO      		101
#define OPTION_SOLASCII_ASCII      		102
#define OPTION_SOLASCII_CSV      		103
#define OPTION_SOLASCII_XML      		104



/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void SolutionAscii::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
{
    SolutionReport::regModOptions(modOptReg);

    REG_CMDL_OPTION( OPTION_SOLASCII_STD, "solStdOut", 0, 0, CMDL_OPTION_NEG_NO_ARG, true );

    REG_CMDL_OPTION( OPTION_SOLASCII_STDIO, "solutionStdOut", 0, 0, CMDL_OPTION_NEG_NO_ARG, true );

    REG_CMDL_OPTION( OPTION_SOLASCII_ASCII, "solutionAscii", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );

    REG_CMDL_OPTION( OPTION_SOLASCII_CSV, "solutionCsv", 0, 1, CMDL_OPTION_NEG_NO_ARG, false );

    REG_CMDL_OPTION( OPTION_SOLASCII_XML, "solution", 0, 1, CMDL_OPTION_NEG_NO_ARG, false );

}

/**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
bool SolutionAscii::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (SolutionReport::parseOption(ref, prio, opt))
        return true;

    switch (ref) {

    case OPTION_SOLASCII_STD:
    case OPTION_SOLASCII_STDIO:

        if (ref==OPTION_SOLASCII_STDIO)
            _isSolStd=true;


        if (_solutionStdOut) {
            delete _solutionStdOut;
            _solutionStdOut = NULL;
        }

        _solutionStdOut=new FileOutput();
        _solutionStdOut-> setFile(_data, IO_MODE_STD_IO, NULL, NULL, true);

        return true;

    case OPTION_SOLASCII_ASCII:

        if (!(opt->neg())) {
            _solutionAscii = new FileOutput();
            _solutionAscii->setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_SOLUTION_ASCII, true);
            if (opt->size() > 0)
                _solutionAscii->setLocSrc(opt->argPos(0));
            else
                _solutionAscii->setLocSrc(opt->loc(true));
        }
        _isFileOut=true;

        return true;
    case OPTION_SOLASCII_CSV:
    case OPTION_SOLASCII_XML:
        _isFileOut=true;


    }


    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void SolutionAscii::usage(ostream& s)
{
    SolutionReport::usage(s);
    //toDO
}

/**
     * run module function
     */
void SolutionAscii::run()
{
    _ctrl->errHandler().setExecStep("run");

    PROTO_OUTL("Start SolutionAscii module " << moduleName());

    GET_DATA(Solution,sol);

    if (sol) {

        if (_isFileOut && !_isSolStd) {
            if (_solutionStdOut) {
                delete _solutionStdOut;
                _solutionStdOut = NULL;
            }
        }

        if (_solutionAscii) {
            CmplOutput(cout,"Writing solution to Ascii file > "  +_solutionAscii->fileNameReplaced());
            solutionReport(sol,_solutionAscii);
        }

        if (_solutionStdOut) {
            CmplOutput(cout,"Reporting solution");
            solutionReport(sol,_solutionStdOut);
        }


    }


    PROTO_OUTL("End SolutionAscii module " << moduleName());
}


void SolutionAscii::solutionReport(Solution *sol, FileOutput *file) {
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


void SolutionAscii::writeSolReport(Solution *sol, ostream& ostr) {

    PROTO_OUTL("Writing solution report");

    ostr << setw(105) << setfill('-') << "" <<endl;
    ostr <<  setfill(' ');

    ostr << setw(21) << left << "Problem  " <<  sol->problemName() << endl;

    ostr << setw(21) << left <<  "Nr. of variables" << sol->nrOfVariables()<< endl;
    ostr << setw(21) << left <<  "Nr. of constraints" << sol->nrOfConstraints() << endl;
    ostr << setw(21) << left <<  "Objective name   " << sol->objName() << endl;
    if (sol->nrOfSolutions()>1) {
        ostr << setw(21) << left <<  "Nr. of solutions " << sol->nrOfSolutions() << endl;
    }
    ostr << setw(21) << left << "Solver name" << sol->solver() << endl;

    ostr << setw(21) << left << "Display variables" ;

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
                if (j>0) ostr << ",";
                ostr << _displayVarList[j];
            }
        }
        if (!_ignoreGeneratedElements)
            ostr << ",generatedElements";

        ostr << ")";
    }

    ostr << endl;

    ostr << setw(21) << left << "Display constraints" ;

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
                if (j>0) ostr << ",";
                ostr << _displayConList[j];
            }
        }
        if (!_ignoreGeneratedElements)
            ostr << ",generatedElements";
        ostr << ")";
    }

    ostr << endl;

    ostr << setw(105) << setfill('-') << "" <<endl;
    ostr <<  setfill(' ');



    if (sol->nrOfSolutions()==0) {
        ostr << endl;
        ostr << setw(21) << left <<  "No solution has been found " << endl << endl;
        ostr << setw(105) << setfill('-') << "" <<endl;
    }



    for (int i=0; i<sol->nrOfSolutions(); i++) {

        ostr << endl;

        ostr <<  setfill(' ');

        if (sol->nrOfSolutions()>1) {
            ostr << setw(21) << left <<  "Solution nr. " << i+1 << endl;
        }
        ostr << setw(21) << left <<  "Objective status " << sol->solution(i)->status() << endl;
        ostr << setw(21) << left <<  "Objective value  " << sol->solution(i)->value()  << " (" << sol->objSense() << "!)" << endl;
        ostr <<endl;


        if (!_ignoreVars) {
            ostr << "Variables " <<  endl;

            ostr << setw(20) << left <<  "Name";
            ostr << setw(5) << right <<  "Type";
            ostr << setw(20) << right <<   "Activity";
            ostr << setw(20) << right <<  "Lower bound";
            ostr << setw(20) << right <<  "Upper bound";
            ostr << setw(20) << right <<  "Marginal" << endl;
            ostr << setw(105) << setfill('-') << "" <<endl;
            ostr <<  setfill(' ');

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

            ostr << setw(105) << setfill('-') << "" <<endl;
            ostr <<  setfill(' ');
            ostr << endl;
        }

        if (!_ignoreCons) {
            ostr << "Constraints " <<  endl;

            ostr << setw(20) << left <<  "Name";
            ostr << setw(5) << right <<  "Type";
            ostr << setw(20) << right <<   "Activity";
            ostr << setw(20) << right <<  "Lower bound";
            ostr << setw(20) << right <<  "Upper bound";
            ostr << setw(20) << right <<  "Marginal" << endl;
            ostr << setw(105) << setfill('-') << "" <<endl;
            ostr <<  setfill(' ');

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

            ostr << setw(105) << setfill('-') << "" <<endl;
        }

    }
}


void SolutionAscii::writeVarValues(Solution *sol, unsigned long i, unsigned long j , ostream& ostr) {

    if ( !_ignoreGeneratedElements || (_ignoreGeneratedElements && !StringStore::startsWith(sol->solution(i)->variable(j)->name(), "__") ) ) {
        ostr << setw(20) <<   left <<  sol->solution(i)->variable(j)->name();
        ostr << setw(5) <<  right << sol->solution(i)->variable(j)->type();
        ostr << setw(20) << right << ( sol->solution(i)->variable(j)->type()=="C" ? sol->solution(i)->variable(j)->activity() : round(sol->solution(i)->variable(j)->activity() ));

        ostr << setw(20) << right << sol->solution(i)->variable(j)->lowerBound();
        ostr << setw(20) << right << sol->solution(i)->variable(j)->upperBound();
        if (sol->hasMarginal())
            ostr << setw(20)  << right<< sol->solution(i)->variable(j)->marginal();
        else
            ostr << setw(20) << right << "-" ;
        ostr << endl;
    }
}

void SolutionAscii::writeConValues(Solution *sol, unsigned long i, unsigned long j , ostream& ostr) {

    if ( !_ignoreGeneratedElements || (_ignoreGeneratedElements && !StringStore::startsWith(sol->solution(i)->constraint(j)->name(), "__") ) ) {
        ostr << setw(20) <<   left <<  sol->solution(i)->constraint(j)->name();
        ostr << setw(5) <<  right << sol->solution(i)->constraint(j)->type();
        ostr << setw(20) << right << sol->solution(i)->constraint(j)->activity() ;


        ostr << setw(20) << right << sol->solution(i)->constraint(j)->lowerBound();

        ostr << setw(20) << right << sol->solution(i)->constraint(j)->upperBound();
        if (sol->hasMarginal())
            ostr << setw(20)  << right<< sol->solution(i)->constraint(j)->marginal();
        else
            ostr << setw(20) << right << "-" ;
        ostr << endl;
    }
}


}





