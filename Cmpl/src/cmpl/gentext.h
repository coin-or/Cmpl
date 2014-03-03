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


/*************************************************************************************
 *
 * Texte, fuer sprachabhaengige Anpassungen.
 *
 *************************************************************************************/

#ifndef GENTEXT_H
#define GENTEXT_H

 
/* Fehlermeldungen */
#define ERROR_LINE_SYMBOL		"Invalid symbol '%s' in line name '%s'"
#define ERROR_LINE_NAME_LEN		"Line name '%s' is too long"
#define ERROR_LINE_NAME_DUP_REPL         "Line name '%s' is not unique: replaced by '%s'"
#define ERROR_VARPROD_MAX		"Too much variables in a product"
#define ERROR_VARPROD_1				"Products of variables are not allowed"
#define ERROR_SYMBOL_DEF			"Name '%s' is already defined"
#define ERROR_SYMBOL_NODEF			"Name '%s' is not defined"
#define ERROR_SYMBOL_INDEF			"Name '%s' is not complete defined"
#define ERROR_SYMBOL_TPK_MIX		"Name '%s' is already defined with another type class"
#define ERROR_ASSIGN_OLD			"Do not use '=' for an assignment"
#define ERROR_LOOP_MAX				"Too much loops"
#define ERROR_INDIZES_MAX			"Too much indexes"								
#define ERROR_IMPORT_PAR_MAX		"Too much parameters for IMPORT"
#define ERROR_CONST_DEF_DIM			"Dimensions in parameter definition don't match"
#define ERROR_CONST_DEF_TPK_MIX		"Mix of different type classes in one parameter definition" 
#define ERROR_CONST_DEF_SET_ELEM	"A set can contain only integers and strings, but no other sets" 
#define ERROR_NODEF_CONST			"Parameter '%s' is not defined"
#define ERROR_DEF_CONST				"Parameter '%s' is already defined"
#define ERROR_DEFTYPE_CONST			"Parameter '%s' is already defined with another type"
#define ERROR_NODEF_VAR				"Variable '%s' is not defined"
#define ERROR_DEF_VAR				"Variable '%s' is already defined"
#define ERROR_USED_VAR				"Variable '%s' is already used"
#define ERROR_DEF_VAR_KONST			"Name for product of variables '%s' is already defined as a parameter"
#define ERROR_DEF_VAR_LK			"Name for product of variables '%s' is already defined as an iteration parameter"
#define ERROR_DEF_RVAR_KONST		"Name for variable for range '%s' is already defined as a parameter"
#define ERROR_DEF_RVAR_LK			"Name for variable for range '%s' is already defined as an iteration parameter"
#define ERROR_MATRIX_FREE_IND		"Too much free indexes in matrix '%s'"
#define ERROR_MATRIX_PROD			"Error in matrix product at '%s'"
#define ERROR_MATRIX_PROD_MAX		"Too much matrix products at '%s'"
#define ERROR_MATRIX_PROD_UNV		"Incomplete matrix product"
#define ERROR_VEKTOR_LEN			"Vector length don't match at '%s'"
#define ERROR_VEKTOR_NO_LEN			"Vector length for the %d. index of '%s' missing"
#define ERROR_VEKTOR_ELEM			"Count of elements for the %d. index of '%s' is wrong"
#define ERROR_REK_SET_IND_MAX		"Too much recursiv indexes"
#define ERROR_VARPROD_IND_MAX		"Product of variables '%s * %s' has too much indexes"
#define ERROR_VARPROD_REAL_WARN		"Product of variables '%s * %s': both are real variables, no exact product is possible"
#define ERROR_VARPROD_NO_LB			"Product of variables: no lower bound for '%s'"
#define ERROR_VARPROD_NO_UB			"Product of variables: no upper bound for '%s'"
#define ERROR_LOOP_NOEND			"Unbounded loop"
#define ERROR_LOOP_WHILE			"while-modifier is not possible here"
#define ERROR_LOOP_ALT_EMPTY		"Alternative is not possible here"
#define ERROR_LOOP_ALT_NEVER		"Alternative in loop is never reached"
#define ERROR_LOOP_CTRL_REF			"Loop reference '%s' at %s not found"
#define ERROR_LOOP_ONCE_CONT		"continue is only valid in iterating loops"
#define ERROR_NOT_A_NUMBER_PARSE	"Operand for numerical operation is not a number"
#define ERROR_NOT_A_NUMBER			"Value '%s' is not a number"
#define ERROR_NOT_AN_INDEX			"Value '%s' is not a valid type for an index"
#define ERROR_NOT_TPK_VALUE			"Value of '%s' is not a simple value"
#define ERROR_NOT_TPK_SET			"Value of '%s' is not a set"
#define ERROR_CONV_IMPL				"Implicit type conversion from %s to %s is not possible"
#define ERROR_CONV_EXPL				"Type conversion from %s to %s is not permitted"
#define ERROR_INFINITE_SET			"Infinite set cannot be iterated"
#define ERROR_EMPTY_SET_FREE_IND	"Empty set for a free index at %s is not permitted"
#define ERROR_SET_NOT_SUPPORTED		"Result of set operation is a not supported set"
#define ERROR_CMP_TPK_MIX			"Mix of different type classes in a compare operation" 
#define ERROR_DIV_0					"Division by 0"
#define ERROR_MAX_0					"Maximum of 0 values"
#define ERROR_MIN_0					"Minimum of 0 values"
#define ERROR_SET_STEP_0			"Step value of 0 for algorithmic set"
#define ERROR_MULT_OBJECTIVE		"More than one line to optimize, only the first is used"
#define ERROR_INTERN_NO_FUNC		"Intern error: unknown intermediate function"
#define ERROR_INTERN_PRV_SET_IND	"Intern error: no previous SET_INDEXES"
#define ERROR_INTERN_STACK_UF		"Intern error: stack underflow"
#define ERROR_INTERN_NO_FREE_IND	"Intern error: no free index at '%s'"
#define ERROR_INTERN_KOMPLEX_LEN	"Intern error: wrong matrix length for complex bounds"
#define ERROR_INTERN_INCL_STACK		"Intern error: include file stack underflow"
#define ERROR_INTERN_INCL_CHDIR		"Intern error: change directory for include file"
#define ERROR_NO_MEMORY				"Out of memory"
#define ERROR_INT_MAX_MIN			"Integer parameter too high or too low for integer values"
#define ERROR_BOUND_DN_NO			"No lower bound for '%s': use default %ld"
#define ERROR_BOUND_UP_NO			"No upper bound for '%s': use default %ld"
#define ERROR_BOUND_UP_IGNORE		"Upper bound ignored"
#define ERROR_BOUND_DN_UP_COMP		"Lower bound (%lf) is greater than upper bound (%lf)"
#define ERROR_BOUND_UP_DN_COMP		"Upper bound (%lf) is less than lower bound (%lf)"
#define ERROR_BOUND_CONSTRAINT		"In constraint '%s' is the lower bound greater than the upper bound"
#define ERROR_VAR_DUP_REPL			"Variable name '%s' is not unique: replaced by '%s'"
#define ERROR_VAR_NAME_LEN			"Variable name '%s' is too long"
#define ERROR_IMPORT_CALL			"Error at calling IMPORT function"
#define ERROR_IMPORT_KOLIOP_TYPE	"Invalid type of Koliop file at IMPORT function"
#define ERROR_IMPORT_KOLIOP_PAR		"Invalid parameter at IMPORT function importing from Koliop"
#define ERROR_INCLUDE_MAX			"Too much recursiv include files"								
#define ERROR_PATH_TOO_LONG			"Path name too long"
#define ERROR_FILE_WRITE			"Write error on file '%s'"
#define ERROR_FILE_READ				"Read error on file '%s'"
#define ERROR_FILE_INCL_DIR_NF		"Directory '%s' for include file not found"
#define ERROR_FILE_INPUT_INCL		"Include file '%s' not found"
#define ERROR_FILE_INPUT_OPEN		"Input file '%s' not found\n"
/* stegger 1.3.1.b csv import */
#define ERROR_FILE_CSV_DIM			"CSV file '%s' with wrong dimension\n"
#define ERROR_FILE_OUT_OPEN			"Output file '%s' for -%c not to open\n"
#define ERROR_COUNT					"\n%d errors found\n"
/* stegger 1.3.1 math function error */
#define ERROR_MATH_FUNC				"Error in mathematical function %s"
#define ERROR_CONST_IN_OBJ_FUNC			"Objective function is not well formed - Contains a constant parameter, which is ignored."
#define ERROR_NO_MODEL                          "No model was created"




#ifdef SOLVERSERVICE
  #define ERROR_OBJ_FUNC			"Objective function not found."
  #define ERROR_NO_OBJ_FUNC			"No objective function found. "
#endif

/* Fehlerausgabe */
#define ERROR_TYP_TXT_USER			"user error"
#define ERROR_TYP_TXT_SONST			"error"
#define ERROR_TYP_TXT_SONST_FATAL	"fatal error"
#define ERROR_TYP_TXT_SONST_WARN	"warning"
#define ERROR_TYP_TXT_COMP			"error (compiler)"
#define ERROR_TYP_TXT_INT			"error (interpreter)"
#define ERROR_TYP_TXT_IO			"error (input/output)"

#define OUTTXT_ERROR				"%s%s: %s%s"
#define OUTTXT_ERROR_ZEILE			"%s%s: file %s line %d: %s%s"
#define OUTTXT_ERROR_TOKEN			"%s%s: file %s line %d at '%s': %s%s"

/* Liste der Variablenproduktzerlegung */
#define OUTTXT_PRODLIST_PROD		"product of variables: %s := %s * %s\n"
#define OUTTXT_PRODLIST_INT			"integer to binary:    %s := %s\n"
#define OUTTXT_PRODLIST_REAL		"real to binary:       %s := %s\n"


#endif // !GENTEXT_H

