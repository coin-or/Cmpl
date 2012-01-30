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
 * The <code>IntFuncBase</code> class is the abstract base class
 * for all classes which are used for the execution of a command of the intermediary code.
 * Also it contains utility functions for executing commands of the intermediary code.
 * This class and all deviated classes are used by the main loop of the execution
 * of the intermediary code which is defined in the <code>Interpreter</code> class.
 * @author  Thomas Schleiff/Mike Steglich
 */

#ifndef CMPL_INTFUNCBASE_HH
#define CMPL_INTFUNCBASE_HH

#include "CmplCore.hh"
#include "FileIO.hh"


namespace cmpl
{
	/**
	 * The <code>IntFuncBase</code> class is the abstract base class for executing a command of the intermediary code.
	 * For every command of the intermediary code there is a class deviated from <code>IntFuncBase</code>,
	 * which overrides the <code>exec</code> function to execute the given command.
	 */
	class IntFuncBase
	{
	protected:
		/**
		 * constructor
		 */
		inline IntFuncBase(CmplCore *m) { glvar = m; }

		/****** common variables ****/

	public:
		/**
		 * <code>glvar</code> is the CmplCore object of CMPL and contains all relevant 
		 * variables and constants 
		 */
		CmplCore *glvar;

		/****** methods ****/

		/**
		 * abstract function for executing a command of the intermediary code.
		 * @param vp	pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp) = 0;


		/****** utility functions for stack handling ****/

	public:
		/**
		 * pushes an integer number to the value stack.
		 * @param v		integer value
		 */
		void push_int(long v);

		/**
		 * pushes a floating point number to the value stack.
		 * @param v		floating point value
		 */
		void push_dbl(double v);

	protected:
		/**
		 * pushes a value to the value stack.
		 * @param p		pointer to the value
		 */
		void push_val(val_str *p);

		/**
		 * tests if a number is in the range for integer numbers and pushes the number as an integer number or a floating point number.
		 * @param v			number
		 * @param to_lng	if the number is in the range for integer numbers then push as an integer
		 */
		void push_rund(double v, bool to_lng);

		/**
		 * increments the stack pointer (i.e. pushs the last poped value back to the stack)
		 */
		void repush();
		
		/**
		 * pops a value from the value stack.
		 * @param p		pointer for returning the value
		 */
		void pop_val(val_str *p);

		/**
		 * pops an integer number from the value stack.
		 * The value from the stack must be numeric, if it is a floating point number it is rounded.
		 * @return		integer number
		 */
		long pop_int();
		
		/**
		 * pops a list from the value stack.
		 * The top value from the stack must be a list counter.
		 * @param cntp		returns the count of elements in the list
		 * @param lenp		returns the count of value elements in the return array
		 * @return			start of the array of the elements (this points directly to the stack, so a push overrides the values)
		 */
		val_str *pop_list(long *cntp, long *lenp);

		/**
		 * pops a list counter from the value stack.
		 * The value from the stack must be a list counter.
		 * @param lstp		returns whether the list can contain other lists
		 * @return			list count
		 */
		long pop_list_cnt(bool *lstp);

		/**
		 * pops the elements of a list from the value stack.
		 * The top value from the stack must be the last list element.
		 * @param cnt		count of elements in the list
		 * @param lst		flag whether the list can contain other lists
		 * @param lenp		returns the count of value elements in the return array
		 * @param dp		returns the max recursion depths of lists (0: no lists in the list)
		 * @return			start of the array of the elements (this points directly to the stack, so a push overrides the values)
		 */
		val_str *pop_list_elems(long cnt, bool lst, long *lenp, int *dp);

		/**
		 * pops a value from the value stack, which can used as an index for cmpl arrays.
		 * The value from the stack must be numeric or a string, if it is a floating point number it is rounded.
		 * @param tp	pointer to return the type of the resulting index value, can be only <code>TP_INT</code> or <code>TP_ISTR</code>
		 * @return		integer number or index of a string.
		 */
		long pop_index(enum etp *tp);

		/**
		 * tests a value if it can be an index value for cmpl arrays.
		 * The value must be numeric or a string, if it is a floating point number it is rounded.
		 * @param valp	pointer to the value which is tested
		 * @param tp	pointer to return the type of the resulting index value, can be only <code>TP_INT</code> or <code>TP_ISTR</code>
		 * @return		integer number or index of a string.
		 */
		long test_val_index(val_str *valp, enum etp *tp);


		/****** utility functions for array handling ****/

	public:
		/**
		 * searches a indexed element in an array.
		 * @param arr_p		array in which is searched
		 * @param ind		index of the searched element
		 * @param ind_tp	type of the index, can be only TP_INT or TP_ISTR
		 * @param found		returns if the element is found
		 * @param ret_ins_p	returns info where you must insert a not found element, use null if this info is not needed
		 * @return			found element
		 */
		union ptr_sym_u search_ind_elem(array_str *arr_p, long ind, enum etp ind_tp, bool *found, array_ins_info_str *ret_ins_p);

		/**
		 * searches an int value in an array, who has an corresponding ascending sorted index array.
		 * @param val_arr	array in which is searched
		 * @param index_arr	ascending sorted index array
		 * @param len		length of the array
		 * @param val		searched value
		 * @return			index of the found value or -1 if not found
		 */
		int search_int_sort_val_array(val_str *val_arr, int *index_arr, int len, long val);

		/**
		 * returns an appropriate pointer to an element in an array of a given type.
		 * @param ptr		pointer to the start of the array
		 * @param tp		type of the elements in the array
		 * @param ind		index of the element in the array
		 * @param found		returns if the element is found, i.e. the element in the array is not void
		 * @return			pointer to the element
		 */
		union ptr_sym_u ptr_symbol_elem(union ptr_sym_u *ptr, enum ptrtp tp, long ind, bool *found);

	protected:
		/**
		 * searches an element in a multidimensional array; the info of indexes of the element must be given in glvar->ind_inf_ptr.
		 * @param s_p		pointer to the symbol structure which contains the arrays
		 * @param array		array in which is searched; not used on start of the recursive search
		 * @param mi		step of recursion; use 0 for start of the recursive search
		 * @param found		returns if the element is found
		 * @param ptp		returns type of the elements in the array
		 * @return			pointer to the element
		 */
		union ptr_sym_u ffetch_p(symbol_str *s_p, array_str *array, int mi, bool *found, enum ptrtp *ptp);

		/**
		 * inserts an indexed element in an array, or updates the element if it already exists.
		 * @param arr_p		array in which the element is inserted
		 * @param ind		index for the element
		 * @param ind_tp	type of the index, can be only TP_INT or TP_ISTR
		 * @param iter_p	iterator for other indexes which are later to insert, to grow the array appropriate; null if no other known indexes
		 * @param ins_info	info where the element is to insert, use null if not known
		 * @return			inserted element
		 */
		union ptr_sym_u insert_ind_elem(array_str *arr_p, long ind, enum etp ind_tp, SetIterator *iter_p, array_ins_info_str *ins_info);

		/**
		 * searches an int value in an ascending sorted array.
		 * @param index_arr	ascending sorted array of int values
		 * @param len		length of the array
		 * @param val		searched value
		 * @return			index of the found value or index of the place where the value must be inserted
		 */
		t_addr search_int_sort(int *index_arr, int len, int val);

		/**
		 * marks an element in an array as void.
		 * @param ptr		pointer to start of the array
		 * @param tp		type of the elements in the array
		 * @param ind		index of the element
		 */
		void set_ind_elem_empty(union ptr_sym_u *ptr, enum ptrtp tp, long ind);

		/**
		 * returns a void pointer to an element in an array.
		 * @param ptr		pointer to start of the array
		 * @param tp		type of the elements in the array
		 * @param ind		index of the element
		 * @param size_ret	returns the size of the element
		 * @return			pointer to the element
		 */
		void* ptr_symbol_void(union ptr_sym_u *ptr, enum ptrtp tp, long ind, size_t *size_ret);

		/**
		 * sets a union ptr_sym_u pointer for a given type from a void pointer.
		 * @param ptr		result union in which the pointer is set
		 * @param tp		type of the element
		 * @param vp		void pointer
		 */
		void set_ptr_sym_u(union ptr_sym_u *ptr, enum ptrtp tp, void *vp);


		/****** utility functions for type conversion ****/

		/**
		 * converts a value to a value of a given type.
		 * @param vs		source value
		 * @param vd		return of the result value
		 * @param tp		type for the result value
		 * @param format	format for converting to a string value or null
		 * @param expl_conv	true: user type conversion; false: internal type conversion
		 */
		void fconv_to_etp(val_str *vs, val_str *vd, enum etp tp, char *format, bool expl_conv);

		/**
		 * converts a value to a floating point value.
		 * @param vs		source value
		 * @param expl_conv	true: user type conversion; false: internal type conversion
		 * @return			floating point value
		 */
		double fconv_to_dbl(val_str *vs, bool expl_conv);

		/**
		 * converts a value to an integer value.
		 * @param vs		source value
		 * @param expl_conv	true: user type conversion; false: internal type conversion
		 * @param conv_num	source value is a numeric value, refers only to error messages
		 * @return			integer value
		 */
		long fconv_to_int(val_str *vs, bool expl_conv, bool conv_num);

		/**
		 * converts a value to a binary value.
		 * @param vs		source value
		 * @param expl_conv	true: user type conversion; false: internal type conversion
		 * @return			binary value
		 */
		bool fconv_to_bin(val_str *vs, bool expl_conv);

		/**
		 * converts a value to a string value.
		 * @param vs		source value
		 * @param format	format for converting to a string value or null
		 * @param expl_conv	true: user type conversion; false: internal type conversion
		 * @param alloc		returns if new memory is allocated for the result value
		 * @return			string value
		 */
		char* fconv_to_str(val_str *vs, char *format, bool expl_conv, bool *alloc);
		
		/**
		 * converts a value to a cmpl problem.
		 * @param vs		source value
		 * @param expl_conv	true: user type conversion; false: internal type conversion
		 * @return			cmpl problem value
		 */
		CMPLProblemMain* fconv_to_problem(val_str *vs, bool expl_conv);




		/****** utility functions for sets and iterations ****/

		/**
		 * generates a set value using values on the value stack.
		 * @param tp		must be a set type, the values on the stack must match this type
		 * @param set		return of the set value
		 */
		void construct_set(enum etp tp, val_str *set);

		/*
		 * gives the intersection of the definition set of an array with a given second set.
		 * @param no_array_set	dont return a set as TP_SET_SYM_ARR, but convert to another set type
		 * @param array		array for which the definition set is used
		 * @param iter_set	second set
		 * @return			set value
		 */
		val_str defset_array(bool no_array_set, array_str *array, SetIterator *iter_set);

		/**
		 * sets the current values for free indexes of a symbol within an implicite loop
		 * @param i			symbol number
		 */
		void fset_free_ind(int i);

		/**
		 * frees all not global allocated memory for sets in an index info
		 * @param ind_inf_p	index info
		 */
		void ffree_index_sets(ind_inf_str *ind_inf_p);


		/****** other utility functions ****/

		/**
		 * replaces the patterns for local parameter values within line names
		 * @param n_dest	destination buffer
		 * @param n_src		line name which can contain patterns for local parameter
		 * @param p_mod		do not replace patterns for line count
		 */
		void flbeg_lname(char *n_dest, char *n_src, bool p_mod);

		/**
		 * writes a value as string to the temp buffer
		 * @param p			value
		 */
		char* write_val_to_temp_buf(val_str *p);
	};


	/**
	 * The <code>IntFuncBaseUtil</code> class does not execute a command of the intermediary code,
	 * like all other classes deviated from <code>IntFuncBase</code> do.
	 * Instead it is a non abstract class to provide access to the utility functions of the abstract class <code>IntFuncBase</code>.
	 */
	class IntFuncBaseUtil : public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncBaseUtil(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command of the intermediary code.
		 * Because this class is not for executing a command of the intermediary code, this is an empty implementation.
		 * @param vp	pointer to the current command in the intermediary code
		 */
		inline virtual void exec(val_str *vp) {}
	};
}

#define FILEIO		glvar->fileIO

#define SET_LEN(a)				glvar->setUtil.set_len(a)
#define SET2_LEN(a,b)			glvar->setUtil.set2_len(a, b)
#define SET_LEN_TP(a,b)			glvar->setUtil.set_len_tp(a, b)
#define SET_TEST_ELEM(a,b)		glvar->setUtil.set_test_elem(a, b)


#endif // !CMPL_INTFUNCBASE_HH

