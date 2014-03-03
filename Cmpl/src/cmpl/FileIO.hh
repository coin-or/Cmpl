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
 * The <code>FileIO</code> class contains methods all IO processes in CMPL.
 * @author  Thomas Schleiff/Mike Steglich
 */

#ifndef CMPL_FILEIO_HH
#define CMPL_FILEIO_HH

#include "CmplCore.hh"
#include "IntFuncBase.hh"
#include "CmplInstance.hh"
#include "CmplTools.hh"


#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;



namespace cmpl
{
	class CmplCore;

	/**
	 * The <code>FileIO</code> class contains methods all IO processes in CMPL.
	 */
	class FileIO
	{
	public:
                /**
		 * constructor
		 */
		FileIO(CmplCore *m);
		//~FileIO();

		/****** Variables ****/
		
		/**
		 * <code>glvar</code> is the CmplCore object of CMPL and contains all relevant 
		 * variables and constants 
		 */
		CmplCore *glvar;			

		/****** methods ****/
		
		/**
		 * Opens all neccessary files and data structures 
		 */
		void io_start_files();
		
		/**
		 * Last CMPL routine, that proceeds the neccessary file outputs and closes all open files. 
		 */
		void io_out_string_files();
		
		/**
		 * Allocates a block of memory for an array of len elements, each of them size bytes long.
		 * @param len	elements
		 * @param size	size
		 * @return A pointer to the memory block allocated by the function or a NULL pointer if the allocation failed.
		 */
		void *io_getmem(size_t len, size_t size);
		
		/**
		 * Writes error messages or warnings to stderr or into a structure. The behaviour depends on the ios->io_type.
		 * @param ios	structure for the output - the io_type can be a structure or stderr 
		 * @param type  error/warning type
		 * @param level error/warning level
		 * @param token token which causes the error/warning
		 * @param text	description of the error/warning
		 */
		void output_error(io_type_str *ios, int typ, int level, const char *token, const char *text);
		
		/**
		 * Returns the textual name of a CMPL data type, especially for output in an error/warning message.
		 * @param tp CMPL data type defined in Main.hh
		 */
		const char *output_typename(enum etp tp);
		
		/**
		* Writes the content of the global string buffer temp_buf to the specified output. Used for several macro definitions.
		* @param art  number of specific output 
		*/
		void output_temp_string(int art);
		
		/**
		 * Writes the model in a MPS file or a Free-MPS file or a OSIL file. The file type 
		 * is defined by the user args.
		 */
		void io_write_dest();

		/**
		 * Generates a variable name for multidimensional variables for using in the resulting Free-MPS file or a OSIL file
		 * @param str	result buffer
		 * @param var_p structure for the specific variable
		 */
		void io_make_varname(char *str, var_str *var_p);
		
		/**
		 * Generates a textual name for multidimensional variables used in a linear term.
		 * @param str	result buffer
		 * @param tvp	structure for the specific variable
		 * @param maxsize	size of the result buffer
		 */
		void tvstr_symname(char *str, struct term_var_str::tv_str *tvp, int maxsize);
		
		/**
		 * Generates a textual name for multidimensional parameters or variables for using in error/warning messages.
		 * @param str	result buffer
		 * @param mfl	output for free index: true: output the set defining the range of the free index; false: output the current index value
		 * @param maxsize	size of the result buffer
		 */
		void ferror_symname(char *str, bool mfl, int maxsize);
		
		/**
		 * Writes a textual form of a set in a string for output issues.
		 * @param dest		result buffer
		 * @param set		set
		 * @param maxlen	size of the result buffer
		 * @return result buffer
		 */
		char *write_set_to_str(char *dest, val_str *set, int maxlen);

		/**
		 * Writes a textual form of a tupel or tupelset in a string for output issues.
		 * @param dest		result buffer
		 * @param vp		tupel or tupelset
		 * @param maxlen	size of the result buffer
		 * @return result buffer
		 */
		char *write_tupel_to_str(char *dest, val_str *vp, int maxlen);

		/**
		 * Reads a value form stdin into a parameter.
		 * @param prompt string for user message 
		 * @param retp 	result pointer 
		 */
		void io_read_stdin(const char *prompt, val_str *retp);
		
		/**
		 * Reads the content of a CSV file into a (single/multidimensional) parameter.
		 * @param dim dimension  (1 - vector | 2 - matrix)
		 * @param filename filename
		 * @param callback pointer to the interpreter function
		 */
		void io_read_csv(int dim, const char *filename, IntFuncBase *callback);

	private:
		FILE *io_open_file(io_type_str *ios, int out_art, bool *new_opened);
		const char *error_typ_lvl(int typ, int level);
		void io_test_getmem_struct(io_type_str *ios, int anz, int step, size_t size);

		void t_write(int fd, char *buf, int len, char *fname);
		void t_read(int fd, char *buf, int len, char *fname);
		void io_make_commline(char *str);
		char *name_wo_path(char *str);

		void fill_out_prod_list(int typ, struct term_var_str::tv_str *dp, struct term_var_str::tv_str *sp1, struct term_var_str::tv_str *sp2);
		void out_var_name(out_str_var *var_p, char *buf, int maxlen);
		void out_file_error(FILE *fp, out_str_error *out_err_p, size_t anz, bool xml);


        void io_write_dest_varprod();

 
	};
}

#endif // !CMPL_OUTPUT_HH

