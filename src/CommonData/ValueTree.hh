
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


#ifndef VALUETREE_HH
#define VALUETREE_HH


#include <map>
#include <unordered_map>
#include <vector>
#include <sstream>

#include "CmplVal.hh"
#include "CmplArray.hh"
#include "../Control/StringStore.hh"


using namespace std;



namespace cmpl
{
	class ValueTree;
	class ValueTreeRoot;


	/**
	 * base class for objects which can be contained within a value tree
	 */
    class ValueTreeElem
	{
		friend class ValueTreeRoot;

	protected:
        unsigned long _id;								///< identy number of the element (only used for leaf elements)

		ValueTree *_parent;								///< parent value tree / NULL: this is the root value tree
        int _level;										///< recursion level of element within the value tree / 0: this is the root value tree / -1: element is not in a value tree

		bool _indexInt;									///< index of this element within the parent value tree is int (true) or string (false)
		intType _parIndex;								///< index number of this element within the parent value tree

    public:
        /**
         * constructor
         */
        ValueTreeElem()									{ _id = 0; _parent = NULL; _level = 0; }

        /**
         * constructor
         * @param par	parent value tree
         * @param lvl	recursion level of value tree
         * @param ii	index of this element within the parent value tree is int (true) or string (false)
         * @param pi	index number of this element within the parent value tree
         */
        ValueTreeElem(ValueTree *par, int lvl, bool ii, intType pi)			{ _id = 0; _parent = par; _level = lvl; _indexInt = ii; _parIndex = pi; }

        /**
         * destructor
         */
        virtual ~ValueTreeElem()                        { }

        /**
         * get root of value tree, to which this belongs
         * @return      root of value tree, or NULL if not in a value tree
         */
        ValueTree *getRoot() const;

        /**
         * set or move element within the value tree
         * @param vtr	root of value tree, to which this belongs
         * @param fi	first index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE), or TP_EMPTY then <code>sc</code> holds only the remaining tuple parts up from <code>sub</code>
         * @param sc	second index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
         * @param sub	subtree for the element if known
         * @return		0:ok; 1:place already occupied
         */
        int setInValueTree(ValueTreeRoot *vtr, const CmplVal &fi, const CmplVal &sc, ValueTree *sub = NULL);

        /**
         * take element out of the value tree
         */
        void unsetInValueTree();

        /**
         * remove element from the value tree, also from store by identy number
         * @param vtr   root of value tree, to which this belongs
         */
        void removeFromValueTree(ValueTreeRoot *vtr);

        /**
         * get whether this element has already a place in the value tree
         */
        inline bool isInTree() const							{ return (_level >= 0); }

        /**
         * get identy number of the element
         */
        inline unsigned long id() const                         { return _id; }

        /**
         * get single index value of a given level of the element in the tree
         * @param level             level of searched index value
         * @param res               return of result value
         */
        void getIndexOfLevel(unsigned level, CmplVal &res) const;

        /**
         * get tuple of index values for a given range of level of the element in the tree
         * @param levelFrom         level from of searched index tuple
         * @param levelTo           level to of searched index tuple or 0 for up to the level of this
         * @param res               return of result value
         */
        void getIndexOfLevel(unsigned levelFrom, unsigned levelTo, CmplVal &res) const;

        /**
         * insert name for usage on formatted outputs in map of names
         * @param sstore			stored string values
         * @param names				map of names, key is the name, value is the id of this ValueTreeElem
         * @param prefStd			prefix for standard name if the element have no name in the value tree / NULL if no standard name
         * @param prefSubs			prefix for substitute name, used if a regular name not exists or is not unique
         * @param cntSubs			counter for substitute name
         * @param quote				quote char for strings, 0 if no one
         * @param rplSp				replacement char for space, 0 for removing spaces, space char for leaving spaces unchanged
         * @param mLen				max length for result name
         * @return					true if the regular name is used, false if the substitute name is used
         */
        bool insOutName(StringStore *sstore, unordered_map<string, unsigned long>& names, const char *prefStd, const char *prefSubs, unsigned& cntSubs, char quote, char rplSp, unsigned mLen);

        /**
         * recursive output of name parts of this value tree element to stream
         * @param ostr				stream for output
         * @param sstore			stored string values
         * @param quote				quote char for strings, 0 if no one
         * @param leaf				true if this is the leaf element
         */
        virtual void recOutName(ostringstream& ostr, StringStore *sstore, char quote, bool leaf);
    };


	/**
	 * class for a value tree, which can contain as elements other trees or leaf elements
	 */
	class ValueTree : public ValueTreeElem
	{
        friend class ValueTreeElem;
		friend class ValueTreeRoot;

	protected:
        map<intType, ValueTreeElem *> _leafInt;			///< mapping to leaf elements by int index
        map<intType, ValueTreeElem *> _leafStr;			///< mapping to leaf elements by string index

        map<intType, ValueTree *> _subInt;				///< mapping to subtree elements by int index
        map<intType, ValueTree *> _subStr;				///< mapping to subtree elements by string index

    public:
        /**
         * constructor
         */
        ValueTree(): ValueTreeElem()	{ }

        /**
         * constructor
         * @param par	parent value tree
         * @param lvl	recursion level of value tree
         * @param ii	index of this element within the parent value tree is int (true) or string (false)
         * @param pi	index number of this element within the parent value tree
         */
        ValueTree(ValueTree *par, int lvl, bool ii, intType pi): ValueTreeElem(par, lvl, ii, pi)			{ }

        /**
         * destructor
         */
        virtual ~ValueTree();
    };


	/**
	 * class for storing elements first in a flat array and second in a value tree
	 */
	class ValueTreeRoot
	{
        friend class ValueTreeElem;
        friend class ValueTree;

	protected:
		tp_e _elemType;									///< type of elements

        vector<ValueTreeElem *> _flat;					///< elements stored by identy number (vector element can be NULL if deleted)
		CmplArray *_arrFlat;							///< cmpl array with elements by identy number, only allocated and filled if needed
        bool _hasArrFlat;								///< true if _arrFlat is filled with actual elements

		ValueTree _tree;								///< value tree with the elements
		CmplArray *_arrTree;							///< cmpl array with elements from value tree, only allocated and filled if needed
        bool _hasArrTree;								///< true if _arrTree is filled with actual elements

        mutex _accMtx;                                  ///< mutex for access lock


	public:
		/**
		 * constructor
		 */
        ValueTreeRoot(tp_e tp)							{ _elemType = tp; _arrFlat = _arrTree = NULL; _hasArrFlat = _hasArrTree = false; }

		/**
		 * destructor
		 */
		~ValueTreeRoot();


        /**
         * get mutex for access lock
         */
        inline mutex& accMtx()                          { return _accMtx; }

        /**
         * insert a new element, but only in vector by id, but not in value tree
         * @param e		new element (not used already in a ValueTreeRoot)
         * @return		new element <code>e</code>
         */
        ValueTreeElem *insertNewElem(ValueTreeElem *e);

		/**
         * insert a new element, also in the value tree
         * @param e		new element (not used already in a ValueTreeRoot)
         * @param fi	first index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE), or TP_EMPTY then <code>sc</code> holds only the remaining tuple parts up from <code>sub</code>
		 * @param sc	second index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
		 * @param sub	subtree for the element if known
		 * @return		new element <code>e</code>
		 */
		ValueTreeElem *insertNewElem(ValueTreeElem *e, CmplVal &fi, CmplVal &sc, ValueTree *sub = NULL);

        /**
         * get count of elements
         */
        unsigned long size() const						{ return _flat.size(); }

        /**
         * get element by flat index number
         * @param i		index number
         * @return 		element
         */
        ValueTreeElem *operator[](unsigned long i) const	{ return _flat[i]; }

		/**
		 * get the element by id
		 * @param id	id of the element (count from 1)
		 * @return		element
		 */
        ValueTreeElem *getElem(unsigned long id) const	{ return _flat[id - 1]; }

		/**
		 * get the element by index
		 * @param fi	first index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
		 * @param sc	second index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
		 * @return		element
		 */
        ValueTreeElem *getElem(CmplVal &fi, CmplVal &sc) const;

		/**
		 * get the element by index
		 * @param fi	first index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
		 * @return		element
		 */
        ValueTreeElem *getElem(CmplVal &fi)	const		{ CmplVal sc; return getElem(fi, sc); }

	};
}

#endif // VALUETREE_HH

