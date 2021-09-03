
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

#ifndef INTCODE_HH
#define INTCODE_HH

#include "../Control/MainData.hh"
#include "CmplVal.hh"


using namespace std;


namespace cmpl
{
	/**
	 * The <code>IntCode</code> class holds the intermediary code, filled by the Compiler module and executed by the Interpreter module.
	 */
	class IntCode : public MainData::DataBase
	{

		/****** definitions for code elements ****/

	public:
		/**
		 * type of code element
		 */
		enum IcType {
			icTypeCommand = 0,					///< code command
			icTypeInt = 1,						///< integer value as argument
			icTypeReal = 2,						///< real value as argument
			icTypeNo2 = 3,						///< two symbol numbers or similar as argument
		};

		/**
		 * struct for code command (icTypeCommand)
		 */
		struct IcCommand {
			unsigned short major;				///< major number of command
			unsigned short minor;				///< minor number of command
			unsigned short par;					///< parameter for command
			unsigned short cnt;					///< count of following arguments
		};

		/**
		 * struct for up to two symbol numbers or similar
		 */
		struct IcNo2 {
			unsigned int n1;					///< first value
			unsigned int n2;					///< second value
		};

		/**
		 * struct for code element
		 */
		struct IcElem {
			IcType tp;							///< type of element
			unsigned se;						///< id of syntax element (SyntaxElement::id())
			union {
				IcCommand c;						///< value if tp == icTypeCommand
				intType i;							///< value if tp == icTypeInt
				realType r;							///< value if tp == icTypeReal
				IcNo2 n;							///< value if tp == icTypeNo2
			} v;								///< value of element

#if PROTO
			/**
			 * write code element to protocol
			 * @param m		module
			 * @param i		address of code element within the code
			 * @param ostr	output to this stream / NULL: to protocol stream
			 */
			void protoOut(ModuleBase *m, unsigned i, ostream *ostr = NULL);
#endif
		};


		/****** array of code ****/

	private:
		IcElem *_code;							///< array with the intermediary code
		unsigned _cnt;							///< count of code elements in _code

	public:
		/**
		 * constructor
		 * @param m         module name
		 * @param data      main data object
		 */
		inline IntCode(const char *m, MainData *data): MainData::DataBase(m, data)				{ _code = NULL; _cnt = 0; }

		/**
		 * destructor
		 */
		inline ~IntCode()											{ cleanUp(); }

		/**
		 * set array with the intermediary code
		 * @param cnt		count of code elements
		 * @param code		array with the intermediary code
		 */
		inline void setCode(unsigned cnt, IcElem *code)				{ cleanUp(); _code = code; _cnt = cnt; }

		/**
		 * get array with the intermediary code
		 * @param cnt		return count of code elements
		 * @return			array with code elements
		 */
		inline IcElem *getCode(unsigned& cnt)						{ cnt = _cnt; return _code; }

		/**
		 * get code element at index i
		 * @param i			index
		 */
		inline IcElem *at(unsigned i)								{ return _code + i; }


        /************** implementation for MainData::DataBase methods **********/

	public:
		/**
		* delete all data
		*/
		virtual void cleanUp();

		/**
		* get count of sub data objects
		*/
		virtual int subCnt()										{ return _cnt; }

		/**
		 * if sub data is splitted up to different parts for serialization then get count of these parts, otherwise 0
		 */
		virtual int subPartCount()									{ return 0; }

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



/****** definition of code commands ****/

// no operation
#define INTCODE_NOP					0
	// (no minor numbers, no parameters)
    // arbitrary count of arguments (ignored when executing the NOP command, can be used as additional argument space for other commands)


// literal value: push a value given in the code to the stack
#define INTCODE_LIT_VAL				1

	// minor numbers
	#define ICS_LIT_INT					1		// literal integer value (as v.c.par (no arg) or v.i (one arg))
	#define ICS_LIT_STR					2		// literal string value (index in string store as v.c.par (no arg) or v.i (one arg))
	#define ICS_LIT_REAL				3		// literal real value (as v.r (one arg))
	#define ICS_LIT_BLANK				4		// blank value (within a tupel list or an array list)

	// parameters
	// 	only if no arg: integer value or string value (if within boundary of unsigned short)

	// arguments
	//  1. integer value or string value (v.i) (if not within boundary of unsigned short), or real value (v.r)


// fetch value: get a value stored in a symbol and push it to the stack
#define INTCODE_FETCH				2

	// minor numbers
	#define ICS_FETCH_GLOBAL			1		// fetch value of a global symbol
	#define ICS_FETCH_LOCAL				2		// fetch value of a local symbol
    #define ICS_FETCH_DEF_CB			3		// definition fetch of a code block symbol (only used as lvalue fetch)

	// parameters
    #define ICPAR_FETCH_LVALUE			0x0001		// bit 0: lvalue fetch or not
    #define ICPAR_FETCH_SUBSYM			0x0002		// bit 1: subsymbol fetch or not
    #define ICPAR_FETCH_NMSPACE			0x0004		// bit 2: namespace fetch (then the following command is an lvalue subsymbol fetch)
    #define ICPAR_FETCH_INCDEC			0x0008		// bit 3: fetch with increment or decrement

    #define ICPAR_FETCH_LVAL_NEW        0x0010      // bit 4: lvalue fetch for assignment with given "new" modificator (only used for subsymbol assignment)
    #define ICPAR_FETCH_LVAL_PUBLIC     0x0020      // bit 5: lvalue fetch for assignment with given "public" modificator (only used for subsymbol assignment)
    #define ICPAR_FETCH_LVAL_PRIVATE    0x0040      // bit 6: lvalue fetch for assignment with given "private" modificator (only used for subsymbol assignment)

	// arguments
	//	1a. symbol number (v.n.n1)
    //	1b. global symbol number for a global symbol / local symbol index for a local symbol / 0: no such symbol (v.n.n2)
    //          but if ICPAR_FETCH_LVALUE && ICPAR_FETCH_SUBSYM && !=0, then it is the code address of an additional argument with validity range for subsymbol,
    //          within this additional argument then v.n.n1 contains the start address of validity and v.n.n2 the end address or 0 for no end bound

    // only for prefix/postfix increment or decrement (ICPAR_FETCH_INCDEC)
	//	2a. prefix (1) or postfix (0) (v.n.n1)
	//	2b. increment (1) or decrement (0) (v.n.n2)


// fetch value of pseudo symbol: get a value by an internal function call and push it to the stack
#define INTCODE_FETCH_SPECIAL		3

	// minor numbers
	#define ICS_SPECIAL_ARG				1		// value of pseudo symbol $arg
	#define ICS_SPECIAL_THIS			2		// value of pseudo symbol $this
    #define ICS_SPECIAL_CURTUPLE		3		// value of pseudo symbol $curTuple
    #define ICS_SPECIAL_CURFULLTUPLE	4		// value of pseudo symbol $curFullTuple
    #define ICS_SPECIAL_CURDESTNAME		5		// value of pseudo symbol $curDestName
    #define ICS_SPECIAL_CURDESTTUPLE	6		// value of pseudo symbol $curDestTuple

    #define ICS_SPECIAL_VARBYNAME		7		// value of pseudo symbol $varByName
    #define ICS_SPECIAL_VARBYNO			8		// value of pseudo symbol $varByNo
    #define ICS_SPECIAL_CONBYNAME		9		// value of pseudo symbol $conByName
    #define ICS_SPECIAL_CONBYNO			10		// value of pseudo symbol $conByNo

    #define ICS_SPECIAL_OBJECTTYPE		11		// value of pseudo subsymbol $objectType
    #define ICS_SPECIAL_DATATYPE		12		// value of pseudo subsymbol $dataType
    #define ICS_SPECIAL_TYPEBASE		13		// value of pseudo subsymbol $typeBase
    #define ICS_SPECIAL_TYPEPAR			14		// value of pseudo subsymbol $typePar
    #define ICS_SPECIAL_ISREADONLY		15		// value of pseudo subsymbol $isReadOnly

    #define ICS_SPECIAL_DESTNAME		16		// value of pseudo subsymbol $destName
    #define ICS_SPECIAL_DESTTUPLE		17		// value of pseudo subsymbol $destTuple
    #define ICS_SPECIAL_DESTNAMETUPLE	18		// value of pseudo subsymbol $destNameTuple
    #define ICS_SPECIAL_DESTFULLNAME	19		// value of pseudo subsymbol $destFullName

    #define ICS_SPECIAL_FULLFORMULA		20		// value of pseudo subsymbol $fullFormula
    #define ICS_SPECIAL_FORMULAARRAY	21		// value of pseudo subsymbol $formulaArray

	// parameters: same as for INTCODE_FETCH

	// (no arguments)


// assign value to an lvalue expression: get the value from the stack and store it to the given lvalue expressions
#define INTCODE_ASSIGN				4

	// minor numbers
	#define ICS_ASSIGN_NORMAL			1		// normal assignment
	#define ICS_ASSIGN_REF				2		// reference assignment
	#define ICS_ASSIGN_ADD				3		// assign with operator "+="
	#define ICS_ASSIGN_SUB				4		// assign with operator "-="
	#define ICS_ASSIGN_MUL				5		// assign with operator "*="
	#define ICS_ASSIGN_DIV				6		// assign with operator "/="

	#define ICS_ASSIGN_LHSONLY			7		// assign without right hand side
	#define ICS_ASSIGN_RHSONLY			8		// assign without left hand side (if no "con" or "obj" then no real assign)

	#define ICS_ASSIGN_EXTERN			9		// declare external symbols (without right hand side)
	#define ICS_ASSIGN_ASSERT			10		// assert symbol definition (without right hand side)

	// parameters
    #define ICPAR_ASSIGN_CNTLHS			0x00ff	// bit 0 - 7: bit mask for count of left hand side values

    #define ICPAR_ASSIGN_CONST			0x0100	// bit  8: mark symbol and assigned values as readonly
    #define ICPAR_ASSIGN_ORDERED		0x0200	// bit  9: execute assign in strict order
    #define ICPAR_ASSIGN_INITIAL        0x0400  // bit 10: execute only initializing assignment
    #define ICPAR_ASSIGN_NOCOND         0x0800  // bit 11: execute non-conditional even in conditional context

    #define ICPAR_ASSIGN_RESNAME		0x2000	// bit 13: also set result object name if appropriate object type
    #define ICPAR_ASSIGN_OBJTYPE_LOCAL	0x4000	// bit 14: given object type is local symbol (false: no object type given or given as global symbol)
    #define ICPAR_ASSIGN_DATATYPE_LOCAL	0x8000	// bit 15: given data type is local symbol (false: no data type given or given as global symbol)

	// arguments
    //  1a. global symbol number / local symbol index of given object type / 0: no given object type (v.n.n1)
    //  1b. global symbol number / local symbol index of given data type / 0: no given data type (v.n.n2)
    //  2. start position of code for the value of the right hand side (v.n.n1) (only given if another execution of this code may compute a changed value)


// operation: get values from the stack, compute a result, and put the result value back to the stack
#define INTCODE_OPERATION			5

	// minor numbers
	#define ICS_OPER_ADD				1		// operator '+'
	#define ICS_OPER_SUB				2		// operator '-'
	#define ICS_OPER_MUL				3		// operator '*'
	#define ICS_OPER_DIV				4		// operator '/'
    #define ICS_OPER_POW				5		// operator '^'

	#define ICS_OPER_AND				6		// operator '&&'
	#define ICS_OPER_OR					7		// operator '||'
	#define ICS_OPER_NOT				8		// operator '!'

	#define ICS_OPER_EQ					9		// operator '='
	#define ICS_OPER_EQ2				10		// operator '=='
	#define ICS_OPER_NE					11		// operator '<>'
	#define ICS_OPER_NE2				12		// operator '!='
	#define ICS_OPER_LT					13		// operator '<'
	#define ICS_OPER_GT					14		// operator '>'
	#define ICS_OPER_LE					15		// operator '<='
	#define ICS_OPER_GE					16		// operator '>='

	#define ICS_OPER_IN					17		// operator 'in'
	#define ICS_OPER_OF					18		// operator 'of'

	#define ICS_OPER_IRR				19		// operator '*>'
	#define ICS_OPER_TRP				20		// operator 'T'

	#define ICS_OPER_OBJTO				21		// operator '->'

	#define ICS_OPER_INDEX				22		// indexation
	#define ICS_OPER_FUNC				23		// function call
	#define ICS_OPER_ARRCAST			24		// array cast

	// parameters
    #define ICPAR_OPER_CNT				0x00ff	// bit 0 - 7: count of source values for the operation (can be only in the range 0 to 2)
    #define ICPAR_OPER_CMP_FOLLOW		0x0100	// bit 8: other compare operation follows immediately as continuation of this compare operation (only used for compare operation)

	// (no arguments)

    // modes for handling out of range for integer values
    #define INT_OPER_OORANGE_IGNORE		0		// don't check for out of range values (operation result is then wrong when out of range occurs!)
    #define INT_OPER_OORANGE_ERROR		1		// error is thrown if out of range occurs
    #define INT_OPER_OORANGE_CONVERT	2		// convert integer to real value if it is out of integer range


// value construction: construct a new value from source values from the stack, and put the new value back to the stack
#define INTCODE_CONSTRUCT			6

	// minor numbers
	#define ICS_CONSTRUCT_ARRAY			1		// construct array
	#define ICS_CONSTRUCT_ARRAY_IND		2		// construct array, with adding new index part
	#define ICS_CONSTRUCT_ARRAY_WOI		3		// construct array, without adding new index part
    #define ICS_CONSTRUCT_TUPLE			4		// construct tuple
	#define ICS_CONSTRUCT_IV_WOLB		5		// construct interval without lower bound (can have 0 - 1 source values)
	#define ICS_CONSTRUCT_IV_LB			6		// construct interval with lower bound (can have 1 - 2 source values)
	#define ICS_CONSTRUCT_SET			7		// construct set (only algorithmic set) (must have 3 source values)

	// parameters
	// 	only if no arg: count of source values (if within boundary of unsigned short)

	// arguments
	//  1. integer value or string value (v.i) (if not within boundary of unsigned short), or real value (v.r)


// commands for line name prefix
#define INTCODE_LINENAMEPREF		7

	// minor numbers
	#define ICS_LNP_EXTEND				1		// extends the current line name prefix by a literal string value (index in string store as v.c.par (no arg) or v.i (one arg))
		// parameters
		// 	only if no arg: string value (if within boundary of unsigned short)
		// arguments
		//  1. string value (v.i) (if not within boundary of unsigned short)

	#define ICS_LNP_RESET				2		// reset the current line name prefix to its previous value
		// (no parameters)
		// (no arguments)


// commands for functions and jumps
#define INTCODE_FUNCTION			8

	// minor numbers
	#define ICS_FUNC_START				1		// start of function (also start of code)
		// parameters
		//	count of local symbols within the function
		// (no arguments)

	#define ICS_FUNC_END				2		// end of function (also end of code)
		// (no parameters)
		// (no arguments)

	#define ICS_FUNC_STORE				3		// create new function pointer, push it to the stack, and then skip the function code
		// parameters
		#define ICPAR_FUNC_WITH_SUBS		1		// bit 0: function uses subsymbols
		// arguments
		//	1a. start address of the function within the intermediary code (v.n.n1)
		//	1b. address after the function within the intermediary code (v.n.n2)

	#define ICS_FUNC_GOTO				4		// jump to an address of the code			//TODO: gedacht für Fälle, wo später größere Mengen Code noch eingefügt werden müssten, dann stattdessen Code anfügen und an eigentlichen Einfügepunkt nur Sprung, danach zurück
		// (no parameters)
		// arguments
		//  1. address within the intermediary code to jump to (v.n.n1)


// commands for code blocks
#define INTCODE_CODEBLOCK			9

	// minor numbers
	#define ICS_CB_BLOCK_START			1		// start of a code block
	#define ICS_CB_BLOCK_END			2		// end of code block

	#define ICS_CB_PART_START			3		// start of code block part
	#define ICS_CB_PART_BODY			4		// start of body of the code block part
	#define ICS_CB_PART_END				5		// end of code block part

	// parameters
	#define ICPAR_BLOCK_CNTPARTS		0x00ff	// bit 0 - 7: bit mask for count of code block parts (ICS_CB_BLOCK_*) or number of code block part (ICS_CB_PART_*)
    #define ICPAR_BLOCK_ORDERED			0x0100	// bit 8: at least in one part (ICS_CB_BLOCK_*) or in this block part (ICS_CB_PART_*) the ordered assign modificator is used
	#define ICPAR_BLOCK_OP_IN			0x0200	// bit 9: at least in one header of the code block a new code block symbol is defined on the left side of the operator "in" or "of" (ICS_CB_BLOCK_*) or in this code block part (ICS_CB_PART_*)
	#define ICPAR_BLOCK_STMNT			0x0400	// bit 10: at least one part (ICS_CB_BLOCK_*) or this block part (ICS_CB_PART_*) body contains statements (otherwise it is an expression)
    #define ICPAR_BLOCK_CTRL			0x0800	// bit 11: at least one code block control command within this code block references this code block or an outer code block (ICS_CB_BLOCK_*) or is within this code block part (ICS_CB_PART_*)
	#define ICPAR_BLOCK_REPEAT			0x1000	// bit 12: a code block control command "repeat" references this code block within this code block (ICS_CB_BLOCK_*) or within this code block part (ICS_CB_PART_*)
    #define ICPAR_BLOCK_NCSYM           0x2000  // bit 13: access to non-const symbol within body or header (but without considerating the first header) in the code block (ICS_CB_BLOCK_*) or this block part (ICS_CB_PART_*)
    #define ICPAR_BLOCK_LNP				0x4000	// bit 14: explicit setting of line name prefix directly in the code block (ICS_CB_BLOCK_*) or this block part (ICS_CB_PART_*)

	// arguments
    //	1a. address of ICS_CB_BLOCK_END command of this code block within the intermediary code (v.n.n1)
    //	1b. address of ICS_CB_PART_END command of this code block part within the intermediary code (v.n.n2) (only for ICS_CB_PART_START and ICS_CB_PART_BODY, otherwise 0)
	//	2a. count of local symbols within the function which are valid within the code block part (v.n.n1) (only for ICS_CB_PART_END)
	//	2b. count of local symbols within the function which are valid also after the code block part (v.n.n2) (only for ICS_CB_PART_END)


#define INTCODE_CB_HEADER           10

    // minor numbers
    #define ICS_CBHEADER_START          1       // start of code block header
    #define ICS_CBHEADER_END            2       // end of code block header

	// parameters
	#define ICPAR_CBHP_HCNT				0x00ff	// bit 0 - 7: bit mask for number of header part
	#define ICPAR_CBHP_CBS				0x0100	// bit 8: the header defines new code block symbols
	#define ICPAR_CBHP_IN				0x0200	// bit 9: only if ICPAR_CBHP_CBS: header uses operator "in" or "of" (otherwise operator "==")
    #define ICPAR_CBHP_NCSYM            0x0400  // bit 10: access to non-const symbol within the header
    #define ICPAR_CBHP_PHCBS            0x0800  // bit 11: access to code block symbol defined in a previous header of the same code block part

    // arguments: for ICS_CBHEADER_START
    //  1. address of ICS_CBHEADER_END for this code block header (v.n.n1)

    // arguments: for ICS_CBHEADER_END
	//	1a. address of ICS_CB_PART_START of next code block part or ICS_CB_BLOCK_END if no next code block part (v.n.n1)
	//	1b. 0: v.n.n1 is address of ICS_CB_PART_START / 1: v.n.n1 is address of ICS_CB_BLOCK_END (v.n.n2)
    //	2a. address of ICS_CB_BLOCK_START of the code block
    //	2b. address of ICS_CB_PART_START of the current code block part


// code block control commands
#define INTCODE_CB_CONTROL			11

	// minor numbers
	#define ICS_CBC_BREAK				1
	#define ICS_CBC_CONTINUE			2
	#define ICS_CBC_REPEAT				3

	// parameters
    #define ICPAR_CBC_CNTLVL			0x00ff	// bit 0 - 7: bit mask for count of levels to jump out (0: this code block; 1: direct outer code block; and so on)

    #define ICPAR_CBC_HASLHS            0x0100  // bit 8: left hand side is present in the code block control statement
    #define ICPAR_CBC_LHSTUPLE          0x0200  // bit 9: left hand side contains a indexing tuple, which lies on the value stack
    #define ICPAR_CBC_RHSVALUE			0x0400	// bit 10: right hand side is present in the code block control statement, its value lies on the value stack

	// arguments
	//  1a. address of ICS_CB_PART_END command of this code block part within the intermediary code (v.n.n1)
	//  1b. address of ICS_CB_BLOCK_END command of this code block part within the intermediary code (v.n.n2)



/****** protocol output of code commands ****/
#if PROTO
#define PROTO_MOD_OUT_IC(m,i,ep)		{ ep->protoOut(m, i); }
#else
#define PROTO_MOD_OUT_IC(m,i,ep)
#endif

#endif // INTCODE_HH

