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

/**
 * The <code>Compiler</code> class is used for generating intermediary code from the input file.
 * This is done by using flex and bison to build the scanner and the parser for the input file.
 * @author  Thomas Schleiff/Mike Steglich
 */

#ifndef CMPL_COMPILER_HH
#define CMPL_COMPILER_HH

#include "CmplCore.hh"
#include "gencode.h"

#include <string>
namespace cmpl {
	class Compiler;
}
#include "_CMPL_Parser.hh"


#define EXPSTR	CMPL_Parser::semantic_type::exp_str


namespace cmpl
{
	class CmplCore;
	class FileIO;

	/**
	 * The <code>Compiler</code> class contains functions for controlling the scanner and parser,
	 * and utility functions called from the scanner and parser.
	 */
	class Compiler
	{
	public:
		/**
		 * constructor
		 */
		Compiler(CmplCore *m);
		//~Compiler();

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

		//TODO: move instance variables out of <code>Main</code> to this class, if they are needed only here.
		//		e.g. instance variables for processing "include"


		/****** control of the scanner and parser ****/

		/**
		 * main function, executes the parser and generate the intermediary code
		 */
		void parse();

		/**
		 * Starts the scanner 
		 */
		void scan_begin();

		/**
		 * Stops the scanner 
		 */
		void scan_end();

		/**
		 * pointer to the scanner context 
		 */
		void *scanner;

		/**
		 * enables tracing for the scanner 
		 */
		bool trace_scanning;

		/**
		 * enables tracing for the parser 
		 */
		bool trace_parsing;

		/**
		 * outputs a error message
		 * @param m		message text
		 */
		void error(const std::string& m);

		//void error(const cmpl::location& l, const std::string& m);


		/****** functions for generating intermediary code ****/

		/**
		 * generates a command without a subcommand and without an embedded parameter
		 * @param st		command (defined in <code>gencode.h</code>)
		 * @param paranz	count of following parameters for the command
		 * @return			current address in the intermediary code
		 */
		t_addr comp(unsigned short st, unsigned short paranz);

		/**
		 * generates a command with a subcommand and with an embedded parameter but without following parameters
		 * @param st		command (defined in <code>gencode.h</code>)
		 * @param sub		subcommand (defined in <code>gencode.h</code>)
		 * @param par		embedded parameter for the command
		 * @return			current address in the intermediary code
		 */
		t_addr comp_sub_par(unsigned short st, unsigned short sub, short par);

		/**
		 * generates a command with a subcommand and with an embedded parameter and with following parameters
		 * @param st		command (defined in <code>gencode.h</code>)
		 * @param sub		subcommand (defined in <code>gencode.h</code>)
		 * @param par		embedded parameter for the command
		 * @param paranz	count of following parameters for the command
		 * @return			current address in the intermediary code
		 */
		t_addr comp_sub_par_cnt(unsigned short st, unsigned short sub, short par, unsigned short paranz);

		/**
		 * generates a command together with one following integer parameter
		 * @param st		command (defined in <code>gencode.h</code>)
		 * @param par		following parameter for the command
		 * @return			current address in the intermediary code
		 */
		t_addr comp_par_int(unsigned short st, long par);

		/**
		 * generates a value as a parameter for the previous command
		 * @param vp		pointer to the parameter value
		 * @return			current address in the intermediary code
		 */
		t_addr comp_val(val_str *vp);

		/**
		 * generates an address in the intermediary code as a parameter for the previous command
		 * @param a			address as parameter
		 * @return			current address in the intermediary code
		 */
		t_addr comp_addr(t_addr a);

		/**
		 * generates an integer value as a parameter for the previous command
		 * @param i			integer value as parameter
		 * @return			current address in the intermediary code
		 */
		t_addr comp_int(long i);

		/**
		 * generates a floating point value as a parameter for the previous command
		 * @param f			floating point value as parameter
		 * @return			current address in the intermediary code
		 */
		t_addr comp_double(double f);

		/**
		 * generates a temporary string value as a parameter for the previous command
		 * @param s			string value as parameter
		 * @return			current address in the intermediary code
		 */
		t_addr comp_str(const char *s);

		/**
		 * generates a cached string value as a parameter for the previous command
		 * @param i			index of the string value as parameter
		 * @return			current address in the intermediary code
		 */
		t_addr comp_istr(long i);

		/**
		 * generates a pointer value as a parameter for the previous command
		 * @param ptr		pointer value as parameter
		 * @return			current address in the intermediary code
		 */
		t_addr comp_voidp(void *ptr);

		/**
		 * generates an integer value, not to the current position but to a given address in the intermediary code
		 * @param dst		address to which the parameter is generated
		 * @param i			integer value as parameter
		 */
		void comp_int_at(t_addr dst, long i);

		/**
		 * generates an address in the intermediary code as a parameter, not to the current position but to a given address in the intermediary code
		 * @param dst		address to which the parameter is generated
		 * @param a			address as parameter
		 */
		void comp_addr_at(t_addr dst, t_addr a);

		/**
		 * gives the current address in the intermediary code
		 * @return			current address in the intermediary code
		 */
		t_addr addr_comp();

		/**
		 * sets the current address in the intermediary code
		 * @param dst		destination address
		 */
		void set_addr_comp(t_addr dst);


		/****** utility functions ****/

		/**
		 * searchs a name in the symbol tables
		 * @param str		name that is searched for
		 * @param ip		returns the index of the symbol in the global symbol table (for global symbols only)
		 * @return			number of the symbol (>=0: global symbol; <=-2: temporary symbol; -1: not found)
		 */
		int search_symbol(const char *str, int *ip);

		/**
		 * inserts a new symbol name in the global symbol table
		 * @param name		name of the new global symbol
		 * @param tpclass	type class of the new global symbol
		 * @return			number of the new global symbol
		 */
		int insert_symbol(const char *name, enum etp tpclass);

		/**
		 * inserts and initializes a new symbol from a cmplData file.
		 * @param name		name of the new global symbol
		 * @param tpclass	type class of the new global symbol
		 * @param dim		count of dimensions for the symbol
		 */
		int insertCmplDataSymbol(const char *name, enum etp tpclass, int dim);

		/**
		 * initialize a symbol table element.
		 * @param exp		expression which must be a symbol
		 * @param symtp		type of symbol
		 * @param tpk		type class for the symbol
		 * @param sym_dim	dimensionality of the symbol
		 * @param local		local or global symbol
		 */
		void init_symbol(EXPSTR *exp, enum ptrtp symtp, enum etp tpk, int sym_dim, bool local);

		/**
		 * inserts the standard subsymbols for a new initialized symbol.
		 * @param symp		symbol for which the subsymbols are to insert
		 */
		void insert_standard_subsyms(symbol_comp_str *symp);

		/**
		 * inserts a subsymbol in a subsymbol table.
		 * @param tp_sub_tab	number of the subsymbol table
		 * @param sname			name for the subsymbol
		 * @param tpk			typeclass for the subsymbol
		 * @param dim			count of dimensions for the subsymbol
		 */
		void insert_subsymbol(int tp_sub_tab, const char *sname, enum etp tpk, int dim);

		/**
		 * tests the type class for a global symbol
		 * @param symp		symbol info
		 * @param tpk		type class
		 * @param test_invalid_local	tests also if the symbol was already used as a local symbol but now out of scope
		 */
		void test_tpk_symbol(symbol_comp_str *symp, enum etp tpk, bool test_invalid_local);

		/**
		 * initialises a new loop context and set the row name prefix
		 * @param nr		number of the loop context
		 * @param base		first loop context after the opening {
		 * @param loop_tp	type of the body of the loop
		 * @param lname		row name prefix / NULL: the loop has no own row name prefix
		 */
		void init_loop_comp(unsigned short nr, bool base, enum looptp loop_tp, const char *lname);

		/**
		 * sets a new loop context as current, and initialises the next loop context
		 * @param sub		sub command of LOOP_CTRL (type of loop)
		 * @param mod		bitwise modifacators for the loop
		 * @return			pointer to the loop context
		 */
		loop_ctrl_str *set_loop_comp(unsigned short sub, unsigned short mod);

		/**
		 * sets type of loop and name of the local loop parameter in the current loop context
		 * @param sub		sub command of LOOP_CTRL (type of loop)
		 * @param exp		expression containing the iterator symbol / NULL: no definition of an iterator symbol
		 * @param symblock	array of symbol numbers for tupel disaggregation / NULL: no tupel disaggregation
		 */
		void set_loop_sname(unsigned short sub, EXPSTR *exp, int *symblock);

		/**
		 * close the loop context up to the next base context and generates the necessary closing commands in the intermediary code
		 */
		void unset_loop_comp();

		/**
		 * searches the next matching loop context
		 * @param s			searches for a loop context with this reference name or this name of the local loop parameter / NULL: no such search
		 * @param mod		searches for a loop context with such a modificator / 0: no such search
		 * @param errstr	error message if no matching context is found
		 * @param iscontinue	this function is called for the CMPL command continue
		 * @return			found loop context
		 */
		loop_ctrl_str *search_loop_name_mod(const char *s, unsigned short mod, const char *errstr, bool iscontinue);

		/**
		 * generates a line name and replaces $...$ patterns by the actual values
		 * @param n_dest	buffer for the generated line name
		 * @param n_src		source part for the line name
		 */
		void linename_repl(char *n_dest, const char *n_src);

		/**
		 * checks the count of variables in a term
		 * @param i		count of variables in the term
		 */
		void var_test_prod(int i);

		/**
		 * removes a tupel construction from the intermediary code and returns the symbols in it.
		 * @param cnt		returns the count of symbols in the tupel
		 * @param localrank	if not 0 then rank of lvalue tupel in a loop header
		 * @return			new allocated block with symbol numbers of the lvalue tupel	
		 */
		int *tupel_assign_symbols(int *cnt, int localrank);

		/**
		 * inserts intermediary code commands for storing values to global symbols.
		 * @param symblock		array of symbol numbers
		 * @param cnt			count of elements in symblock
		 * @param freeblock		free symblock after inserting the commands
		 * @return				current address in the intermediary code
		 */
		t_addr comp_symbols_assign(int *symblock, int cnt, bool freeblock);

		/**
		 * inserts a block of intermediary code commands to the intermediary code.
		 * @param codeblock		block of intermediary code commands
		 * @param len			length of the block with intermediary code commands
		 * @param freeblock		free the block after inserting the commands
		 * @return				current address in the intermediary code
		 */
		//t_addr comp_insert_block(val_str *codeblock, int len, bool freeblock);

		/**
		 * outputs a warning or error message about the use of deprecated assignment operator "="
		 * @param err	generate error message
		 */
		void out_warn_old_assign(bool err);

		/**
		 * outputs a warning or error message
		 * @param typ		type of error (one of ERROR_TYPE_*)
		 * @param level		level of the error (one of ERROR_LVL_*)
		 * @param text		error message text
		 */
		void output_error_parse(int typ, int level, const char *text);

		/**
		 * tests a number if it is a valid int number or a floating point number
		 * @param yylvalp		pointer to parser semantic value to set the number within
		 * @return				appropriate token
		 */
		CMPL_Parser::token_type test_number(CMPL_Parser::semantic_type *yylvalp);

		/**
		 * tests the type of a symbol or if it is the transpose code
		 * @param yylvalp		pointer to parser semantic value to set the symbol number within
		 * @param linename		symbol contains special characters only valid in a linename
		 * @return				appropriate token
		 */
		CMPL_Parser::token_type test_symbol(CMPL_Parser::semantic_type *yylvalp, bool linename);

		/**
		 * tests a name for a subsymbol.
		 * @param yylvalp		pointer to parser semantic value to set the symbol number within
		 * @return				appropriate token
		 */
		CMPL_Parser::token_type test_subsymbol(CMPL_Parser::semantic_type *yylvalp);

		/**
		 * opens an included file
		 * @param yylvalp		pointer to parser semantic value
		 * @return				file pointer to the opened file
		 */
		FILE *open_include_file(CMPL_Parser::semantic_type *yylvalp);

		/**
		 * ends up the include of an file
		 */
		void include_pop();


		/****** functions for type deviation of expressions ****/

		/**
		 * deviates the type of elements in an array
		 * @param t1		type of elements in the array up to now
		 * @param t2		type of a next element for the array
		 * @return			type of elements in the array
		 */
		//enum etp parse_type_dev_array(enum etp t1, enum etp t2);
		EXPSTR parse_type_dev_array(EXPSTR *t1, EXPSTR *t2);

		/**
		 * deviates the type of an expression which is the choice between two expressions
		 * @param t1		type of first input expression
		 * @param t2		type of second input expression
		 * @return			type of result expression
		 */
		//enum etp parse_type_dev_switch(enum etp t1, enum etp t2);
		EXPSTR parse_type_dev_switch(EXPSTR *t1, EXPSTR *t2);

		/**
		 * deviates the type of an expression which is a numeric operation
		 * @param sub		subcommand to the command NUM_OP
		 * @param t1		type of first operand
		 * @param t2		type of second operand
		 * @return			type of result expression
		 */
		EXPSTR parse_type_dev_num_op(int sub, EXPSTR *t1, EXPSTR *t2);

		/**
		 * deviates the type of an expression which is a compare operation,
		 * really only tests the type of the operands
		 * @param sub		subcommand to the command CMP_OP
		 * @param t1		type of first operand
		 * @param t2		type of second operand
		 * @return			type of result expression
		 */
		EXPSTR parse_type_dev_cmp_op(int sub, EXPSTR *t1, EXPSTR *t2);

		/**
		 * deviates the type of an litteraly set expression,
		 * really only tests the type of the operands
		 * @param settp		type of the set
		 * @param t1		type of first operand in the set expression
		 * @param t2		type of second operand in the set expression
		 * @param t3		type of third operand in the set expression
		 * @return			type of the set
		 */
		EXPSTR parse_type_dev_lit_set(enum etp settp, EXPSTR *t1, EXPSTR *t2, EXPSTR *t3);

		/**
		 * deviates the result type of the subsymbol operator,
		 * really searches the subsymbol and inserts it in the subsymbol table.
		 * @param t			expression left of the subsymbol operator
		 * @param subno		global subsymbol number of the symbol right of the subsymbol operator
		 */
		EXPSTR parse_type_dev_subsym_op(EXPSTR *t, int subno);

		void test_debug(EXPSTR *t, int i);


	private:
		/****** functions for include files ****/

		/**
		 * initializes the possibility for including files
		 */
		void include_init();

		/**
		 * starts include of a file
		 * @param full_filename		name of included file, can contain directory name
		 */
		void include_push(const char *full_filename);

		/**
		 * gets the current directory name
		 * @return		current directory name
		 */
		const std::string get_current_wd();

		/**
		 * sets the name of the current included file
		 * @param filename			name of current file
		 */
		void include_set_filename(const char *filename);

		/**
		 * splits a file name which can contain directory name to the file name and the directory name
		 * @param full_filename			input file name
		 * @param path					pointer to return the directory name
		 * @return						file name without directory name
		 */
		char *split_path_name(const char *full_filename, const char **path);
	};
}


/* macro definitions for using in parse rules (parse-param: Compiler &cmplcomp) */
#ifndef PARSE_CONTEXT
#define PARSE_CONTEXT	cmplcomp
#define GLV				(PARSE_CONTEXT.glvar)
#endif

#define COMP(x,a)				PARSE_CONTEXT.comp((unsigned short)(x), (unsigned short)(a))
#define COMP_SUB(x,s)			PARSE_CONTEXT.comp_sub_par((unsigned short)(x), (unsigned short)(s), (short)0)
#define COMP_PAR(x,p)			PARSE_CONTEXT.comp_sub_par((unsigned short)(x), (unsigned short)0, (short)(p))
#define COMP_SUB_PAR(x,s,p)		PARSE_CONTEXT.comp_sub_par((unsigned short)(x), (unsigned short)(s), (short)(p))
#define COMP_SUB_PAR_CNT(x,s,p,a)	PARSE_CONTEXT.comp_sub_par_cnt((unsigned short)(x), (unsigned short)(s), (short)(p), (unsigned short)(a))
#define COMP_PAR_INT(x,i)		PARSE_CONTEXT.comp_par_int((unsigned short)(x), (long)(i))
#define COMP_VAL(etp,comp,vl)	{ val_str compval; compval.t=etp; compval.v.comp=vl; PARSE_CONTEXT.comp_val(&compval); }
#define COMP_LIT_VAL(vl)		{ PARSE_CONTEXT.comp((unsigned short)LIT_VAL, (unsigned short)1); PARSE_CONTEXT.comp_val(&vl); }
#define COMP_ADDR(x)			PARSE_CONTEXT.comp_addr((t_addr)(x))
#define COMP_INT(x)				PARSE_CONTEXT.comp_int((long)(x))
#define COMP_DOUBLE(x)			PARSE_CONTEXT.comp_double((double)(x))
#define COMP_STR(x)				PARSE_CONTEXT.comp_str((char *)(x))
#define COMP_ISTR(x)			PARSE_CONTEXT.comp_istr((long)(x))
#define COMP_VOIDP(x)			PARSE_CONTEXT.comp_voidp((void *)(x))
#define COMP_INT_AT(d,x)		PARSE_CONTEXT.comp_int_at((t_addr)(d), (long)(x))
#define COMP_ADDR_AT(d,a)		PARSE_CONTEXT.comp_addr_at((t_addr)(d), (t_addr)(a))

#define ADDR_COMP				PARSE_CONTEXT.addr_comp()
#define BACK_ADDR_COMP(i)		PARSE_CONTEXT.set_addr_comp(PARSE_CONTEXT.addr_comp() - i)


/* function prototype for the scanner */
#define YY_DECL                                   \
	cmpl::CMPL_Parser::token_type                         \
		cmpllex (cmpl::CMPL_Parser::semantic_type* yylval_param,      \
					cmpl::CMPL_Parser::location_type* yylloc_param,      \
					void* yyscanner, cmpl::Compiler& PARSE_CONTEXT)
YY_DECL;



#endif // !CMPL_COMPILER_HH

