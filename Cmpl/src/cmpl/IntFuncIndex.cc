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


#include "IntFuncIndex.hh"
#include "gencode.h"


namespace cmpl
{
	//********************************************************
	// IntFuncSetIndexes
	//********************************************************

	/* Indizes vom Wertestack zum Indexstack */
	void IntFuncSetIndexes::exec(val_str *vp)
	{
		int sub = vp->v.c.sub;
		set_index_info((sub == IND_GIVEN), true, NULL, false);
	}


	//********************************************************
	// IntFuncDelIndexes
	//********************************************************

	/* Loeschen oberstes Element Indexstack */
	void IntFuncDelIndexes::exec(val_str *vp)
	{
		if((--(glvar->ind_inf_ptr)) < glvar->ind_inf_table)
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_PRV_SET_IND);

		ffree_index_sets(glvar->ind_inf_ptr);
	}


	//********************************************************
	// IntFuncIndTranspose
	//********************************************************

	/* Festlegen Matrix als transponiert */
	void IntFuncIndTranspose::exec(val_str *vp)
	{
		if(glvar->ind_inf_ptr == glvar->ind_inf_table) {
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_PRV_SET_IND);
		}
		glvar->ind_inf_ptr[-1].i_is_transp = true;
	}
}

