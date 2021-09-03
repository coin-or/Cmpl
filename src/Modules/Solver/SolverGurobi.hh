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


#ifndef SOLVERGUROBI_HH
#define SOLVERGUROBI_HH

#include <string>
#include <iostream>

#include "../../CommonData/Solution.hh"

#include "Solver.hh"

namespace cmpl
{

class Solution;



/**
 * @brief The SolverGurobi class
 */
class SolverGurobi : public Solver
{


public:
    /**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
    virtual void init(MainControl *ctrl, MainData *data, const char *name);

    /**
     * run module function
     */
    virtual void run();

    /**
     * @brief reads the solver specific solution file
     * @param sol   pointer to Solution object
     * @param om    pointer to ObtModel object
     */
    void readSolFile(Solution *sol, OptModel* om);


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

private:

    /**
      * @brief generates the the solver specific command line for the solver
      */
    void generateCmdLine();

    /**
     * @brief reads a key and a val from an Xml string
     * @param valString     Xml string
     * @return
     */
    string readXmlVal(string valString);
};


}
#endif // SOLVERGUROBI_HH
