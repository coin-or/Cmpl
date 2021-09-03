
/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010, 2011 Thomas Schleiff - Halle(Saale), 
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

#ifndef SYNTAXELEMENTS_HH
#define SYNTAXELEMENTS_HH


#include <map>

#include "../Control/MainData.hh"


using namespace std;


// start for symbol ids of extensions
#define EXTENSION_SYMBOL_BASE				1000


/************** defines for predefined symbol values **********/
// (defines for pseudo symbols are not here, but in IntCode.hh as minor numbers for INTCODE_FETCH_SPECIAL)
// (values here must be in the range between -1 and -(EXTENSION_SYMBOL_BASE-1) !)

// object types
#define PREDEFSYMVAL_OBJTYPE_PAR			-100
#define PREDEFSYMVAL_OBJTYPE_VAR			-101
#define PREDEFSYMVAL_OBJTYPE_OBJ			-102
#define PREDEFSYMVAL_OBJTYPE_CON			-103

// data types
#define PREDEFSYMVAL_DATATYPE_REAL			-120
#define PREDEFSYMVAL_DATATYPE_INT			-121
#define PREDEFSYMVAL_DATATYPE_BIN			-122
#define PREDEFSYMVAL_DATATYPE_NUMERIC		-123
#define PREDEFSYMVAL_DATATYPE_FORMULA		-124
#define PREDEFSYMVAL_DATATYPE_STRING		-125
#define PREDEFSYMVAL_DATATYPE_INDEXPART		-126
#define PREDEFSYMVAL_DATATYPE_TUPLE			-127
#define PREDEFSYMVAL_DATATYPE_SET			-128
#define PREDEFSYMVAL_DATATYPE_INTERVAL		-129
#define PREDEFSYMVAL_DATATYPE_FUNCTION		-130
#define PREDEFSYMVAL_DATATYPE_CONTAINER		-131
#define PREDEFSYMVAL_DATATYPE_TYPE			-132
#define PREDEFSYMVAL_DATATYPE_OBJTYPE		-133

// special values
#define PREDEFSYMVAL_VALUE_TRUE				-150
#define PREDEFSYMVAL_VALUE_FALSE			-151
#define PREDEFSYMVAL_VALUE_NULL				-152
#define PREDEFSYMVAL_VALUE_INFINITE			-153
#define PREDEFSYMVAL_VALUE_INVALID			-154

// functions
#define PREDEFSYMVAL_FUNC_SUM				-200
#define PREDEFSYMVAL_FUNC_MIN				-201
#define PREDEFSYMVAL_FUNC_MAX				-202
#define PREDEFSYMVAL_FUNC_AND				-203
#define PREDEFSYMVAL_FUNC_OR				-204

#define PREDEFSYMVAL_FUNC_COUNT				-220
#define PREDEFSYMVAL_FUNC_DEF				-221
#define PREDEFSYMVAL_FUNC_VALID				-222
#define PREDEFSYMVAL_FUNC_DIM				-223
#define PREDEFSYMVAL_FUNC_DEFSET			-224
#define PREDEFSYMVAL_FUNC_VALIDSET			-225
#define PREDEFSYMVAL_FUNC_LEN				-226
#define PREDEFSYMVAL_FUNC_RANK				-227
#define PREDEFSYMVAL_FUNC_ARRAY				-228

#define PREDEFSYMVAL_FUNC_SRAND				-230
#define PREDEFSYMVAL_FUNC_RAND				-231
#define PREDEFSYMVAL_FUNC_MOD				-240
#define PREDEFSYMVAL_FUNC_DIV				-241

#define PREDEFSYMVAL_FUNC_ECHO				-290
#define PREDEFSYMVAL_FUNC_ERROR				-291
#define PREDEFSYMVAL_FUNC_FORMAT			-292

// subsymbols with special meaning
#define PREDEFSYMVAL_SUB_ASVAR              -501
#define PREDEFSYMVAL_SUB_ASCON              -502
#define PREDEFSYMVAL_SUB_ASOBJ              -503

#define PREDEFSYMVAL_SUB_ASSTRING           -511



namespace cmpl
{
	class SyntaxElements;

	/**
	 * The <code>SymbolInfo</code> class holds the info about a symbol
	 */
	class SymbolInfo
	{
		friend class SyntaxElements;

	private:
		const char *_name;						///< symbol name, stored in global string store
		unsigned _defId;						///< unique definition number (count from 1)
		unsigned _globId;						///< unique number for global symbol (count from 1) / 0: no global symbol
		LocationInfo _loc;						///< location of symbol definition
		int _predef;							///< predefined symbol value (0: no predefined symbol; >0: pseudo symbol (one of ICS_SPECIAL_*); <0: predefined symbol (one of PREDEFSYMVAL_*) (if >1000 or <-1000 then predefined by an extension)
		bool _subSymbol;						///< symbol is subsymbol
		bool _readOnly;							///< symbol cannot be assign destination
		int _scopeType;							///< type of definition scope (see possible values in CompilerUtil.hh in enum SymbolScopeType)
		int _special;							///< special meaning for the symbol (see possible values in CompilerUtil.hh in enum SpecialSymbol)
        int _predefCont;                        ///< only used for predefined subsymbol: _predef of the container this symbol belongs to (the same symbol _defId can then appear more than once, with different _predefCont and unique _predef)

		/**
		 * constructor
		 */
		SymbolInfo()			{ }

	public:
		/**
		 * constructor
		 */
        SymbolInfo(MainData *data, const char *name, unsigned defId, unsigned globId, LocationInfo& loc, int predef, bool sub, bool ro, int st, int sp, int pdc)
                                                                                    { _name = data->globStrings()->store(name); _defId = defId; _globId = globId; _loc = loc; _predef = predef; _subSymbol = sub; _readOnly = ro; _scopeType = st; _special = sp; _predefCont = pdc; }

		/**
		 * set type of symbol
		 * @param ro			symbol cannot be assign destination
		 * @param st			type of definition scope
		 * @param sp			special meaning for the symbol
		 * @param gl			unique number for global symbol / 0: no global symbol
		 */
		inline void setType(bool ro, int st, int sp, unsigned gl)					{ _readOnly = ro; _scopeType = st; _special = sp; _globId = gl; }

		/**
		 * get symbol name (from string store in _symTab)
		 */
		inline const char *name()                                                   { return _name; }

		/**
		 * get predefined symbol value
		 */
		inline int predef()															{ return _predef; }

        /**
         * get whether symbol is sub symbol
         */
        inline bool subSymbol()														{ return _subSymbol; }

        /**
         * get _predef of the container this subsymbol belongs to
         */
        inline int predefCont()                                                     { return _predefCont; }

		/**
		 * get unique definition number
		 */
		inline unsigned defId()														{ return _defId; }

		/**
		 * get unique number for global symbol / 0: no global symbol
		 */
		inline unsigned globId()													{ return _globId; }

		/**
		 * get whether symbol cannot be assign destination
		 */
		inline bool readOnly()														{ return _readOnly; }

		/**
		 * writes the symbol info as an XML entry
		 */
		void writeXML(ostream& ostr, const string& indent);
	};


	/**
	 * types of syntax elements
	 */
	enum SyntaxElementType {
		SyntaxElementCode = 0,					///< whole cmpl code input

		SyntaxElementDirective,					///< assignment directive
		SyntaxElementModificator,				///< assignment or code block modificator

		SyntaxElementLitString,					///< literal string
		SyntaxElementLitInt,					///< literal int number
		SyntaxElementLitDouble,					///< literal floating point number

		SyntaxElementStatement,					///< cmpl statement
		SyntaxElementAssignment,				///< assignment
		SyntaxElementAssignLhs,					///< left hand side of assignment
		SyntaxElementAssignRhs,					///< right hand side of assignment

		SyntaxElementCodeblock,					///< code block
		SyntaxElementBlockPart,					///< part of code block
		SyntaxElementBlockHeader,				///< header of a part of code block
		SyntaxElementBlockBody,					///< body of a part of code block

		SyntaxElementUnaryOp,					///< unary operation
		SyntaxElementBinaryOp,					///< binary operation
		SyntaxElementCompareOp,					///< compare operation
		SyntaxElementSubSymbolOp,				///< sub symbol operation
		SyntaxElementIndexation,				///< indexation operation
		SyntaxElementFuncCall,					///< function call operation
		SyntaxElementArrayCast,					///< array cast operation

		SyntaxElementArray,						///< array construction expression
		SyntaxElementTupel,						///< tupel construction expression
		SyntaxElementSet,						///< algorithmic set construction expression
		SyntaxElementInterval,					///< interval construction expression
		SyntaxElementFunction,					///< function definition expression

		SyntaxElementLineNamePrefix,			///< line name prefix

		SyntaxElementSymbol,					///< symbol name
		SyntaxElementIncDec,					///< increment/decrement for symbol value

		SyntaxElementEmpty,						///< dummy element
		SyntaxElementError,						///< element with a syntax error within
	};


	/**
	 * The <code>SyntaxElement</code> class is used to store syntax elements and their location within the cmpl input.
	 */
	class SyntaxElement
	{
		friend class SyntaxElements;
		
	private:
		unsigned _id;					///< unique id number of this syntax element
		SyntaxElementType _type;		///< type of the element
		const char *_text;				///< text of the element
		const char *_alttext;			///< alternative text of the element
		unsigned _symId;				///< unique symbol definition number (count from 1) / 0: no symbol
		LocationInfo _loc;				///< location of the element

		SyntaxElement *_next;			///< pointer to next element
		SyntaxElement *_child;			///< pointer to first child element

	public:
		/**
		 * constructor
		 * @param id			unique id number for the syntax element
		 */
		inline SyntaxElement(unsigned id)															{ _id = id; _type = SyntaxElementEmpty; _text = NULL, _alttext = NULL, _symId = 0; _next = NULL; _child = NULL; }

		/**
		 * destructor
		 */
        //inline ~SyntaxElement()																	{ if (_child) delete _child; if (_next) delete _next; }
        inline ~SyntaxElement()																		{ if (_child) delete _child; while (_next) { SyntaxElement *s = _next; _next = s->_next; s->_next = NULL; delete s; } }

		/**
		 * creates a new element
		 * @param id			unique id number for the syntax element
		 * @param tp			type of the element
		 * @param locBegin		begin location
		 * @param locEnd		end location
		 * @return				new element
		 */
		inline static SyntaxElement* newEntry(unsigned id, SyntaxElementType tp, LocationInfo *locBegin, LocationInfo *locEnd) 								{ return newEntry(id, tp, NULL, NULL, 0, locBegin, locEnd); }

		/**
		 * creates a new element
		 * @param id			unique id number for the syntax element
		 * @param tp			type of the element
		 * @param text			text
		 * @param locBegin		begin location
		 * @param locEnd		end location
		 * @return				new element
		 */
		inline static SyntaxElement* newEntry(unsigned id, SyntaxElementType tp, const char *text, LocationInfo *locBegin, LocationInfo *locEnd) 			{ return newEntry(id, tp, text, NULL, 0, locBegin, locEnd); }

		/**
		 * creates a new element
		 * @param id			unique id number for the syntax element
		 * @param tp			type of the element
		 * @param text			text
		 * @param alttext		alternative text
		 * @param locBegin		begin location
		 * @param locEnd		end location
		 * @return				new element
		 */
		inline static SyntaxElement* newEntry(unsigned id, SyntaxElementType tp, const char *text, const char *alttext, LocationInfo *locBegin, LocationInfo *locEnd) 	{ return newEntry(id, tp, text, alttext, 0, locBegin, locEnd); }

		/**
		 * creates a new element
		 * @param id			unique id number for the syntax element
		 * @param tp			type of the element
		 * @param text			text
		 * @param symId			unique symbol definition number
		 * @param locBegin		begin location
		 * @param locEnd		end location
		 * @return				new element
		 */
		inline static SyntaxElement* newEntry(unsigned id, SyntaxElementType tp, const char *text, unsigned symId, LocationInfo *locBegin, LocationInfo *locEnd) 		{ return newEntry(id, tp, text, NULL, symId, locBegin, locEnd); }

	private:
		/**
		 * creates a new element
		 * @param id			unique id number for the syntax element
		 * @param tp			type of the element
		 * @param text			text
		 * @param alttext		alternative text
		 * @param symId			unique symbol definition number
		 * @param locBegin		begin location
		 * @param locEnd		end location
		 * @return				new element
		 */
		static SyntaxElement* newEntry(unsigned id, SyntaxElementType tp, const char *text, const char *alttext, unsigned symId, LocationInfo *locBegin, LocationInfo *locEnd);

	public:
		/**
		 * get unique id number of this syntax element
		 */
		inline unsigned id() const																	{ return _id; }

		/**
		 * get text of this syntax element
		 * @param alt	use alternative text if exists
		 */
		inline const char *text(bool alt = false) const												{ return (alt && _alttext ? _alttext : _text); }

		/**
		 * set unique symbol definition number
		 */
		inline void setSymId(unsigned s)															{ _symId = s; }

		/**
		 * appends the next element at the end of next elements
		 * @param n		element to append
		 */
		inline void appendNext(SyntaxElement *n)													{ if (_next) _next->appendNext(n); else _next = n; }

		/**
		 * appends the next element at the end of child elements
		 * @param n		element to append
		 */
		inline void appendChild(SyntaxElement *n)													{ if (_child) _child->appendNext(n); else _child = n; }

		/**
		 * count syntax elements including this
		 */
        int count() const																			{ int cnt = 0; const SyntaxElement *s = this; while (s) { cnt += 1 + (s->_child ? s->_child->count() : 0); s = s->_next; } return cnt; }

		/**
		 * location of the element
		 */
		inline const LocationInfo& loc() const														{ return _loc; }

		/**
		 * fills a map of syntax elements by id
		 * @param mp				map to fill
		 */
		void fillMap(map<unsigned, const SyntaxElement *> *mp);

		/**
		 * Writes the element as an XML entry
		 * @param ostr				stream to write to
		 * @param indent			string for indentation of element
		 * @param childIndentAdd	additional indentation for child elements
		 */
		void writeXML(ostream& ostr, const string &indent, int childIndentAdd);

	private:
		/**
		 * Writes the element as an XML entry
		 * @param ostr				stream to write to
		 * @param etnm				element type names
		 * @param indent			string for indentation of element
		 * @param childIndentAdd	additional indentation for child elements
		 */
		void writeXML(ostream& ostr, const char **etnm, const string &indent, int childIndentAdd);

		/**
		 * insert strings and locations used from this in the data object
		 * @param data		main data object
		 */
		void insertStringsAndLocs(MainData *data);

		/**
		 * write element to output stream
		 * @param si				serialization info
		 * @param data				main data object
		 * @param cur				current number
		 * @param prv				number of previous element / -1: current element is root element
		 * @param child				current element is child of previous element
		 */
		void serializeTo(MainData::SerializeInfo& si, const MainData *data, int& cur, int prv, bool child);
	};


	/**
	 * The <code>SyntaxElements</code> class holds the root pointer of syntax elements.
	 */
	class SyntaxElements : public MainData::DataBase
	{
	private:
		SyntaxElement *_root;						///< root of syntax elements
		int _cnt;									///< count of syntax elements

        map<unsigned, SymbolInfo *> *_symbols;		///< mapping for defined symbols (maps unique definition number to symbol info) (normally key is SymbolInfo::_defId, but for non-first entries of the same predefined subsymbol it is a unique number not used as _defId of any symbol)


	public:
		/**
		 * constructor
		 * @param m			module name
		 * @param data		main data object
		 */
		inline SyntaxElements(const char *m, MainData *data): MainData::DataBase(m, data)			{ _root = NULL; _cnt = 0; _symbols = NULL; }

		/**
		 * destructor
		 */
		inline ~SyntaxElements()																	{ cleanUp(); }

		/**
		 * set root element
		 * @param r			root of syntax elements
		 * @param s			mapping for defined symbols
		 * @param data		main data object
		 */
		void setRoot(SyntaxElement *r, map<unsigned, SymbolInfo *> *s, MainData *data);

		/**
		 * get root element
		 */
		inline SyntaxElement *getRoot()																{ return _root; }

		/**
		 * get symbol infos
		 */
		inline map<unsigned, SymbolInfo *> *getSymbolInfo()											{ return _symbols; }

		/**
		 * Writes syntax elements as XML entries
		 * @param ostr				stream to write to
		 * @param childIndentAdd	additional indentation for child elements
		 */
		void writeXML(ostream& ostr, int childIndentAdd);

	private:
		/**
		 * insert strings and locations used from this in the data object
		 * @param data		main data object
		 */
		void insertStringsAndLocs(MainData *data);


		/************** implementation for MainData::DataBase methods **********/

	public:
		/**
		 * delete all data
		 */
		virtual void cleanUp();

		/**
		 * get count of sub data objects
		 */
		virtual int subCnt()																		{ return _cnt + (_symbols ? _symbols->size() : 0); }

		/**
		 * if sub data is splitted up to different parts for serialization then get count of these parts, otherwise 0
		 */
		virtual int subPartCount()																	{ return 2; }

		/**
		 * check if serialization to or from stream should be done for the data object
		 * @param si		serialization info
		 * @param data		main data object
		 * @return			true if serialization should be done
		 */
		virtual bool checkSerialize(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * write data object to output stream
		 * @param si		serialization info
		 * @param data		main data object
		 */
		virtual void serializeTo(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * fill data object from input stream
		 * @param si		serialization info
		 * @param data		main data object
		 * @param pos		position of input string, returns iterated position
		 * @param subCnt	count of sub data objects
		 * @param lc		line count per element or 0 if elements are len coded
		 * @param rline		remaining section start line
		 */
		virtual void deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline);
	};
	
}

#endif // SYNTAXELEMENTS_HH

