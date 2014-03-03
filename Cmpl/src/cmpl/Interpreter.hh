/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010 Thomas Schleiff - Halle(Saale), 
 *  Germany and Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany 
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


/**
 * The <code>Interpreter</code> class is used for executing the intermediary code
 * which the <code>Compiler</code> class has generated from the input file.
 * The <code>Interpreter</code> class contains the main loop of this execution.
 * @author  Thomas Schleiff/Mike Steglich
 */

#ifndef CMPL_INTERPRETER_HH
#define CMPL_INTERPRETER_HH

#include "CmplCore.hh"
#include "gencode.h"


namespace cmpl
{
	class CmplCore;
	class FileIO;
	class Compiler;

	/**
	 * The <code>Interpreter</code> class contains the main loop for executing the intermediary code
	 */
	class Interpreter
	{
	public:
		/**
		 * constructor
		 */
		Interpreter(CmplCore *m);
		//~Interpreter();

		/****** common variables ****/

		/**
		 * <code>glvar</code> is the main object of CMPL and contains all relevant 
		 * variables and constants 
		 */
		CmplCore *glvar;

		/**
		 * <code>fileIO</code> is used for error/warning messages 
		 */
		FileIO *fileIO;

		/**
		 * <code>compiler</code> is only needed for linearization of products of problem variables.
		 * It is used then for using its utility functions to generate the necessary additional intermediary code.
		 */
		Compiler *compiler;


		/****** methods ****/
		/**
		 * interpretes all the generated intermediary code by executing the main loop.
		 */
		void int_prog();

		/*
		 * executes a single command of the intermediary code.
		 * @param vp	pointer to the current command in the intermediary code
		 * @return		false if this is the END command so the main loop has to stop, otherwise true
		 */
		bool int_command(val_str *vp);

	private:
		/*
		 * tests if the current position in the intermediary code contains a valid command.
		 * @param vp	pointer to the current position in the intermediary code
		 * @return		true if vp points to a valid command, otherwise an error message is generated
		 */
		bool test_command(val_str *vp);
	};
}


#endif // !CMPL_INTERPRETER_HH

