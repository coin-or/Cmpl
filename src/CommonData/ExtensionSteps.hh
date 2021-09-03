
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


#ifndef EXTENSIONSTEPS_HH
#define EXTENSIONSTEPS_HH


/************** definition of execution steps, for which extensions can be registered **********/

// Precompiler
#define EXT_CMDLOPT_PRECOMP_SIMPLE                  100         ///< command line option simply to deliver from precompiler module to the extension object
#define EXT_STEP_PRECOMP_REG_DUMMY                  101         ///< dummy for unique registration for the precompiler module
#define EXT_CMDLOPT_PRECOMP_PROC_DATA               111         ///< process command line option for external data import

// Compiler
#define EXT_STEP_COMPILE_PREDEF_SYMBOLS				201			///< define predefined symbols or pseudo symbols (additional parameter is CompilerContext object)

// Interpreter
#define EXT_CMDLOPT_INTERPRET_SIMPLE                300         ///< command line option simply to deliver from interpreter module to the extension object
#define EXT_STEP_INTERPRET_PREDEF_SYMBOLS_INIT		301			///< initialize predefined symbols for module Interpreter (additional parameter is entry in symbol table)
#define EXT_STEP_INTERPRET_PSEUDO_SYMBOL_OBJ        302         ///< get new instance of class for functionality of a pseudo symbol (additional parameter is ValSpecialBase**)
#define EXT_STEP_INTERPRET_INIT_FINAL               311         ///< finish of initialization, before execution of intermediary code starts
#define EXT_STEP_INTERPRET_EXTDATA_IMPORT           321         ///< import data from an external data source
#define EXT_STEP_INTERPRET_MODELPROP                361         ///< set optimization model properties (additional parameter is OptModel::Properties*)
#define EXT_STEP_INTERPRET_SOLVERSEL                371         ///< setting command line options for solver selection based on properties of the generated optimization model
#define EXT_STEP_INTERPRET_REMODEL                  391         ///< linearisation or other remodeling of constraints and objectives

// OutModel
#define EXT_STEP_OUTMODEL_SPECIAL_INFO              401         ///< get info for special extension data for the optimization model (additional parameter is list<OutModelExtDataBase::Info>*)



#endif // EXTENSIONSTEPS_HH

