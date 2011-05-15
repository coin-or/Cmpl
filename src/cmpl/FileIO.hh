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

#include "Main.hh"
#include "IntFuncBase.hh"



namespace cmpl
{
	class Main;

	/**
	 * The <code>FileIO</code> class contains methods all IO processes in CMPL.
	 */
	class FileIO
	{
	public:
		/**
		 * constructor
		 */
		FileIO(Main *m);
		//~FileIO();

		/****** Variables ****/
		
		/**
		 * <code>glvar</code> is the main object of CMPL and contains all relevant 
		 * variables and constants 
		 */
		Main *glvar;			

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
		 * Returns a description for a given error/warning type.
		 * @param tp A  error/warning type defined in main.hh
		 */
		const char *output_typename(enum etp tp);
		
		/**
		* Writes a specific buffer i a defined output. Used for several macro definitions.
		* @param art  a spcific output 
		*/
		void output_temp_string(int art);
		
		/**
		 * Writes the model in a MPS file or a Free-MPS file or a OSIL file. The file type 
		 * is defined by the user args.
		 */
		void io_write_dest();

		/**
		 * Generates a variable name for multidimensional variables 
		 * @param str	varibale name basis
		 * @param var_p Structure for the specific variable
		 */
		void io_make_varname(char *str, var_str *var_p);
		
		/**
		 * ??
		 */
		void tvstr_symname(char *str, struct term_var_str::tv_str *tvp, int maxsize);
		
		/**
		 * ??
		 */
		void ferror_symname(char *str, bool mfl, int maxsize);
		
		/**
		 * Writes a set in a string for outout issues
		 */
		char *write_set_to_str(char *dest, val_str *set, int maxlen);

		/**
		 * Reads a value form stdin into a parameter
		 * @param prompt string for user message 
		 * @param retp structure for a parameter 
		 */
		void io_read_stdin(const char *prompt, val_str *retp);
		
		/**
		 * Reads the content of a CSV file into a (single/multidimensional) parameter
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
		void io_write_obj_to(FILE *fp);
		char *name_wo_path(char *str);

		void fill_out_prod_list(int typ, struct term_var_str::tv_str *dp, struct term_var_str::tv_str *sp1, struct term_var_str::tv_str *sp2);
		void out_var_name(out_str_var *var_p, char *buf, int maxlen);
		void out_file_error(FILE *fp, out_str_error *out_err_p, size_t anz, bool xml);
		void out_file_prod_list(FILE *fp, out_str_prod_list *out_pl_p, size_t anz);
		void out_file_statistic(FILE *fp, out_str_statistic *out_stat_p);

		/* Stegger 1.2.3  changes in export files*/
		void io_write_dest_mps();
		void io_write_dest_xml();
		void io_write_dest_others();

		/* stegger 1.3.1.b help functions for xml export */	//TODO: Verwendung ersetzen durch std::list<std::string> und std::list<long>
		char_list *add_char_list(char *cp, char_list **clist, char_list *last_elem);
		long_list *add_long_list(long i, long_list **llist, long_list *last_elem);
	};
}

#endif // !CMPL_OUTPUT_HH

