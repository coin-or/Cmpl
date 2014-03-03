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


#include "IntFuncCode.hh"



namespace cmpl
{
	//********************************************************
	// IntFuncProgEnd
	//********************************************************
	
	/* Ende Zwischencodeprogramm, Matrix speichern */
	void IntFuncProgEnd::exec(val_str *vp)
	{
		int n, m;
		int gnum = 1;


		glvar->dest_names = (name_str *) glvar->c_getmem(glvar->akt_defnum*sizeof(name_str));

		for(glvar->symbol_akt_p=glvar->symbol_table,n=0;n<glvar->symbol_anzahl;n++,glvar->symbol_akt_p++) {
			if(glvar->symbol_akt_p->tp_sym == TPK_VAR) {
				/* Fuer Symbol ohne Index */
				glvar->ind_inf_ptr->i_anz_ind = 0;
				if (glvar->symbol_akt_p->val_ip0)
					fend_name(glvar->symbol_akt_p->val_ip0->elem.v, &gnum);

				/* Fuer Symbol mit Indizes */
				for(m=1; m<MAX_ANZ_IND_1; m++) {
					glvar->ind_inf_ptr->i_anz_ind = m;
					fend_names(glvar->symbol_akt_p->val_ip[m-1], 0, &gnum);
				}
			}
		}


		if(glvar->anz_int_error==0) {
			FILEIO->io_write_dest();
		}
	}

	/* Fuer alle Variablen innerhalb eines Vektors die Variablennamen fuer Matrix generieren, Variablenschranken eintragen */
	void IntFuncProgEnd::fend_names(array_str *array, int ii, int *gnump)
	{
		if (array && array->attr.t != PTR_TP_EMPTY)
		{
			int i, n;
			struct a_str *arr_p;
			union ptr_sym_u *up;

			/* Alle Elemente im Array durchgehen */
			for (i=0; i<3; i++)
			{
				arr_p = (i==0 ? &(array->a_dir) : (i==1 ? &(array->a_num) : &(array->a_str)));
				glvar->ind_inf_ptr->i_ind[ii].cur_tp = (i==2 ? TP_ISTR : TP_INT);

				for (n=0; n<arr_p->len; n++)
				{
					glvar->ind_inf_ptr->i_ind[ii].cur_val = (i==0 ? n : arr_p->index[n]);

					if (array->attr.t == PTR_TP_ARRAY)
						fend_names(arr_p->array.a + n, ii+1, gnump);
					else if (array->attr.t == PTR_TP_VAR)
						fend_name(arr_p->array.v + n, gnump);

				}
			}
		}
	}

	/* Fuer eine Variable den Variablennamen fuer Matrix generieren, Variablenschranken eintragen */
	void IntFuncProgEnd::fend_name(var_str *vp, int *gnump)
	{
		if (vp && vp->v_type != TPP_EMPTY) {
			name_str *np;
			int n;
			char nstr[DST_NAME_LEN+10];

			np = glvar->dest_names + vp->v_defnum;
			FILEIO->io_make_varname(np->n_name, vp);

                        /*for(n=0; n<vp->v_defnum; n++) {
                                /* Eindeutigen Namen erzeugen */
                                /*if(strcmp(glvar->dest_names[n].n_name, np->n_name)==0) {
					snprintf(nstr, DST_NAME_LEN-1, SHORT_NAME_VAR, *gnump);
					(*gnump)++;
					nstr[DST_NAME_LEN-1] = '\0';
					SPRINT_ERR_3(ERROR_VAR_DUP_REPL, np->n_name, nstr);
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);
					strncpy(np->n_name, nstr, DST_NAME_LEN);
				}
                        }*/
#ifdef VAR_PROD
			np->n_type = vp->v_otype;
#else
			np->n_type = vp->v_type;
#endif
			np->n_is_koeff = (glvar->gl_mline_fp_any && glvar->gl_mline_cap > vp->v_defnum && glvar->gl_mline_fp_any[vp->v_defnum]);
			np->n_is_dn_b = vp->v_is_dn_b;
			np->n_dn_b = vp->v_dn_b;
			np->n_is_up_b = vp->v_is_up_b;
			np->n_up_b = vp->v_up_b;
		}
	}

	
	//********************************************************
	// IntFuncSrcFile
	//********************************************************

	/* Quelltextdatei wechseln */
	void IntFuncSrcFile::exec(val_str *vp)
	{
		if (vp[1].t != TP_STR)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			PROTO_OUT_2("  %s", vp[1].v.s);
			glvar->akt_src_file = vp[1].v.s;
		}
	}


	//********************************************************
	// IntFuncSrcLine
	//********************************************************

	/* Quelltextzeile wechseln */
	void IntFuncSrcLine::exec(val_str *vp)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			PROTO_OUT_2("  %d", vp[1].v.i);
			glvar->akt_src_line = vp[1].v.i;
		}
	}


}

