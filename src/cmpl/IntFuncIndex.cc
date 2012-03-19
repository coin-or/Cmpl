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
		int n, ind_t;
		val_str v;
		ind_inf_one_str *inf_p;

		glvar->ind_inf_ptr->i_anz_ind = pop_int();
		glvar->ind_inf_ptr->i_free_ind = 0;
		glvar->ind_inf_ptr->i_is_transp = false;

		for(n=glvar->ind_inf_ptr->i_anz_ind-1; n>=0; n--)
		{
			inf_p = glvar->ind_inf_ptr->i_ind + n;
			if (sub == IND_GIVEN)
				pop_val(&v);
			else
				v.t = TP_SET_ALL;
				
			if (TYPECLASS(v.t) == TPK_SET)
			{
				/* Freier Index */
				glvar->ind_inf_ptr->i_free_ind++;
				inf_p->is_free = true;
				inf_p->iter_set.set = v;
			}
			else
			{
				/* Nicht-freier Index */
				inf_p->is_free = false;
				inf_p->cur_val = test_val_index(&v, &(inf_p->cur_tp));
			}
		}
		(glvar->ind_inf_ptr)++;

		PROTO_OUT_2("  index info stackptr : %d", (int)(glvar->ind_inf_ptr-glvar->ind_inf_table));

		if(glvar->ind_inf_ptr >= glvar->ind_inf_table + MAX_IND_INF) {
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_REK_SET_IND_MAX);
		}
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

