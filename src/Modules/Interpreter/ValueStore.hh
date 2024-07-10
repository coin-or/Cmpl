
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


#ifndef VALUESTORE_HH
#define VALUESTORE_HH

#include <mutex>
#include <vector>

#include "../../CommonData/CmplVal.hh"
#include "../../CommonData/CmplArray.hh"
#include "../../CommonData/SyntaxElements.hh"



using namespace std;



namespace cmpl
{
	class SymbolValue;
	class StackValue;
	class ExecContext;
    class OptModel;
    class ValueTree;
    class ValueAssertion;
    class VSChangeInfo;


    /**
     * base class for object with assertion
     */
    class ValueAssertionBase
    {
    protected:
        ValueAssertion *_nextAss;   ///< pointer to next assertion / NULL: no next assertion

        /**
         * constructor
         */
        inline ValueAssertionBase(): _nextAss(NULL)         { }

        /**
         * destructor
         */
        ~ValueAssertionBase();

    public:
        /**
         * add assertion
         * @param a         assertion to add
         */
        void addAssertion(ValueAssertion *a);
    };


    /**
     * assertion for cmpl value or array
     */
    class ValueAssertion: public ValueAssertionBase
    {
        friend class ValueStore;

    private:
        CmplVal _objType;			///< assertion about the object type (TP_OBJECT_TYPE) of the values in the store (must be TP_OBJECT_TYPE) / TP_EMPTY: no such assertion
        CmplVal _dataType;			///< assertion about the data type (TP_DATA_TYPE) of the values in the store (must be TP_DATA_TYPE) / TP_EMPTY: no such assertion

        CmplVal _defSet;            ///< assertion about the definition set of the array in the store (must be TP_SET_*) / TP_EMPTY: no such assertion
        bool _subSet;               ///< definition set of the array must be a subset of <code>_defSet</code>

        unsigned _syntaxElem;       ///< syntax element id of this assertion

    public:
        /**
         * constructor
         * @param se                syntax element id
         */
        inline ValueAssertion(unsigned se): ValueAssertionBase(), _objType(), _dataType(), _defSet(), _subSet(false), _syntaxElem(se)        { }

        /**
         * copy constructor
         * @param src
         */
        ValueAssertion(ValueAssertion& src);

        /**
         * destructor
         */
        inline ~ValueAssertion()                            { _objType.dispose(); _dataType.dispose(); _defSet.dispose(); }

        /**
         * check assertions, reports error if assertion fails
         * @param ctx               execution context
         * @param v                 value to check
         * @param ctp               check data type and object type
         * @param cds               check array definition set
         * @param vse               syntax element of value to check / 0: in assert command itself
         */
        void checkAsssertion(ExecContext *ctx, CmplVal& v, bool ctp, bool cds, unsigned vse = 0);

        /**
         * check assertions, reports error if assertion fails
         * @param ctx               execution context
         * @param v                 array with values to check
         * @param ctp               check data type and object type
         * @param cds               check array definition set
         * @param vse               syntax element of value to check / 0: in assert command itself
         */
        void checkAsssertion(ExecContext *ctx, CmplArray& v, bool ctp, bool cds, unsigned vse = 0);

        /**
         * set assertion about the object type
         * @param ot                object type
         */
        inline void setObjType(intType ot)                  { _objType.dispSet(TP_OBJECT_TYPE, ot); }

        /**
         * set assertion about the data type
         * @param dt                data type
         */
        inline void setDataType(ValType *dt)                { _dataType.dispSet(TP_DATA_TYPE, (CmplObjBase *)dt, true); if (_defSet) { setSubSet(); } }

        /**
         * set assertion about array definition set
         * @param ds                array definition set
         */
        inline void setDefset(CmplVal& ds)                  { _defSet.copyFrom(ds, true, true); if (_dataType) { setSubSet(); } }

    private:
        /**
         * set <code>_subSet</code> from <code>_dataType</code>
         */
        void setSubSet();
    };


	/**
	 * the <code>ValueStore</code> stores cmpl values in an array.
	 */
    class ValueStore : public CmplObjBase, public ValueAssertionBase
	{
		friend class SymbolValue;

	private:
        CmplArray *_values;         ///< array with the stored values

		bool _constVal;				///< the existing values in this store cannot be changed, added or deleted 
		bool _constSet;				///< the definition set of the array of this store cannot be changed (values cannot be added or deleted, but existing values can be changed)

        mutex _accMtx;              ///< mutex for access lock


	public:
		/**
		 * constructor
		 */
        inline ValueStore(): ValueAssertionBase(), _values(NULL), _constVal(false), _constSet(false)				{ }

        /**
         * constructor as copy of another value store
         */
        inline ValueStore(ValueStore *src): ValueStore()				{ if (src->_values) { src->_values->incRef(); _values = src->_values; } if (src->_nextAss) { _nextAss = new ValueAssertion(*(src->_nextAss)); } }

        /**
          * constructor as copy of another value (can be TP_VALUESTORE, TP_ARRAY, or ordinary value (no array or list))
          */
        ValueStore(CmplVal& v);

		/**
		 * destructor
		 */
        inline ~ValueStore()								{ if (_values && _values->decRef()) { delete _values; } }


        /**
         * get mutex for access lock
         */
        inline mutex& accMtx()                              { return _accMtx; }

		/**
		 * get value array
		 */
        inline CmplArray *values() volatile					{ return _values; }

		/**
		 * get pointer to simple value (no array) or NULL if values is an array
		 * @return					pointer to value or NULL
		 */
        inline CmplVal *simpleValue() const volatile        { return (_values ? _values->simpleValue() : NULL); }

        /**
         * get whether value is an simple empty value
         */
        inline bool isSimpleEmpty() const volatile          { CmplVal *v = simpleValue(); return (v ? v->isEmpty() : false); }

        /**
         * get whether value store is constant
         */
        inline bool isConst() const volatile                { return (_constVal && _constSet); }

        /**
         * get whether value store is empty array
         */
        inline bool isNull() const volatile                 { return (!_values || _values->isEmpty()); }

		/**
		 * set value from a stack value
		 * @param ctx				execution context
		 * @param sv				value to copy (can be also an array or an list, but no symbol value) (if a list it must be the top element on the value stack)
		 * @param disp				discard all previous values
         * @param srn				set row/col name for result matrix to this name
         * @param va                value, alternative to <code>sv</code>
         * @param se                syntax element, only used if <code>va</code> is given
         * @param chgInfo           track changes in this object / NULL: no change tracking
         * @param ord               use user order of definition set
         */
        void setValue(ExecContext *ctx, StackValue *sv, bool disp, const char *srn = NULL, CmplVal *va = NULL, unsigned se = 0, VSChangeInfo *chgInfo = NULL, bool ord = false);

        /**
         * set single value within the array
         * @param ctx				execution context
         * @param tpl               indexing tuple for the value within the array / NULL: not given
         * @param ind1              direct index+1 for the value within the array / 0: not given (at least tpl or ind1 must be given, if both given it must match)
         * @param val				value to set within the array, must be a single value (no array or list)
         * @param se                syntax element
         * @param srn				set row/col name for result matrix to this name
         * @param op                assign operation (+,-,*,/) or '\0'
         * @param chgInfo           track changes in this object / NULL: no change tracking
         */
        void setSingleValue(ExecContext *ctx, const CmplVal *tpl, unsigned long ind1, CmplVal& val, unsigned se, const char *srn = NULL, char op = '\0', VSChangeInfo *chgInfo = NULL);

        /**
         * fetch and increment single int value
         * @param ctx               execution context
         * @param pre               prefix or postfix incrementation
         * @param inc               increment value
         * @param se                syntax element
         * @return                  fetched value
         */
        intType fetchInc(ExecContext *ctx, bool pre, int inc, unsigned se);

        /**
         * add assertion
         * @param a                 assertion to add
         */
        void setAssertion(ValueAssertion *a)                    { addAssertion(a); if (a->_defSet && !a->_subSet) _constSet = true; }

        /**
         * check assertions, reports error if assertion fails
         * @param ctx               execution context
         * @param ctp               check data type and object type
         * @param cds               check array definition set
         * @param vse               syntax element of value to check
         */
        void checkAsssertions(ExecContext *ctx, bool ctp, bool cds, unsigned vse)       { if (_nextAss) { _nextAss->checkAsssertion(ctx, *_values, ctp, cds, vse); } }

        /**
         * set or move value for variable or constraint within the value tree for the result matrix
         * @param ctx				execution context
         * @param v					value to set in the value tree
         * @param se				syntax element id of the value
         * @param resMatrix			result matrix
         * @param fi				first index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE), or TP_EMPTY then <code>sc</code> holds only the remaining tuple parts up from <code>sub</code>
         * @param sc				second index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
         * @param rec               recursive call after applying name prefix
         */
        static void setValInValueTree(ExecContext *ctx, CmplVal *v, unsigned se, OptModel *resMatrix, const CmplVal& fi, const CmplVal& sc, bool rec = false);

        /**
         * set or move value for variable or constraint within the value tree for the result matrix, using only current name prefix
         * @param ctx				execution context
         * @param v					value to set in the value tree
         * @param se				syntax element id of the value
         * @param tp                additional index tuple / NULL: no one
         */
        static void setValInValueTree(ExecContext *ctx, CmplVal *v, unsigned se, OptModel *resMatrix, const CmplVal *tp = NULL);

		//TODO: Funktionen insbesondere zum Einfügen und Ändern von Werten
	
		/**
		 * copy values if necessary before write to the array
		 * @param disp				writing will discard all previous values
		 */
		void copyOnWrite(bool disp);

        /**
         * copy all data of another value store to this
         * @param src               source symbol value
         * @param assertions        copy also assertions
         */
        void copyAllFrom(ValueStore *src, bool assertions);
    };


    /**
     * info for changes within a value store
     */
    class VSChangeInfo
    {
        friend class ValueStore;

    private:
        ValueStore _baseVS;                         ///< unchanged base value store

        bool _fullChg;                              ///< all elements of the array are changed

        bool _defsetAdd;                            ///< element added to definition set (only used if !_fullChg)
        bool _defsetDel;                            ///< element deleted from definition set (only used if !_fullChg)

        unsigned long _chgIndFrom;                  ///< index number of first changed element of the array (only used if !_fullChg)
        unsigned long _chgIndEnd;                   ///< index number + 1 of last changed element of the array (only used if !_fullChg)

        vector<bool> _chgFlags;                     ///< changed elements per index number in _baseArray (only used if !_fullChg)

    public:
        /**
         * constructor
         * @param src           source value store
         */
        VSChangeInfo(ValueStore *src): _baseVS(src), _fullChg(false), _defsetAdd(false), _defsetDel(false), _chgIndFrom(0), _chgIndEnd(0)      { }

        /**
         * get unchanged base value store
         */
        ValueStore *baseVS()                                { return &_baseVS; }

        /**
         * get whether all elements of the array are changed
         */
        bool fullChg()                                      { return _fullChg; }

        /**
         * get whether element added to definition set
         */
        bool defsetAdd()                                    { return _defsetAdd; }

        /**
         * get whether element deleted from definition set
         */
        bool defsetDel()                                    { return _defsetDel; }

        /**
         * get whether definition set is changed
         */
        bool defsetChg()                                    { return (_defsetAdd || _defsetDel); }

        /**
         * get whether an element is changed
         * @param ind           index number of element
         * @return              true if element is changed
         */
        bool chgInd(unsigned long ind)                      { return (_fullChg || (ind < _chgFlags.size() && _chgFlags[ind])); }

        /**
         * get whether there is at least one changed element
         * @param allChg        return whether all elements are changed
         * @return              true if at least one element is changed
         */
        bool hasChgElem(bool& allChg);

    private:
        /**
         * mark all elements as changed
         */
        void chgFull()                                      { _fullChg = true; }

        /**
         * mark change of element indexed by the null tuple
         * @param ctx               execution context
         */
        void chgScalar(ExecContext *ctx)                    { CmplVal tpl(TP_ITUPLE_NULL); chgTpl(ctx, &tpl); }

        /**
         * mark change of one element
         * @param ctx				execution context
         * @param tpl               indexing tuple for the value within the array / NULL: not given
         * @param del               element is deleted
         * @param ind1              direct index+1 for the value within the array / 0: not given (at least tpl or ind1 must be given, if both given it must match)
         * @param set               set to which ind1 belongs (must be given if ind1 is used)
         */
        void chgTpl(ExecContext *ctx, const CmplVal *tpl, bool del = false, unsigned long ind1 = 0, CmplVal *set = NULL);
    };


    /**
	 * the <code>SymbolValue</code> contains the value of a cmpl symbol
	 */
	class SymbolValue
	{
	private:
		ValueStore *_val;			///< value store (this store may be shared by several symbol values)
		CmplVal _simpleConst;		///< only used if !_val, to store a const simple (non-array) value

        bool _global;               ///< global or local symbol
		bool _readOnly;				///< the value store cannot be changed in any way using this symbol value
		unsigned _defId;			///< unique definition number of the symbol (count from 1) (use Interpreter::_symbolInfo to get the symbol info for this definition number) / 0: not initialized

        mutex _accMtx;              ///< mutex for access lock

	public:
		/**
		 * empty constructor
		 */
		inline SymbolValue(): _val(NULL), _simpleConst(), _readOnly(false), _defId(0)	{ }

		/**
		 * destructor
		 */
		inline ~SymbolValue()								{ _simpleConst.dispose(); if (_val && _val->decRef()) { delete _val; } }

        /**
         * get mutex for access lock
         */
        inline mutex& accMtx()                              { return _accMtx; }

		/**
		 * initialize this for a symbol
		 * @param sym				symbol info
         * @param gl                global or local symbol
		 * @param cro				copy readonly from symbol info
		 */
        inline void init(SymbolInfo *sym, bool gl, bool cro)	{ _defId = sym->defId(); _global = gl; if (cro) { _readOnly = sym->readOnly(); } }

		/**
		 * get unique definition number of the symbol
		 */
		inline unsigned defId()								{ return _defId; }

        /**
         * get whether this symbol is global or local
         */
        inline bool global()                                { return _global; }

		/**
		 * get whether in this symbol value cannot be written
		 */
        inline bool readOnly() volatile 					{ return (_readOnly || (_val && _val->_constVal)); }

		/**
		 * get whether this symbol value has a value store
		 */
        inline bool hasValueStore()	volatile				{ return (_val != NULL); }

		/**
		 * get whether this symbol value has an const simple value (no array)
		 */
        inline bool hasSimpleConstVal() volatile			{ return (!_val && _readOnly); }

		/**
		 * get whether this symbol value is not initialized
		 */
        inline bool isUninitialized() volatile				{ return (!_val && !_readOnly); }

		/**
		 * get value store
		 * @param nw				allocate new value store if no one exists
		 */
		inline ValueStore *valueStore(bool nw = false)		{ if (_val || !nw) { return _val; } _val = new ValueStore(); _val->incRef(); _simpleConst.dispUnset(); return _val; }

		/**
		 * get reference of const simple value (no array)
		 */
        inline CmplVal& simpleConstVal()        			{ return _simpleConst; }

		/**
		 * get pointer to simple value (no array) or NULL if values is an array
		 * @return					pointer to value or NULL
		 */
        inline CmplVal *simpleValue()                       { return (_val ? _val->simpleValue() : (_readOnly ? &_simpleConst : NULL)); }

        /**
         * get whether value store is empty array
         */
        inline bool isNull() const volatile                 { return (_val && _val->isNull()); }

        /**
         * get whether value is an simple empty value
         */
        inline bool isSimpleEmpty()                         { CmplVal *v = simpleValue(); return (v ? v->isEmpty() : false); }


		/**
		 * empty this symbol value entry
		 * @param delId				unset also _defId
         * @param delRo             unset also _readOnl
		 */
        inline void unset(bool delId = false, bool delRo = false)		{ if (_val && _val->decRef()) { delete _val; } _val = NULL; _simpleConst.dispUnset(); if (delId) { _defId = 0; } if (delRo) { _readOnly = false; } }

		/**
		 * set const simple value as a copy of another value
		 * @param v					value to copy (must be a simple value)
         */
        inline void setSimpleConstVal(const CmplVal& v)     { unset(); _simpleConst.copyFrom(v, true, false); _readOnly = true; }

		/**
		 * set const simple value as a value desribed only by type
		 * @param tp				type for value
		 */
		inline void setSimpleConstVal(tp_e tp)				{ unset(); _simpleConst.set(tp); _readOnly = true; }

        /**
         * set const simple value to a CmplObjBase value
         * @param tp				type for value
         * @param p					value
         */
        inline void setSimpleConstVal(tp_e tp, CmplObjBase *p)  { unset(); _simpleConst.set(tp, p); _readOnly = true; }

		/**
		 * set this symbol value to read only
		 * @param vs				also set value store to read only
		 */
		inline void setReadOnly(bool vs = true)				{ _readOnly = true; if (vs && _val) { _val->_constVal = true; } }

		/**
		 * copy contents of another symbol value to this (value store as reference)
         * @param src               source symbol value
         * @param cro				copy also _readOnly
		 */
		inline void copyFrom(SymbolValue *src, bool cro = false)
															{ unset(); if (src->_val) { _val = src->_val; _val->incRef(); if (cro) _readOnly = src->_readOnly; } else if (src->_readOnly) { _simpleConst.copyFrom(src->_simpleConst, true, false); _readOnly = true; } }

        /**
         * copy all data of another symbol value to this
         * @param src               source symbol value
         * @param ref               copy value store as reference
         */
        void copyAllFrom(SymbolValue *src, bool ref);


        //TODO: Funktionen zum Zugriff
	};
}

#endif // VALUESTORE_HH

