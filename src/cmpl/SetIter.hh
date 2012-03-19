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
 * classes for sets and iteration.
 */

#ifndef CMPL_SETITER_HH
#define CMPL_SETITER_HH

namespace cmpl
{
	class CmplCore;
	class SetIterator;

	/****** structs used for sets ****/

	/**
	 * The struct <code>set_ea_str</code> is used for an enumeration set (TP_SET_ENUM) or for an algorithmic set (TP_SET_ALG).
	 */
	typedef struct set_ea_str_s {
		bool is_enum;			///< true: enumeration set / false: algorithmic set
		bool is_gl_mem;			///< true: memory is global reserved and not to release up to the end / false: release memory if this set object is not longer used
		long len;				///< number of elements in the set

		union set_ea_str_u {
			struct set_ea_str_e {	///< for an enumeration set
				val_str *array;         ///< array with the elements of the set, in iteration order of this set
				int *index;             ///< indexes of the elements in <code>array</code> in the canonical order
											/// (that is, first all numerical elements ascending, then all string elements alphabetical ascending)
				int string_start;       ///< index in <code>index</code> of the first string element
			} e;
			struct set_ea_str_a {	///< for an algorithmic set
				long start_val;			///< first value
				long end_val;			///< last value
				long step_val;			///< step to the next value
			} a;
		} u;
	} set_ea_str;


	/****** classes used for sets and iterations ****/

	/**
	 * The <code>SetUtil</code> class contains utility functions for sets.
	 * It doesn't contain the set itself, so all functions need a parameter for the set.
	 */
	class SetUtil
	{
	public:
		/**
		 * constructor
		 */
		inline SetUtil() { glvar = NULL; }

		/**
		 * constructor
		 * @param m		CmplCore object of CMPL
		 */
		inline SetUtil(CmplCore *m) { glvar = m; }

	public:
		/**
		 * <code>glvar</code> is the CmplCore object of CMPL and contains all relevant
		 * variables and constants.
		 * it is used only for the functions <code>set2_len</code> and <code>set_test_elem</code>.
		 */
		CmplCore *glvar;

	public:
		/**
		 * counts the elements in the set.
		 * @param setp		set
		 * @return 			number of elements
		 */
		long set_len(val_str *setp);

		/**
		 * counts the elements in the set of a specific value type.
		 * @param setp		set
		 * @param tp		searched value type (can be only TP_INT o TP_ISTR)
		 * @return			number of found elements
		 */
		long set_len_tp(val_str *setp, enum etp tp);

		/**
		 * counts the elements in the intersection of two sets.
		 * @param set1p		first set
		 * @param set2p		second set
		 * @return			number of elements
		 */
		long set2_len(val_str *set1p, val_str *set2p);

		/**
		 * tests if a specific value is an element of the set.
		 * @param setp		set
		 * @param ind		searched value
		 * @return			true: the set contains the searched value
		 */
		bool set_test_elem(val_str *setp, val_str *ind);
	};


	/**
	 * The <code>SetIterator</code> class is an iterator over the elements of a set.
	 */
	class SetIterator : public SetUtil
	{
	public:
		/**
		 * constructor
		 */
		inline SetIterator(): SetUtil() {}

		/**
		 * constructor
		 * @param m		CmplCore object of CMPL
		 */
		inline SetIterator(CmplCore *m): SetUtil(m) {}

		/**
		 * constructor
		 * @param m		CmplCore object of CMPL
		 * @param setp	set to iterate
		 */
		inline SetIterator(CmplCore *m, val_str *setp): SetUtil(m) { set = *setp; }

	/****** variables for iteration ****/
	public:
		val_str set;			///< set to iterate (must be of one type TP_SET_*)
		val_str cur_val;		///< current iteration value
		int cur_ind;			///< index number of the current iteration value within the set
		bool set_end;			///< end of set is reached through the iteration

	private:
		val_str set2;			///< second set, if iterating over the intersection of two sets / TPP_EMPTY: not an iteration over the intersection
		long array_ind_dir;		///< only if iterating over index values of an array: current index number in array_str.a_dir
		long array_ind_num;		///< only if iterating over index values of an array: current index number in array_str.a_num
		long array_ind_str;		///< only if iterating over index values of an array: current index number in array_str.a_str
		int cur_ind1;			///< index number of the current element in the set, which determines the iteration order
		bool dir_fwd;			///< true: forward iteration / false: backward iteration
		bool order2;			///< only if iterating over the intersection of two sets: if true, then the second set determines the iteration order

	
	/****** functions for iteration ****/
	public:
		/**
		 * starts the iteration over the set.
		 * @param dirfwd	true: forward iteration / false: backward iteration
		 * @return			true: current value is set / false: end of iteration is reached
		 */
		bool iter_set_start(bool dirfwd);

		/**
		 * starts the iteration over the intersection of the set and a second set.
		 * @param set2p		second set
		 * @param dirfwd	true: forward iteration / false: backward iteration
		 * @return			true: current value is set / false: end of iteration is reached
		 */
		bool iter_set2_start(val_str *set2p, bool dirfwd);

		/**
		 * restarts the iteration again with the first element (or last element if iterating backward).
		 * @return			true: current value is set / false: end of iteration is reached
		 */
		bool iter_set_restart();

		/**
		 * iterates two the next current value.
		 * @return			true: next current value is set / false: end of iteration is reached
		 */
		bool iter_set_next();

		/**
		 * sets the iteration to an specific current value.
		 * @param vp		searched current value
		 * @return			the iteration set contains the value <code>vp</code>
		 */
		bool iter_set_val(val_str *vp);

		/**
		 * gets the number of elements in the iteration.
		 * @return			number of elements in the iteration
		 */
		long iter_len();

	private:
		/**
		 * starts the iteration over the set or over the intersection of the set and a second set.
		 * @param setp		set which determines the iteration order
		 * @param dirfwd	true: forward iteration / false: backward iteration
		 * @return			true: current value is set / false: end of iteration is reached
		 */
		bool iter_set_start_intern(val_str *setp, bool dirfwd);

		/**
		 * iterates two the next current value.
		 * @param setp		set which determines the iteration order
		 * @return			true: current value is set / false: end of iteration is reached
		 */
		void iter_set_next_intern(val_str *setp);

		/**
		 * only for iterating over index values of an array: searches the next non empty element of the array.
		 * @param array		array
		 * @param iter		true: searches the next non empty element / false: searches the first non empty element
		 * @return			non empty element is found
		 */
		bool iter_find_next_array_set(array_str *array, bool iter);
	};
}

#endif // !CMPL_SETITER_HH

