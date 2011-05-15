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
 * The <code>Main</code> class is the main class of cmpl.
 * One instance of it exists while cmpl runs, every other object holds a pointer to it.
 * The <code>Main</code> class contains common used variables and functions to start cmpl.
 * @author  Thomas Schleiff/Mike Steglich
 */

#ifndef CMPL_MAIN_HH
#define CMPL_MAIN_HH


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <setjmp.h>
#include <string>
#include <time.h>

#include "gen.h"
#include "genstart.h"



/****** comparison modes for the lines of the result matrix ****/
#define MODE_N		1			///< not bounded
#define MODE_G		2			///< equals
#define MODE_O		3			///< upper bounded
#define MODE_U		4			///< lower bounded


/****** definition of common used structs ****/
//TODO: make own classes out of the structs where it make sense

namespace cmpl
{
	/****** types for values and commands of the intermediary code ****/
	/* Typen fuer Werte und Zwischencodebefehle */
	/* Zwischencodebefehl */

	/**
	 * command of the intermediary code.
	 */
	typedef struct {
		unsigned short cmd;         ///< command index (index in array code_str[])
		unsigned short sub;         ///< sub command number
		short par;          		///< embedded parameter
		unsigned short len;         ///< count of following parameter values
	} cmd_str;

	/**
	 * type for an address (index) in the intermediary code or in other arrays of values.
	 */
	typedef unsigned int t_addr;

	struct set_ea_str_s;			///< declaration of a struct for an enumeration set (TP_SET_ENUM) or an algorithmic set (TP_SET_ALG)
	struct array_str_s;				///< declaration of a struct for an array of values with integer and string indexes */

	/**
	 * value of an arbitrary data type.
	 */
	typedef union {
		cmd_str c;       	///< command of the intermediary code
		long i;             ///< integer value or index for an interned string
		double f;           ///< real number
		char *s;            ///< string value (not interned)
		struct set_ea_str_s *set;	///< pointer to an enumeration set or an algorithmic set
		struct array_str_s *array;	///< pointer to an array of values
		void *p;            ///< other pointer
		t_addr a;			///< address in the intermediary code
	} tp_u;

	/**
	 * type code, to determine which data type is in a <code>tp_u</code>.
	 * rule for the numbers:	the high byte codes the type class, the low byte distincts the types of this type class, the low byte of 0 means the type class itself.
	 *							but the high byte of 0 is not a type class, but several pseudo types.
	 *							types with the highest bit set in the low byte are not used for values, but only for type deviation.
	 *							for set types the bit pattern of the lower byte is used for special informations.
	 * rule for the names:		TPK_*	type class
	 *							TP_*	type for values
	 *							TPU_*	type only for type deviation
	 *							TPP_*	pseudo type
	 */
	enum etp {
		/* pseudo types */
		TPP_EMPTY 	= 0x00,		///< value is not set (<code>tp_u</code> is not used)
		TPP_NULL	= 0x01,		///< generic null value, interpreted depending of the context as 0, empty string or empty set (uses <code>tp_u.i</code> (which contains 0))
		TPP_MIX_TPK	= 0x02,		///< only used in the type deviation for mixed type classes

		TPP_CMD		= 0x10,		///< command of the intermediary code (uses <code>tp_u.c</code>)
		TPP_ADDR	= 0x11,		///< address in the intermediary code (uses <code>tp_u.a</code>)

		TPP_PTR		= 0x20,		///< other pointer (uses <code>tp_u.p</code>)

		/* types for the problem variables, not used in <code>val_str</code> */
		TPK_VAR		= 0x100,	///< type class for problem variables
		TP_VAR_DBL	= 0x111,	///< real number problem variable
		TP_VAR_INT	= 0x112,	///< integer number problem variable
		TP_VAR_BIN	= 0x113,	///< binary problem variable
		TP_VAR_NEW	= 0x181,	///< new defined problem variable, for which the type is changed later
		TP_VAR_F_NEW = 0x182,	///< for the first time new defined problem variable

		/* types for scalar values */
		TPK_VALUE	= 0x200,	///< type class for a scalar value
		TP_DBL		= 0x201,	///< floating point value (uses <code>tp_u.f</code>)
		TP_INT		= 0x202,	///< integer value (uses <code>tp_u.i</code>)
		TP_BIN		= 0x203,	///< binary value (uses <code>tp_u.i</code>, which is restricted to 0 or 1)
		TP_ISTR		= 0x211,	///< index of an interned string (uses <code>tp_u.i</code>)
		TP_STR		= 0x212,	///< pointer to a string, which is not interned (uses <code>tp_u.s</code>)

		TPU_NUM		= 0x281,	///< numeric value (floating point, integer or binary)

		/* types for sets */
		TPK_SET		= 0x300,	///< type class for a set value

		TP_SET_EMPTY   = 0x301,	///< empty set: contains no element

		TP_SET_ALG0_1  = 0x318,	///< algorithmic set, which contains all integer numbers between 0 and an upper bound (uses <code>tp_u.i</code> for the upper bound)
		TP_SET_ALG1_1  = 0x319,	///< algorithmic set, which contains all integer numbers between 1 and an upper bound (uses <code>tp_u.i</code> for the upper bound)
		TP_SET_ALG	   = 0x328,	///< algorithmic set (uses <code>tp_u.set</code> for the start value, the increment, and the end value)
		TP_SET_ALG_S1  = 0x329, ///< special type for an algorithmic set with an increment of 1, only for use in function LIT_SET (<code>construct_set()</code>)
		TP_SET_ENUM	   = 0x32c,	///< enumeration set (uses <code>tp_u.set</code> for the enumeration of its elements)
		TP_SET_SYM_ARR = 0x33c,	///< enumeration set of the indexes, for which an array has values (uses <code>tp_u.array</code> for the array)

		TP_SET_ALL	   = 0x34c,	///< infinite set, which contains all possible elements (all integer numbers and all strings)
		TP_SET_ALL_NUM = 0x348,	///< infinite set, which contains all integer numbers
		TP_SET_ALL_STR = 0x344,	///< infinite set, which contains all strings
		TP_SET_HALF_LB = 0x358,	///< infinite set, which contains all integer numbers greater or equal than an lower bound (uses <code>tp_u.i</code> for the lower bound)
		TP_SET_HALF_UB = 0x359,	///< infinite set, which contains all integer numbers lesser or equal than an upper bound (uses <code>tp_u.i</code> for the upper bound)

	};

	/**
	 * gets the type class for a type.
	 */
	#define TYPECLASS(tp)		((enum etp) ((int)(tp) & 0xff00))

	/**
	 * returns if the type is for a problem variable which has already a type.
	 */
	#define VAR_WITH_TYPE(tp)	(((int)(tp) & 0xfff0) == 0x110)
	/**
	 * returns if the type is for a problem variable which is new defined and has no type up to now.
	 */
	#define VAR_IS_NEW(tp)		(((int)(tp) & 0xfff0) == 0x180)

	/**
	 * returns if the type is for an infinite set (over which you cannot iterate).
	 */
	#define SET_IS_INFINITE(tp)	(((int)(tp) & 0xff40) == 0x340)			// .1.. ....
	/**
	 * returns if the type is for an empty set.
	 */
	#define SET_IS_EMPTY(tp)	(((int)(tp) & 0xfff0) == 0x300)			// 0000 ....
	/**
	 * returns if the type is for a set, which is complete described by its type (doesn't use <code>tp_u</code>).
	 */
	#define SET_USE_NO_PAR(tp)	(((int)(tp) & 0xff30) == 0x300)			// ..00 ....
	/**
	 * returns if the type is for a set, which uses <code>tp_u.i</code>.
	 */
	#define SET_USE_VALINT(tp)	(((int)(tp) & 0xff30) == 0x310)			// ..01 ....
	/**
	 * returns if the type is for a set, which uses the pointer <code>tp_u.set</code>.
	 */
	#define SET_USE_POINTER(tp)	(((int)(tp) & 0xff30) == 0x320)			// ..10 ....
	/**
	 * returns if the type is for a set, which uses the pointer <code>tp_u.array</code>.
	 */
	#define SET_USE_ARRAY(tp)	(((int)(tp) & 0xff30) == 0x330)			// ..11 ....
	/**
	 * returns if the type is for a set, which can contain string values as elements.
	 */
	#define SET_CAN_STRINGS(tp)	(((int)(tp) & 0xff04) == 0x304)			// .... .1..
	/**
	 * returns if the type is for a set, which can contain integer values as elements.
	 */
	#define SET_CAN_NUMBERS(tp)	(((int)(tp) & 0xff08) == 0x308)			// .... 1...



	/**
	 * value with its type.
	 */
	typedef struct {
		tp_u v;        		///< value
		enum etp t;         ///< type of the value
	} val_str;



	class IntFuncBase;
	/**
	 * description of a command of the intermediary code.
	 */
	typedef struct {
		const char *code_text;			///< name of the command
		IntFuncBase *exec_class;		///< object for execution the command by its <code>exec()</code> function
	} t_code_str;



	/**
	 * types for use in union <code>ptr_sym_u</code>.
	 */
	enum ptrtp {
		PTR_TP_CONST,			///< parameter value (uses <code>ptr_sym_u.c</code>)
		PTR_TP_VALUE,			///< simple value (uses <code>ptr_sym_u.s</code>)
		PTR_TP_VAR,				///< problem variable (uses <code>ptr_sym_u.v</code>)
		PTR_TP_ARRAY,			///< array of values or of problem variables (uses <code>ptr_sym_u.a</code>)
		PTR_TP_EMPTY			///< empty value (doesn't use <code>ptr_sym_u</code>)
	};

	/**
	 * gives the type in <code>enum ptrtp</code> for a type class out of <code>enum etp</code>.
	 */
	#define PTRTP_FOR_TYPECLASS(tpc)	(tpc==TPK_VAR ? PTR_TP_VAR : (tpc==TPK_VALUE ? PTR_TP_CONST : (tpc==TPK_SET ? PTR_TP_VALUE : PTR_TP_EMPTY)))



	/**
	 * parameter value.
	 */
	typedef struct {
		val_str c_val;			///< value
		enum etp tp;			///< explicit type (must then match <code>c_val.t</code>) / TPP_EMPTY: no explicit type
	} const_str;

	/**
	 * problem variable.
	 */
	typedef struct {
		enum etp v_type;			///< type of the problem variable (must be in type class TPK_VAR)
#ifdef VAR_PROD
		enum etp v_otype;			///< resulting type for the problem variable (must be in type class TPK_VAR)
#endif
		int v_defnum;				///< unique number in definition order
		bool v_is_koeff;			///< problem variable has at least one coefficient (not 0) in at least one line of the resulting matrix
		val_str v_koeff;			///< coefficient of the problem variable in the current line of the resulting matrix
		bool v_is_dn_b;				///< problem variable has a lower bound
		val_str v_dn_b;				///< lower bound for the problem variable (only used if <code>v_is_dn_b</code>)
		bool v_is_up_b;				///< problem variable has an upper bound
		val_str v_up_b;				///< upper bound for the problem variable (only used if <code>v_is_up_b</code>)
	} var_str;



	/**
	 * pointer to a value or to a vector of values.
	 */
	union ptr_sym_u {
		val_str *s;						///< simple value
		const_str *c;					///< parameter value
		var_str *v;						///< problem variable
		struct array_str_s *a;			///< array of values
	};

	/**
	 * attributes for a defined cmpl symbol (with a certain count of indexes).
	 */
	struct sym_attr_str
	{
		enum ptrtp t;				///< type of elements, which are referenced by the belonging pointer <code>union ptr_sym_u</code>
		bool s_newvar;				///< the symbol represents new defined problem variables
			/* (more attributes can added here, for instance for export and import) */
	};

	/**
	 * array of values.
	 */
	struct a_str {
		union ptr_sym_u array;		///< pointer to the vector with the values
		int *index;					///< vector for ascending ordered indexes of the elements in the vector pointed by <code>array</code> (not used for directly indexed elements)
		t_addr cap;					///< capacity of the vector pointed by <code>array</code> (also capacity for vector <code>index</code>)
		t_addr len;					///< last used element + 1 in the vector pointed by <code>array</code> (and also in vector <code>index</code>)
		t_addr cnt;					///< count of used elements in the vector pointed by <code>array</code>
	};

	/**
	 * cmpl array of parameter values or problem variables.
	 */
	typedef struct array_str_s {
		struct sym_attr_str attr;	///< attributes for the cmpl symbol
		struct a_str a_dir;			///< array for directly indexed elements (numerical index is directly used as vector index for <code>a_dir.array</code>)
		struct a_str a_num;			///< array for other (not directly indexed) numerical indexed elements (<code>a_num.index</code> contains the indexes)
		struct a_str a_str;			///< array for elements indexed by string values (<code>a_str.index</code> contains the numbers of the interned strings used as index)
		val_str defset;				///< definition set for the array (must not have the type TP_SET_SYM_ARR) / TPP_EMPTY: no explicit definition set (the definition set of an array contains all indexes of the array as elements)
	} array_str;

	/**
	 * scalar cmpl symbol for a parameter value or problem variable.
	 */
	typedef struct nonarray_str_s {
		struct sym_attr_str attr;	///< attributes for the cmpl symbol
		union ptr_sym_u elem;		///< pointer to the single element
	} nonarray_str;

	/**
	 * utility information for inserting an element in an array.
	 */
	typedef struct {
		int ins_ind_num;			///< insert position when inserting in <code>array_str.a_num</code> / -1: not set
		int ins_ind_str;			///< insert position when inserting in <code>array_str.a_str</code> / -1: not set
	} array_ins_info_str;

	/**
	 * symbol table element for a cmpl symbol.
	 */
	typedef struct {
		char s_name[SYM_NAME_LEN];			///< symbol name
		enum etp tp_sym;					///< type class of the symbol
		nonarray_str *val_ip0;				///< value for the scalar symbol
		array_str *val_ip[MAX_ANZ_IND];		///< arrays for the symbol with indexes (0: one index; 1: two indexes; ...)
	} symbol_str;


	/**
	 * interned strings (referenced by their number).
	 */
	typedef struct {
		const char **strings_ind;	///< vector of pointers to the strings; the index in this vector is the number of the interned string
		const char **strings_sort;	///< vector of pointers to the same strings, but sorted in ascending alphabetic order
		int *index;					///< vector of the indexes of the strings in <code>strings_ind</code> in the same order like in <code>strings_sort</code>
		int cap;					///< capacity of the vectors <code>strings_ind</code>, <code>strings_sort</code> and <code>index</code>
		int len;					///< last used element + 1 in the vectors <code>strings_ind</code>, <code>strings_sort</code> and <code>index</code>
	} strings_str;
}



// include "SetIter.hh" after the definition of common structs, but before using the definitions of classes for sets and iteration
#include "SetIter.hh"

/****** structs for sets and iteration ****/

namespace cmpl
{
	/**
	 * type of the body of a cmpl control structure.
	 */
	enum looptp {
		LOOP_EXEC,				///< the body of the control structure contains statements
		LOOP_SUM,				///< the body of the control structure is an expression, which is summed up (or other operations with the operator symbol '+')
		LOOP_SET,				///< the body of the control structure is an expression, which is an element of a set to construct
	};

	/**
	 * cmpl control structure.
	 */
	typedef struct {
		char *s_name;				///< name of the defined local parameter symbol / null: no local parameter symbol (only used by generating the intermediary code)
		char *l_name;				///< new defined prefix for the line name / null: no new prefix (only used by generating the intermediary code)
		char *line_name_pref;		///< full prefix for the line name / null: no prefix (only used by generating the intermediary code)
		char *line_name_post;		///< full postfix for the line name / null: no postfix (only used by generating the intermediary code)
		t_addr loop_start;			///< start address of the control structure in the intermediary code (only used by generating the intermediary code)
		t_addr iter_start;			///< start address of the iteration in the control structure (only used by generating the intermediary code)
		t_addr end_jump;			///< the jump to the end of the control structure must be inserted at this address (only used by generating the intermediary code)
		t_addr last_jump;			///< the jump to the next alternative condition in the control structure must be inserted at this address (only used by generating the intermediary code)
		enum looptp loop_tp;		///< type of the body of the control structure (only used by generating the intermediary code)
		bool jump_end;				///< dont jump back to the iteration start, but directly to the end of the control structure (only used by generating the intermediary code)
		bool base;					///< this is the starting control structure after a { (only used by generating the intermediary code)
		bool valid;					///< this entry is already valid (only used by generating the intermediary code)

		unsigned short sub;			///< type of the control structure (is a subcommand to LOOP_CTRL)
		unsigned short mod;			///< modificators (bitwise)
		val_str loop_val;			///< value in the control head (if the control structure is an iteration then the value is a set; if the control structure is a condition then the value is binary)
		SetIterator iter_set;		///< iteration object / iter_set.set.t == TPP_EMPTY: no iteration over a set
		short exec_num;				///< executed part of the control structure (0: nothing; 1: main body; -1: alternative default body; else: number of alternative body)
		bool start;					///< entry is new initialized before the first iteration or condition test
		bool is_lokal_assign;		///< current value in iter_set is assigned within the last execution of the control structure

		/**
		 * initialize all to 0.
		 */
		inline void init()
		{
			s_name = l_name = line_name_pref = line_name_post = (char *)0;
			loop_start = iter_start = end_jump = last_jump = 0;
			jump_end = base = valid = false;
			sub = mod = 0;
			loop_val.t = iter_set.set.t = TPP_EMPTY;
			exec_num = 0;
			start = is_lokal_assign = false;
		}
	} loop_ctrl_str;


	/**
	 * structure for one entry within a line of the result matrix.
	 */
	typedef struct {
		int le_col;					///< column number
		val_str le_val;				///< coefficient for the result matrix
	} l_elem_str;


	/**
	 * structure for a bound.
	 */
	typedef struct {
		bool b_is_b;				///< bound exists
		val_str b_val_b;			///< value of the bound
	} bound_str;


	/**
	 * structure for a line of the result matrix.
	 */
	typedef struct d_str {
		int d_lineno;				///< number of the line
		char d_name[DST_NAME_LEN_1];	///< name for the line
		int d_mode;					///< compare operation for the line
		int d_line_obj_to;			///< only for an objective line: direction of optimization (1: maximize; -1: minimize; 0: not specified)
		val_str d_fonds;			///< fonds of the line

		int d_anz_val;				///< count of coefficients not 0 in the line
		l_elem_str *d_valp;			///< info for coefficients not 0

		bool d_is_range;			///< line is both side bounded
#ifdef RANGE
		val_str d_range;			///< value for the range
#else
		var_str *d_range_vp;		///< inserted variable for modelling the range
#endif
		struct d_str *d_nextline;	///< next line of the result matrix
	} dest_str;


	/**
	 * structure of a problem variable for the result matrix.
	 */
	typedef struct {
		char n_name[DST_NAME_LEN_1];	///< name for the problem variable in the result matrix
		enum etp n_type;			///< type of the variable (type class TPK_VAR)
		bool n_is_koeff;			///< variable has a coefficient not 0
		bool n_is_dn_b;				///< variable has a lower bound
		val_str n_dn_b;				///< lower bound
		bool n_is_up_b;				///< variable has an upper bound
		val_str n_up_b;				///< upper bound
		int n_akt_pos;				///< position in the list of the active variables
	} name_str;


	/**
	 * structure for a matrix multiplication in a term (for an implicit loop)
	 */
	struct term_ind_str {
		t_addr ti_beg_term;			///< start address of the term in the intermediary code
		bool ti_is_floop;			///< first iteration in the implicit loop
		bool ti_is_gcol;			///< implicit loop over a free column vector for the whole line
		bool ti_is_colexp;			///< next free vector for a column expected
		int ti_anz_prod;			///< count of matrix products

		SetIterator iter_set[MAX_ANZ_VPROD];	///< iteration object for the implicit loops
	};


	/**
	 * structure for the problems variables within a term.
	 */
	struct term_var_str {
		int tv_anz_var;				///< count of variables in the term
		//TODO: tv_str should be an independent struct
		struct tv_str {
			var_str *tv_varp;				///< pointer to the variable
			int tv_sym_num;					///< symbol number of the variable
			val_str tv_inds[MAX_ANZ_IND];	///< index values of the variable, can only be TP_INT or TP_ISTR
		}
#ifdef VAR_PROD
		tv_vars[MAX_ANZ_VAR_PROD];	///< info for the variables (if more than one then it is a product of variables)
#else
		tv_var;						///< info for the one variable (more than one are not possible)
#endif
	};


#ifdef VAR_PROD

	/**
	 * structure for binary disaggregation of an integer problem variable.
	 * (also used for normalization of a real problem variable)
	 */
	struct var_split_str {
		struct term_var_str::tv_str vs_ivar;		///< variable to disaggregate
		struct term_var_str::tv_str vs_bvars;		///< disaggregation (last index in <code>vs_bvars.tv_inds</code> is free and set to the maximum)
		struct var_split_str *vs_next_vs;			///< next element in a list
	};


	/**
	 * structure for products of problem variables.
	 */
	struct var_prod_str {
		struct term_var_str::tv_str vp_fakt1;		///< first variable in the product
		struct term_var_str::tv_str vp_fakt2;		///< second variable in the product
		struct term_var_str::tv_str vp_prod;		///< vp_prod = vp_fakt1 * vp_fakt2
		struct var_prod_str *vp_next_vp;			///< next element in a list
	};

#endif /* VAR_PROD */


	/**
	 * structure for index information for a single index.
	 */
	typedef struct {
		bool is_free;								///< index is free (i.e. not a single value, but a set of values)
		SetIterator iter_set;						///< iteration context for the set of a free index
		long cur_val;								///< index value (current index value if iterating over a free index)
		enum etp cur_tp;							///< type of <code>cur_val</code> (can only be TP_INT or TP_ISTR)
	} ind_inf_one_str;

	/**
	 * structure for all index information for a symbol.
	 */
	typedef struct {
		char i_anz_ind;								///< count of indexes
		char i_free_ind;							///< count of free indexes
		char i_akt_ind;								///< number of current index
		bool i_is_transp;							///< array is treated as transposed matrix
		ind_inf_one_str i_ind[MAX_ANZ_IND];			///< info for the indexes
	} ind_inf_str;


	/**
	 * structure for processing a line with an implicit loop.
	 */
	struct line_vekt_str {
		t_addr lv_beg_line;							///< start address of the line in the intermediary code
		bool lv_is_floop;							///< first loop of the iteration
		bool lv_is_vekt;							///< current line has an implicit loop

		SetIterator iter_set;						///< iteration context for the set of the implicit loop
	};


	//TODO: use generic standard container classes instead of these own structures.
	/**
	 * structure for a list of floating point numbers, used for writing xml-output files.
	 */
	/*
	struct double_list {
		double f;
		double_list *next;
	};
	*/

	/**
	 * structure for a list of integers, used for writing xml-output files.
	 */
	struct long_list {
		long i;
		long_list *next;
	};

	/**
	 * structure for a list of strings, used for writing xml-output files.
	 */
	struct char_list {
		char c[101];
		char_list *next;
	};
}



namespace cmpl
{
	class FileIO;
	class Interpreter;

	/**
	 * The <code>Main</code> class contains global variables und common functions.
	 * It exists one instance of this class.
	 */
	class Main
	{
	public:
		/**
		 * constructor
		 */
		Main();
		//~Main();

		/****** common variables ****/

		/**
		 * <code>glvar</code> points to this.
		 * defined here because the other classes have the same instance variable, so macros can rely on this in all classes.
		 */
		Main *glvar;

        /**
		 * <code>fileIO</code> is used for input and output of data and for error/warning messages.
		 */
		FileIO *fileIO;

		/**
		 * object for processing the intermediary code, only set while this processing.
		 */
		Interpreter *interpreter;

		/**
		 * utility object for sets for common use.
		 */
		SetUtil setUtil;


	public:
		aufruf_str *aufruf_p;							///< call parameters
		bool is_protokoll;								///< write protocol output

		jmp_buf *jmp_env;								///< used for abort if a fatal error occurs

		val_str *prog;									///< array for the intermediary code
		t_addr comp_ptr;								///< current address in the intermediary code while compiling this code
		t_addr int_ptr;									///< current address in the intermediary code while processing this code
		t_addr prog_max;								///< reserved length of the array <code>prog</code>
		
		void **mem_alloc;								///< array with pointers to global allocated memory
		t_addr mem_alloc_akt;							///< current position in <code>mem_alloc</code>
		t_addr mem_alloc_max;							///< reserved length of the array <code>mem_alloc</code>

		int incl_stack_ptr;								///< stack pointer in the stack for include while scanning the input cmpl file
		std::string incl_file_name[MAX_INCL_STACK];		///< stack for include: file name without path
#ifdef CHDIR
		std::string incl_dir_name[MAX_INCL_STACK];		///< stack for include: path name
		std::string dir_start_name;						///< path name of the starting directory
		char dir_sep_char;								///< path separator char
#endif

		const char *akt_src_file;						///< current input cmpl file
		int akt_src_line;								///< current line in the input cmpl file

		int anz_comp_error;								///< count of errors while parsing the input cmpl file
		int anz_int_error;								///< count of errors while processing the intermediary code
		int anz_warning;								///< global count of warnings
		int anz_error;									///< global count of errors

		char errbuf[ERRBUFLEN+1];						///< common buffer for error messages
		char temp_buf[TEMPBUFLEN+1];					///< common buffer for temporary strings

		val_str *value_stack;							///< value stack for processing the intermediary code
		t_addr stack_ptr;								///< stack pointer for <code>value_stack</code>
		t_addr stack_max;								///< reserved length of the array <code>value_stack</code>
		
		bound_str var_dn_b;								///< lower bound within a definition of problem variables
		bound_str var_up_b;								///< upper bound within a definition of problem variables

		int akt_defnum;									///< count of defined problem variables

		symbol_str *symbol_table;						///< symbol table for cmpl parameters and problem variables
		t_addr symbol_anzahl;							///< count of symbols
		t_addr symbol_max;								///< reserved length of the array <code>symbol_table</code>
		symbol_str *symbol_akt_p;						///< pointer to the current symbol

		strings_str strings;							///< structure for interned strings

#ifdef VAR_PROD
		loop_ctrl_str tmp_table[MAX_ANZ_TMP+1];			///< iteration contexts for loops (one extra implicit loop for products of problem variables)
#else
		loop_ctrl_str tmp_table[MAX_ANZ_TMP];			///< iteration contexts for loops
#endif
		int tmp_level;									///< current depth in cmpl control structures

		bool neg_fak_term;								///< all coefficients in the current terms are to negate

		dest_str *dest_grd;								///< start pointer to the list of generated matrix lines
		dest_str *dest_akt;								///< current matrix line
		dest_str *old_dest_akt;							///< previous matrix line

		name_str *dest_names;							///< problem variables as columns for the result matrix, in order of the columns

		struct term_ind_str term_ind_bl;				///< info for a matrix product in the current term
		struct term_var_str term_var_bl;				///< problem variables in the current term

		ind_inf_str index_info;							///< info of indexes while parsing the input cmpl file
		ind_inf_str ind_inf_table[MAX_IND_INF];			///< stack for index infos while processing the intermediary code
		ind_inf_str *ind_inf_ptr;						///< current index info in <code>ind_inf_table</code>

		int akt_s_line;									///< number of the current line of the result matrix
		val_str fonds_s_line;							///< right hand side of the current line of the result matrix
		int mode_s_line;								///< comparison mode (MODE_N, MODE_G, MODE_O or MODE_U) of the current line of the result matrix
		char name_s_line[ZC_NAME_LEN];					///< name of the current line of the result matrix
		char org_name_s_line[ZC_NAME_LEN];				///< original name of the current line of the result matrix
		bool is_left_side;								///< processing of the left side for the current line
		int akt_line_side;								///< number of current side for the current line
		t_addr beg_akt_line;							///< start address of the current line in the intermediary code
		struct line_vekt_str line_vekt_bl;				///< info if the current line has an implicit loop
		int akt_line_obj_to;							///< optimization direction for the current line (0:nothing, 1:maximize, -1:minimize)

		char last_str[COMP_STR_LEN+1];					///< Parsing cmpl file: last scanned string
		int exp_transp;									///< Parsing cmpl file: mode for transpose (1: following T means transpose; 0: T is symbol name)
		unsigned short last_bef;						///< Parsing cmpl file: temporary store for a command for the intermediary code
		int sect;										///< Parsing cmpl file: current section in the cmpl file
		bool is_left;									///< Parsing cmpl file: parsing left hand side of a line
		bool is_scann_const_vekt;						///< Parsing cmpl file: reading values for an array of cmpl parameters

		val_str *gl_mline_vp;							///< finish processing for a matrix line: array with coefficients
		bool *gl_mline_fp;								///< finish processing for a matrix line: array with flag if coefficient is not 0
		int gl_mline_anz;								///< finish processing for a matrix line: count of coefficients not 0

		int gl_mline_defnum;							///< finish processing for a matrix line: number of a problem variable to search for
		var_str *gl_mline_varptr;						///< finish processing for a matrix line: found problem variable to <code>gl_mline_defnum</code>

#ifdef VAR_PROD
		t_addr comp_end;								///< address after the intermediary code
		int vp_sn_num;									///< counter for short names for new generated variables

		struct var_split_str *var_split_grd;			///< start pointer to the list for binary dissection for an integer problem variable
		struct var_prod_str *var_prod_grd;				///< start pointer to the list for products of two problem variables 

		int pvarb_s_line;								///< line number in the result matrix where linearization of products of problem variables starts
		char name_pvarb_line[ZC_NAME_LEN];				///< line name in the result matrix for a linearization of products of problem variables
#endif

		t_code_str *code_table;							///< static table of objects implementing <code>IntFuncBase.exec()</code> for executing commands of the intermediary code



		/****** functions ****/
	public:

		/**
		 * Starting point of execution, with evaluation of the call parameters.
		 * @param argc			count of call parameters
		 * @param argv			array with call parameters
		 * @return				error code or 0 for successful execution
		 */
		int start_args(int argc, char *argv[]);

		/**
		 * Starting point of execution.
		 * @param aufruf_par	call parameters
		 * @return				error code or 0 for successful execution
		 */
		int start_par(aufruf_str *aufruf_par);


		/**
		 * allocates memory and stores this allocation for global memory management.
		 * @param size			size of memory to allocate
		 * @return				pointer to allocated memory
		 */
		void *c_getmem(size_t size);

		/**
		 * stores an allocation of memory for the global memory management.
		 * @param c				pointer to allocated memory
		 */
		void ins_mem(void *c);

		/**
		 * enlarge an allocated memory block.
		 * @param mem_base		pointer to the allocated memory
		 * @param mem_max		pointer to the count of elements in the allocated memory
		 * @param mem_inc		increment for the count of elements
		 * @param size			size of one element
		 */
		void incmem(void **mem_base, t_addr *mem_max, t_addr mem_inc, size_t size);

		/**
		 * frees allocated memory which is stored in the global memory management.
		 * @param mem_alloc_cnt	number of allocation in the global memory management
		 */
		void freemem(t_addr mem_alloc_cnt);

		/**
		 * frees all allocated memory which is stored in the global memory management.
		 */
		void freemem_all();


		/**
		 * search and insert a string to the interned strings.
		 * @param valp			return of the string as interned string
		 * @param str			input string
		 * @param copy			copy the string to new allocated memory before inserting to the interned strings
		 * @return				true if string <code>str</code> is direct inserted to the interned strings (not a copy of it)
		 */
		bool string_to_istr(val_str *valp, const char *str, bool copy);


		/**
		 * rounds a floating point value to an integer value.
		 * @param x				floating point value
		 * @param fl			check if <code>x</code> is in the valid range for integer values
		 * @return				integer value
		 */
		long rund(double x, bool fl);


	private:
		/**
		 * initialize <code>this</code>.
		 */
		void init();

		/**
		 * parse call parameters and fill them in structure <code>aufruf_str</code>.
		 * @param aufruf_par	structure to fill
		 * @param argc			count of call parameters
		 * @param argv			array with call parameters
		 * @return				success
		 */
		bool parse_args(aufruf_str *aufruf_par, int argc, char *argv[]);

		/**
		 * initialize instance variables and allocate memory for arrays.
		 * @param aufruf_par	call parameters
		 */
		void set_vals(aufruf_str *aufruf_par);

		/**
		 * delete objects in the array <code>code_table</code>
		 */
		void delete_code_table();

		/**
		 * initialize the structure for call parameters with default values.
		 * @param aufruf_par	call parameters
		 */
		void init_aufruf_str(aufruf_str *aufruf_par);

		/**
		 * check and complete the structure for call parameters with default values.
		 * @param aufruf_par	call parameters
		 */
		bool fill_aufruf_str(aufruf_str *aufruf_par);

		/**
		 * frees the structure for call parameters.
		 * @param aufruf_par	call parameters
		 */
		void free_aufruf_par(aufruf_str *aufruf_par);

		/**
		 * print a short usage info.
		 * @param name			call name of cmpl program
		 * @param param			call parameter with an error or null if no specific parameter
		 * @param err			print usage because of an error
		 * @return				ever false, meaning execution is cancelled
		 */
		bool usage(const char *name, const char *param, const char *err);


		/**
		 * main function for parsing the cmpl input file.
		 */
		void comp_main();

		/**
		 * main function for processing the intermediary code to generate the result matrix.
		 */
		void int_main();
	};
}


/****** macro definitions for use in the processing of the intermediary code ****/
//TODO: move to a more specific header file

#define RUND(x)			glvar->rund(x, true)			///< round a floating point number to an integer

#define EPS_EQ			0.00001							///< used for check if two floating point numbers are equal
#define IS_EQ(d1,d2)	(d1-d2<EPS_EQ && d2-d1<EPS_EQ)	///< check if two floating point numbers are equal

#define INT_VAL(x)		((x).t==TP_DBL ? RUND((x).v.f) : (x).v.i)			///< cast a numeric value from <code>val_str</code> to an integer
#define DBL_VAL(x)		((x).t==TP_DBL ? ((x).v.f) : ((double)((x).v.i)))	///< cast a numeric value from <code>val_str</code> to a double



/****** macro definitions for writing in the common error buffer ****/
#define SPRINT_ERR_1(a)				snprintf((glvar)->errbuf, ERRBUFLEN, a); (glvar)->errbuf[ERRBUFLEN] = '\0'
#define SPRINT_ERR_2(a,b)			snprintf((glvar)->errbuf, ERRBUFLEN, a,b); (glvar)->errbuf[ERRBUFLEN] = '\0'
#define SPRINT_ERR_3(a,b,c)			snprintf((glvar)->errbuf, ERRBUFLEN, a,b,c); (glvar)->errbuf[ERRBUFLEN] = '\0'
#define SPRINT_ERR_4(a,b,c,d)		snprintf((glvar)->errbuf, ERRBUFLEN, a,b,c,d); (glvar)->errbuf[ERRBUFLEN] = '\0'

/****** macro definitions for writing in the common temporary string buffer ****/
#define SPRINT_TEMP_1(a)			snprintf((glvar)->temp_buf, TEMPBUFLEN, a); (glvar)->temp_buf[TEMPBUFLEN] = '\0'
#define SPRINT_TEMP_2(a,b)			snprintf((glvar)->temp_buf, TEMPBUFLEN, a,b); (glvar)->temp_buf[TEMPBUFLEN] = '\0'
#define SPRINT_TEMP_3(a,b,c)		snprintf((glvar)->temp_buf, TEMPBUFLEN, a,b,c); (glvar)->temp_buf[TEMPBUFLEN] = '\0'
#define SPRINT_TEMP_4(a,b,c,d)		snprintf((glvar)->temp_buf, TEMPBUFLEN, a,b,c,d); (glvar)->temp_buf[TEMPBUFLEN] = '\0'

/****** macro definitions for output over the common temporary string buffer ****/
#define OUT_TEMP_1(x,a)				{ SPRINT_TEMP_1(a); glvar->fileIO->output_temp_string(x); }
#define OUT_TEMP_2(x,a,b)			{ SPRINT_TEMP_2(a,b); glvar->fileIO->output_temp_string(x); }
#define OUT_TEMP_3(x,a,b,c)			{ SPRINT_TEMP_3(a,b,c); glvar->fileIO->output_temp_string(x); }
#define OUT_TEMP_4(x,a,b,c,d)		{ SPRINT_TEMP_4(a,b,c,d); glvar->fileIO->output_temp_string(x); }



/****** macro definitions for output to the protocol file ****/
#ifdef PROTO
#define PROTO_PREF				{ if((glvar)->is_protokoll) { \
										io_type_str *_ios = (glvar)->aufruf_p->output + OUTPUT_PROTOKOLL;

#define PROTO_OUT_1(a)          PROTO_PREF \
										if(_ios->io_type == IO_TYPE_FILE) fprintf(_ios->file_fp, a);\
										else if(_ios->io_type != IO_TYPE_NO) OUT_TEMP_1(OUTPUT_PROTOKOLL,a) } }
#define PROTO_OUT_2(a,b)        PROTO_PREF \
										if(_ios->io_type == IO_TYPE_FILE) fprintf(_ios->file_fp, a,b);\
										else if(_ios->io_type != IO_TYPE_NO) OUT_TEMP_2(OUTPUT_PROTOKOLL,a,b) } }
#define PROTO_OUT_3(a,b,c)      PROTO_PREF \
										if(_ios->io_type == IO_TYPE_FILE) fprintf(_ios->file_fp, a,b,c);\
										else if(_ios->io_type != IO_TYPE_NO) OUT_TEMP_3(OUTPUT_PROTOKOLL,a,b,c) } }
#define PROTO_OUT_4(a,b,c,d)    PROTO_PREF \
										if(_ios->io_type == IO_TYPE_FILE) fprintf(_ios->file_fp, a,b,c,d);\
										else if(_ios->io_type != IO_TYPE_NO) OUT_TEMP_4(OUTPUT_PROTOKOLL,a,b,c,d) } }
#else
#define PROTO_OUT_1(a)
#define PROTO_OUT_2(a,b)
#define PROTO_OUT_3(a,b,c)
#define PROTO_OUT_4(a,b,c,d)
#endif


#endif // !CMPL_MAIN_HH

