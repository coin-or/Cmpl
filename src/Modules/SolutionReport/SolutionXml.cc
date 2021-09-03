#include "SolutionXml.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/Solution.hh"

#include <iomanip>

namespace cmpl
{
/*********** module definition **********/

// defines the module "SolveCbc" that is implemented in class "SolveCbc".
// to register this module you have to insert "MODULE_CLASS_REG(1, SolverCbc)" to file "modules.reg"
MODULE_CLASS( solutionReportXml, SolutionXml )



/*********** initialization **********/

/**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
void SolutionXml::init(MainControl *ctrl, MainData *data, const char *name)
{
    SolutionReport::init(ctrl, data, name);

    _SolutionXml=NULL;
}


/*********** handling of command line options **********/


#define OPTION_SOLXML_XML       		100



/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void SolutionXml::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
{
    SolutionReport::regModOptions(modOptReg);

    REG_CMDL_OPTION( OPTION_SOLXML_XML, "solution", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );

}

/**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
bool SolutionXml::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (SolutionReport::parseOption(ref, prio, opt))
        return true;

    switch (ref) {


    case OPTION_SOLXML_XML:

        if (!(opt->neg())) {
            _SolutionXml = new FileOutput();
            _SolutionXml->setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_SOLUTION_SOLXML, true);
            if (opt->size() > 0)
                _SolutionXml->setLocSrc(opt->argPos(0));
            else
                _SolutionXml->setLocSrc(opt->loc(true));
        }
        return true;

    }


    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void SolutionXml::usage(ostream& s)
{
    SolutionReport::usage(s);
    //toDO
}

/**
     * run module function
     */
void SolutionXml::run()
{
    _ctrl->errHandler().setExecStep("run");

    PROTO_OUTL("Start SolutionXml module " << moduleName());

    GET_DATA(Solution,sol);

    if (sol) {

        if (_SolutionXml) {
            CmplOutput(cout,"Writing solution to CmplSolution file > "  +_SolutionXml->fileNameReplaced());
            solutionReport(sol,_SolutionXml);
       }


    }


    PROTO_OUTL("End SolutionXml module " << moduleName());
}


void SolutionXml::solutionReport(Solution *sol, FileOutput *file) {
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
        _ctrl->errHandler().error(ERROR_LVL_NORMAL, "write model as CmplSolution file error", file->locSrc(), &ex);
    }
}


void SolutionXml::writeSolReport(Solution *sol, ostream& ostr) {

    PROTO_OUTL("Writing CmplSolution report");



    //ostr.precision( glvar->args->maxDecimals()+1 );
    ostr << "<?xml version = \"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << endl;
    ostr << "<CmplSolutions version=\"1.1\">" << endl;
    ostr << "   <general>" << endl;
    ostr << "       <instanceName>" <<  sol->problemName()  << "</instanceName>" << endl;
    ostr << "       <nrOfVariables>" << sol->nrOfVariables() << "</nrOfVariables>" << endl;
    ostr << "       <nrOfConstraints>" << sol->nrOfConstraints() << "</nrOfConstraints>" << endl;
    ostr << "       <objectiveName>" << sol->objName() << "</objectiveName>" << endl;
    ostr << "       <objectiveSense>" << sol->objSense() << "</objectiveSense>" << endl;
    ostr << "       <nrOfSolutions>" << sol->nrOfSolutions()<< "</nrOfSolutions>" <<  endl;

    ostr << "       <solverName>" << sol->solver() << "</solverName>" << endl;

    //if (nrOfSolutions()==0)
    ostr << "       <solverMsg>" <<  sol->status() << "</solverMsg>" << endl;

    ostr << "       <variablesDisplayOptions>" ;

    if (_ignoreVars)
        ostr << "ignoreAllVariables ";
    else {
        if (_ignoreZeros)
            ostr << "nonZeroVariables ";

        ostr << "(";
        if(_displayVarList.size()==0)
            ostr << "all";
        else {
            for (int j=0; j<_displayVarList.size(); j++){
                if (j>0) ostr << ",";
                ostr << _displayVarList[j];
            }
        }

        if (!_ignoreGeneratedElements)
            ostr << ",generatedElements";

        ostr << ")";
    }
    ostr  << "</variablesDisplayOptions>" << endl;

    ostr << "       <constraintsDisplayOptions>" ;
    if (_ignoreCons)
        ostr << "ignoreAllConstraints ";
    else {
        if (_ignoreZeros)
            ostr << "nonZeroConstraints ";
        ostr << "(";
        if(_displayConList.size()==0)
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
    ostr << "</constraintsDisplayOptions>" <<endl;

    ostr << "   </general>" << endl;


    for (int i=0; i<sol->nrOfSolutions(); i++) {

        ostr << "   <solution idx=\""<< i << "\" status=\""<<  sol->solution(i)->status() << "\" value=\"" <<  sol->solution(i)->value() << "\">" << endl;


        if (!_ignoreVars) {

            ostr << "       <variables>"<< endl;

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
            ostr << "       </variables>" << endl;
        }

        if (!_ignoreCons) {
            ostr << "       <linearConstraints> " << endl;

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
            ostr << "       </linearConstraints>" << endl;
        }
        ostr << "   </solution>"<< endl;
    }
    ostr << "</CmplSolutions>"<< endl;
}


void SolutionXml::writeVarValues(Solution *sol, unsigned long i, unsigned long j , ostream& ostr) {

    if ( !_ignoreGeneratedElements || (_ignoreGeneratedElements && !StringStore::startsWith(sol->solution(i)->variable(j)->name(), "__") ) ) {


        ostr << "           <variable idx=\""<< j << "\" name=\""<< sol->solution(i)->variable(j)->name() <<
                "\" type=\"" << sol->solution(i)->variable(j)->type() <<
                "\" activity=\"" << ( sol->solution(i)->variable(j)->type()=="C" ? sol->solution(i)->variable(j)->activity() : round(sol->solution(i)->variable(j)->activity() )) << "\"";

        ostr <<  " lowerBound=\"" << sol->solution(i)->variable(j)->lowerBound() ;
        ostr << "\" upperBound=\"" << sol->solution(i)->variable(j)->upperBound() ;

        ostr << "\" marginal=\"" ;
        if (sol->hasMarginal())
            ostr <<  right << sol->solution(i)->variable(j)->marginal()  ;
        else
            ostr  << "NaN" ;
        ostr  << "\"/>" << endl;

    }


}

void SolutionXml::writeConValues(Solution *sol, unsigned long i, unsigned long j , ostream& ostr) {

    if ( !_ignoreGeneratedElements || (_ignoreGeneratedElements && !StringStore::startsWith(sol->solution(i)->constraint(j)->name(), "__") ) ) {


        ostr << "           <constraint idx=\""<< j << "\" name=\""<< sol->solution(i)->constraint(j)->name() <<
                "\" type=\"" << sol->solution(i)->constraint(j)->type()  <<
                "\" activity=\"" << sol->solution(i)->constraint(j)->activity() << "\"";

        ostr <<  " lowerBound=\"" << sol->solution(i)->constraint(j)->lowerBound();
        ostr << "\" upperBound=\"" << sol->solution(i)->constraint(j)->upperBound();

        ostr << "\" marginal=\"";

        if (sol->hasMarginal())
            ostr << sol->solution(i)->constraint(j)->marginal() ;
        else
            ostr  << "NaN" ;
        ostr  << "\"/>" << endl;

    }
}


}





