#ifndef SOLUTIONREPORT_HH
#define SOLUTIONREPORT_HH

#include <string>
#include <iostream>


#include "../../Control/ModuleBase.hh"
#include "../../CommonData/Solution.hh"


namespace cmpl
{





/**
* <code>TestModule</code> is only used for tests of cmpl main control.
* <code>TestModule</code> uses three test command line options:
* 	-a		boolean option without any argument
* 	-b		option with one argument, which must be an int number
* 	-c		option with arbitrary arguments
* beside these options there is an output control command line options:
*  -o		option with one file name argument, this file is used for outputs instead of <code>cout</code>
*/
class SolutionReport : public ModuleBase
{


public:
    /**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
    virtual void init(MainControl *ctrl, MainData *data, const char *name);



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


    bool _ignoreZeros;
    bool _ignoreVars;
    bool _ignoreCons;
    bool _ignoreGeneratedElements;
    bool _solutionPool;


    vector <string> _displayVarList;
    vector <string> _displayConList;

    
};


}
#endif // SOLUTIONREPORT_HH
