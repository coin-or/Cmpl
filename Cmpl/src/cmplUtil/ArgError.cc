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


#include "ArgError.hh"


namespace cmplUtil {



    /* **************** getMessage ************* */
    void ArgError::getMessage() {

        string errM;
        if (_cmplHeader)
            errM="Error (CMPL header): ";
        else
            errM="Error (CMPL argument): ";

        cerr <<  errM ;

        if (_cmplFile.length()!=0)
            cerr << "file " << _cmplFile << " in line " << _lineNr << ": ";
        if ( _eArg.length()!=0)
                cerr << " " << _eArg << ": " << _message << endl;
            else
                cerr << " " << _message << endl;
        if (!_cmplHeader)
            //usage(1);
            cerr << "Type cmpl -h for valid commands" << endl;
    } // end getMessage



    /* **************** usage ************* */
    void ArgError::usage(int mode) {

        #define uOut ( mode==1 ? cerr : cout)

        uOut << "Usage: cmpl <cmplFile.cmpl> [options]" << endl << endl ;


        uOut << "Model mode:" << endl;
        uOut << " -i <cmplFile>           : input file " << endl;
        uOut << " -m [<File>]             : exports model in MPS format in a file or stdout" << endl;
        uOut << " -fm [<File>]            : exports model in Free-MPS format in a file or stdout" << endl;
        uOut << " -x [<File>]             : exports model in OSiL XML format in a file or stdout" << endl;
        uOut << " -syntax                 : checks the syntax of the CMPL model w/o generating of a MPS or OSiL file " << endl;

#ifdef SOLVERSERVICE
        uOut << " -noOutput               : no generating of an MPS or OSiL file" << endl;
#endif

        uOut << endl;

#ifdef SOLVERSERVICE
        uOut << "Solver mode:" << endl;
        uOut << " -solver <solver>        : name of the solver you want to use" << endl;
        uOut << "                           possible options: glpk, glpsol, cbc, scip,  gurobi, cplex (default cbc)" << endl;
        uOut << " -cmplUrl <url>          : URL of a CmplServer" << endl;
        uOut << " -maxServerTries <x>     : maximum number of tries of failed CmplServer calls" << endl;
        uOut << " -maxQueuingTime <x>     : maximum time in <x> seconds that a problem waits in a CmplServer queue" << endl;

        uOut << " -solution [<File>]      : optimisation results in CmplSolution XML format " << endl;
        uOut << " -solutionCsv [<File>]   : optimisation results in CSV format " << endl;
        uOut << " -solutionAscii [<File>] : optimisation results in ASCII format " << endl;
        uOut << " -obj <objName>          : name of the objective function" <<endl;
        uOut << " -objSense <max/min>     : objective sense" << endl;
        uOut << " -maxDecimals <x>        : maximal number of decimals in the solution report (max 12)" << endl;
        uOut << " -zeroPrecision <x>      : Precision of zero values in the solution report (default 1e-9)" << endl;
        uOut << " -ignoreZeros            : display only variables and constraints with non-zero values in the solution report" << endl;
        uOut << " -dontRemoveTmpFiles     : Don't remove temporary files (mps,osil,osrl,gsol) " << endl;
        uOut << " -alias <alias>          : uses an alias name for the cmpl model " << endl;

        uOut << endl;
#endif

        uOut << "General options:" << endl;
        uOut << " -data <cmplDataFile>    : reads a cmplData file " << endl;
        uOut << " -e [<File>]             : output for error messages and warnings " << endl;
        uOut << "                           -e simple output to stderr (default)" << endl;
        uOut << "                           -e <File> output in CmplMessage XML format to file" << endl;

        uOut << " -matrix [<File>]        : Writes the generated matrix in a file or on stdout. " <<endl;
        uOut << " -l [<File>]             : output for replacements for products of variables" << endl;
        uOut << " -s [<File>]             : short statistic info" << endl;
        uOut << " -p [<File>]             : output for protocol" << endl;
#ifdef SOLVERSERVICE
        uOut << " -silent                 : suppresses CMPL and solver messages" << endl;
#else
        uOut << " -silent                 : suppresses CMPL messages" << endl;
#endif

        uOut << " -integerRelaxation      : All integer variables are changed to continuous variables." << endl;

        uOut << " -gn                     : matrix reductions" << endl;
        uOut << " -gf                     : Generated constraints for products of variables are included at the original position of the product." << endl;
        uOut << " -cd                     : warning at multiple parameter definition" << endl;
        uOut << " -ci <x>                 : mode for integer expressions (0 - 3), (default 1)"  << endl;
        uOut << "                           If the result of an integer operation is outside the range of a long integer then the type of " << endl;
        uOut << "                           result will change from integer to real. This flag defines the integer range check behaviour. " << endl;
        uOut << "                           -ci 0	 no range check " << endl;
        uOut << "                           -ci 1	 default, range check with a type change if necessary " << endl;
        uOut << "                           -ci 2	 range check with error message if necessary " << endl;
        uOut << "                           -ci 3	 Each numerical operation returns a real result" << endl;

        uOut << " -f% <format>            : format option for MPS or OSiL files (C++ style - default %f)" << endl;

        uOut << " -h                      : get this help" << endl;

        uOut << " -v                      : version" << endl <<endl;

    } // end usage

     /* **************** ~ArgError ************* */
    ArgError::~ArgError() {

    } //end ~ArgError


}
