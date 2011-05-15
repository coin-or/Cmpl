/***********************************************************************
 *  This code is part of cmplFrame
 *
 *  Copyright (C) 2010 Mike Steglich - Technical University
 *  of Applied Sciences  Wildau, Germany
 *  and Thomas Schleiff - Halle(Saale), Germany
 *
 *  cmplFrame and CMPL are projects of the Technical University of
 *  Applied Sciences Wildau and the Institute for Operations Research
 *  and Business Management at the Martin Luther University
 *  Halle-Wittenberg.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  cmplFrame is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  cmplFrame is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef cFExceptionH
#define cFExceptionH

#include <string>
#include <iostream>
using namespace std;

/**
 * The <code>cFException</code> implements the cmplFrame exceptions handling.
 *
 * @author  Mike Steglich
 */

class cFException 
{
	
public:
        /**
         * constructor
         *
         * @param msg text of the error message
         */
        cFException(string msg);

        /**
         * <p>Returns a cmplFramework message  </p>
         * Necessary if a cmplFramework exception was happened
         */
	void getMessage();

        /**
         * destructor
         */
	virtual  ~cFException();

private:
        /**
          * string of the current cmplFrameMessage
          */
        string cFMessage;
	

	
};



#endif
