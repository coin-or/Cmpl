#include "SolutionReport.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/Solution.hh"

#include <iomanip>

namespace cmpl
{


/*********** initialization **********/

/**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
void SolutionReport::init(MainControl *ctrl, MainData *data, const char *name)
{
    ModuleBase::init(ctrl, data, name);

    _ignoreZeros=false;
    _ignoreVars=false;
    _ignoreCons=false;
    _ignoreGeneratedElements=true;
    _solutionPool=false;
}


/*********** handling of command line options **********/

#define OPTION_SOLREPORT_DISPLAY            30




/**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
void SolutionReport::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
{
    ModuleBase::regModOptions(modOptReg);

    REG_CMDL_OPTION( OPTION_SOLREPORT_DISPLAY, "display", 1, 200, CMDL_OPTION_NEG_NO_ARG, false );


}

/**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
bool SolutionReport::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
{
    if (ModuleBase::parseOption(ref, prio, opt))
        return true;

    switch (ref) {


    case  OPTION_SOLREPORT_DISPLAY:

        for (size_t i=0; i<opt->size();i++) {

            string option = (*opt)[i];
            string dispOpt = StringStore::lrTrim(StringStore::upperCase(option));

            if (dispOpt=="IGNOREZEROS" || dispOpt=="NONZEROS" )
                _ignoreZeros=true;
            else if (dispOpt=="IGNOREVARS")
                _ignoreVars=true;
            else if (dispOpt=="IGNORECONS")
                _ignoreCons=true;
            else if (dispOpt=="SOLUTIONPOOL")
                _solutionPool=true;
            else if (dispOpt=="GENERATEDELEMENTS")
                _ignoreGeneratedElements=false;
            else if ( StringStore::startsWith(dispOpt, "VAR") || StringStore::startsWith(dispOpt, "CON")  ) {

                string option=(*opt)[i];
                vector <string> options;
                vector <string> optList;
                StringStore::split(option,options,"=");

                if (options.size()>1) {
                    StringStore::split(options[1],optList,",");
                    for ( string & o : optList) {
                        if ( StringStore::startsWith(dispOpt, "VAR") ) {
                            _displayVarList.push_back(o);
                        }
                        else {
                            _displayConList.push_back(o);
                        }
                    }
                } else {
                    _ctrl->errHandler().error(ERROR_LVL_WARN, _ctrl->printBuffer("Wrong display option %s", option.c_str() )  , opt->loc(true) );
                }

            } else {
                _ctrl->errHandler().error(ERROR_LVL_WARN, _ctrl->printBuffer("Wrong display option %s", dispOpt.c_str() )  , opt->loc(true) );
            }


        }
        return true;


    }


    return false;
}

/**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
void SolutionReport::usage(ostream& s)
{
    ModuleBase::usage(s);
    //toDO

    s << "  -display <ignoreZeros|nonZeros>            only activities with an value unequal to zero are shown" << endl;
    s << "  -display ignoreVars                        variables are not shown" << endl;
    s << "  -display ingnoreCons                       constraints are not shown" << endl;
    s << "  -display generatedElements                 columns and rows generated by CMPL are shown" << endl;
    s << "  -display <var|con> <varOrConName=pattern>  only variables and/or constraints with name matching the pattern are shown" << endl;
    s << "  -display solutionPool                      show multiple solutions using Cpelx or Gurobi" << endl;
}

}





