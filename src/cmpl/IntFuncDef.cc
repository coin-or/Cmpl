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


#include "IntFuncDef.hh"
#include "CmplData.hh"
#include "gencode.h"


namespace cmpl
{
	//********************************************************
	// IntFuncDefBase
	//********************************************************

	/* Definition Variable oder Konstante */
	void IntFuncDefBase::fdef_val(symbol_str *sym_p, int c_i, int mod, enum etp tp_cd, int dim_rhs, bool indices, val_str *vpall)
	{
        int dim_lhs = (glvar->ind_inf_ptr->i_is_tupelset ? glvar->ind_inf_ptr->i_ind->iter_set.set.v.tps->unfixRank() : glvar->ind_inf_ptr->i_free_ind);

		// Wenn rechte Seite skalar, dann Wert dort fuer alles verwenden
		val_str vp0;
		if (dim_rhs == 0 && mod != DEF_VAR && !vpall)
		{
			pop_val(&vp0);
			vpall = &vp0;
		}

		// Unterscheidung, ob ueber Tupelset
		if (dim_rhs == 1 || ((mod == DEF_VAR || vpall) && glvar->ind_inf_ptr->i_is_tupelset))
		{
			if (dim_lhs == 0 && mod != DEF_VAR)
			{
				// Vektor muss Laenge 1 haben, dann skalar zuweisen
				if (!vpall)
				{
					int cnt = pop_int();
					if (cnt != 1)
					{
						FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "vector on right hand side must have length 1");	//TODO
						//TODO: Werte vom Stack nehmen
						return;
					}
				}

				if (!sym_p->val_ip0)
				{
					sym_p->val_ip0 = (nonarray_str *) glvar->c_getmem(sizeof(nonarray_str));
					sym_p->val_ip0->attr.t = PTRTP_FOR_TYPECLASS(sym_p->tp_sym);
				}
				sym_p->val_ip0->attr.s_newvar = (mod == DEF_VAR);

				fdef_ins_val(sym_p, &(sym_p->val_ip0->elem), mod, tp_cd, vpall);
			}
			else if ((dim_lhs == 1 || vpall) && !glvar->ind_inf_ptr->i_is_tupelset && !indices)
			{
				// Normale Definition eindimensionales Array
				fdef_vekts(sym_p, c_i, (array_str *)0, 0, mod, tp_cd, vpall);
			}
			else
			{
				// Tupelset wird benoetigt
				Tupelset *tps;
				if (glvar->ind_inf_ptr->i_is_tupelset)
					tps = glvar->ind_inf_ptr->i_ind->iter_set.set.v.tps;
				else
					tps = tupelsetFromIndexinfo(glvar->ind_inf_ptr);

				// Arraydefinition ueber Tupelset
				fdef_arr_tps(sym_p, mod, tp_cd, tps, indices, vpall);
			}
		}
		else
		{
			if (mod != DEF_VAR && !vpall)
			{
				if (glvar->ind_inf_ptr->i_is_tupelset)
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "right hand side must have rank 1");	//TODO
					//TODO: Werte vom Stack nehmen
					return;
				}
				if (dim_rhs != dim_lhs)
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "rank of right hand side doesn't match left hand side");	//TODO
					//TODO: Werte vom Stack nehmen
					return;
				}
			}

			fdef_vekts(sym_p, c_i, (array_str *)0, 0, mod, tp_cd, vpall);
		}
	}

	/* Definition Variable oder Konstante
	   (auch Matrix, dann auch Aufbau Vektorbaum) */
	void IntFuncDefBase::fdef_vekts(symbol_str *sym_p, int c_i, array_str *arr_p, int ii, int mod, enum etp tp_cd, val_str *vpall)
	{
		union ptr_sym_u up;
		array_ins_info_str ret_ins_info;
		ind_inf_one_str *i_info;
		val_str set, *set_p;
		set_ea_str *set_ea;
		SetIterator *iter_p;
		bool found;
		int n;
		long ind;
		enum etp ind_tp;

		SetIterator iter_set(glvar);

		if (glvar->ind_inf_ptr->i_anz_ind == 0)
		{
			/* Einfacher Wert (kein Index) */
			if (!sym_p->val_ip0)
			{
				sym_p->val_ip0 = (nonarray_str *) glvar->c_getmem(sizeof(nonarray_str));
				sym_p->val_ip0->attr.t = PTRTP_FOR_TYPECLASS(sym_p->tp_sym);
			}

                       sym_p->val_ip0->attr.s_newvar = (mod == DEF_VAR);

			fdef_ins_val(sym_p, &(sym_p->val_ip0->elem), mod, tp_cd, vpall);
			return;
		}

		if (ii == 0)
		{
			/* Ausgangsarray aus Symboleintrag holen */
			arr_p = sym_p->val_ip[glvar->ind_inf_ptr->i_anz_ind - 1];
			if (!arr_p)
			{
				/* Ausgangsarray neu anlegen */
				arr_p = (array_str *) glvar->c_getmem(sizeof(array_str));
				sym_p->val_ip[glvar->ind_inf_ptr->i_anz_ind - 1] = arr_p;

				arr_p->attr.t = (glvar->ind_inf_ptr->i_anz_ind == 1 ? PTRTP_FOR_TYPECLASS(sym_p->tp_sym) : PTR_TP_ARRAY);
				arr_p->a_dir.cap = arr_p->a_dir.len = arr_p->a_dir.cnt = 0;
				arr_p->a_num.cap = arr_p->a_num.len = arr_p->a_num.cnt = 0;
				arr_p->a_str.cap = arr_p->a_str.len = arr_p->a_str.cnt = 0;
			}
			arr_p->attr.s_newvar = (mod == DEF_VAR);
		}

		/* Falls es bisher einen expliziten Definitionsset gibt, wird er jedenfalls ungueltig */
		arr_p->defset.t = TPP_EMPTY;

		/* Wenn Index frei ist, dann Set dafuer verwenden: Set pruefen und noetigenfalls anpassen */
		i_info = glvar->ind_inf_ptr->i_ind + ii;
		set_ea = (set_ea_str *)0;

		if(i_info->is_free)
		{
			set_p = &(i_info->iter_set.set);
			if(mod==DEF_VAR || vpall)
			{
				if (SET_IS_INFINITE(set_p->t))
				{
					/* Set darf nicht unendlich sein, sonst Fehler und ersatzweise leerer Set */
					snprintf(glvar->errbuf, ERRBUFLEN, ERROR_VEKTOR_NO_LEN, ii+1, glvar->symbol_akt_p->s_name);
					glvar->errbuf[ERRBUFLEN] = '\0';
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);

					set.t = TP_SET_EMPTY;
				}
				else
				{
					/* Set direkt verwenden */
					set = *set_p;
				}
			}
			else
			{
				n = pop_int();
				if (SET_IS_INFINITE(set_p->t))
				{
					/* Nur unendliche Sets erlaubt, die nummerische Werte enthalten koennen */
					if (!SET_CAN_NUMBERS(set_p->t))
					{
						snprintf(glvar->errbuf, ERRBUFLEN, ERROR_VEKTOR_NO_LEN, ii+1, glvar->symbol_akt_p->s_name);
						glvar->errbuf[ERRBUFLEN] = '\0';
						FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);

						/* Ersatzweise algorithmischer Set ab 1 */
						set.t = TP_SET_ALG1_1;
						set.v.i = n;
					}
					else
					{
						/* Unendlichen Set durch algorithmischen ersetzen */
						long start_val = (set_p->t == TP_SET_HALF_LB ? set_p->v.i : (set_p->t == TP_SET_HALF_UB ? set_p->v.i + 1 - n : 1));
						if (start_val == 0 || start_val == 1)
						{
							set.t = (start_val == 0 ? TP_SET_ALG0_1 : TP_SET_ALG1_1);
							set.v.i = start_val + n - 1;
						}
						else
						{
							set_ea = (set_ea_str *) calloc(1, sizeof(set_ea_str));
							set_ea->is_enum = false;
							set_ea->is_gl_mem = false;
							set_ea->len = n;
							set_ea->u.a.step_val = 1;

							if (set_p->t == TP_SET_HALF_LB)
							{
								set_ea->u.a.start_val = set_p->v.i;
								set_ea->u.a.end_val = set_p->v.i + n - 1;
							}
							else if (set_p->t == TP_SET_HALF_UB)
							{
								set_ea->u.a.end_val = set_p->v.i;
								set_ea->u.a.start_val = set_p->v.i - n + 1;
							}
							else /*if (set_p->t == TP_SET_ALL)*/
							{
								set_ea->u.a.start_val = 1;
								set_ea->u.a.end_val = n;
							}

							set.t = TP_SET_ALG;
							set.v.set = set_ea;
						}
					}
				}
				else if (iter_set.set_len(set_p) != n)
				{
					/* Anzahl Elemente im Set passt nicht zur angegebenen Werteanzahl, Fehler und ersatzweise algorithmischen Set mit passender Elementanzahl */
					snprintf(glvar->errbuf, ERRBUFLEN, ERROR_VEKTOR_ELEM,
													ii+1, glvar->symbol_akt_p->s_name);
					glvar->errbuf[ERRBUFLEN] = '\0';
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);

					if (set_p->t == TP_SET_ALG)
					{
						set_ea = (set_ea_str *) calloc(1, sizeof(set_ea_str));
						set_ea->is_enum = false;
						set_ea->is_gl_mem = false;
						set_ea->len = n;

						set_ea->u.a.start_val = set_p->v.set->u.a.start_val;
						set_ea->u.a.step_val = set_p->v.set->u.a.step_val;
						set_ea->u.a.end_val = set_ea->u.a.start_val + set_ea->u.a.step_val * (n - 1);

						set.t = TP_SET_ALG;
						set.v.set = set_ea;
					}
					else if (set_p->t == TP_SET_ALG0_1 || set_p->t == TP_SET_ALG1_1)
					{
						set.t = set_p->t;
						set.v.i = (set_p->t == TP_SET_ALG0_1 ? 0 : 1) + n - 1;
					}
					else
					{
						set.t = TP_SET_ALG1_1;
						set.v.i = n;
					}
				}
				else
				{
					/* Set direkt verwenden */
					set = *set_p;
				}
			}

			iter_set.set = set;
			iter_p = &iter_set;

			/* Wenn Array bisher leer ist, dann diesen Set als expliziten Definitionsset eintragen (wenn selbst Definitionsset, dann nur wenn explizit) */
			if (arr_p->a_dir.cnt + arr_p->a_num.cnt + arr_p->a_str.cnt == 0)
			{
				val_str *defset = (SET_USE_ARRAY(set.t) ? &(set.v.array->defset) : &set);
				if (defset->t != TPP_EMPTY)
				{
					if (SET_USE_POINTER(defset->t) && !defset->v.set->is_gl_mem)
					{
						/* Reservierten Speicher fuer den Set jetzt in Speicherverwaltung aufnehmen */
						glvar->ins_mem((void *)(defset->v.set));
						defset->v.set->is_gl_mem = true;
					}
					arr_p->defset = *defset;
				}
			}
		}
		else
		{
			iter_p = NULL;
		}
		
		/* Wenn freier Index, dann ueber Set fuer freien Index iterieren; sonst nur einmal durchlaufen */
		for (n = 0; ; n++)
		{
			if (i_info->is_free)
			{
				if (n == 0)
					iter_p->iter_set_start(false);
				else
					iter_p->iter_set_next();

				if (iter_p->set_end)
					break;

				i_info->cur_val = iter_p->cur_val.v.i;
				i_info->cur_tp = iter_p->cur_val.t;
			}
			ind = i_info->cur_val;
			ind_tp = i_info->cur_tp;

			/* Indexelement im Array suchen */
			up = search_ind_elem(arr_p, ind, ind_tp, &found, &ret_ins_info);

			/* Noetigenfalls neu einfuegen */
			if (!found)
				up = insert_ind_elem(arr_p, ind, ind_tp, iter_p, &ret_ins_info);

			if (ii == glvar->ind_inf_ptr->i_anz_ind - 1)
			{
				/* Letzten Index erreicht, also jetzt in Array arr_p Wert/Werte selbst stellen */
				fdef_ins_val(sym_p, &up, mod, tp_cd, vpall);
			}
			else
			{
				/* Noch nicht letzter Index, in Array arr_p kommt also weiteres Array/weitere Arrays */
				array_str *n_arr_p = up.a;
				if (!found)
				{
					n_arr_p->attr.t = (ii == glvar->ind_inf_ptr->i_anz_ind - 2 ? PTRTP_FOR_TYPECLASS(sym_p->tp_sym) : PTR_TP_ARRAY);
					n_arr_p->a_dir.cap = n_arr_p->a_dir.len = n_arr_p->a_dir.cnt = 0;
					n_arr_p->a_num.cap = n_arr_p->a_num.len = n_arr_p->a_num.cnt = 0;
					n_arr_p->a_str.cap = n_arr_p->a_str.len = n_arr_p->a_str.cnt = 0;
				}

				/* Rekursiv fuer naechsten Index */
				fdef_vekts(sym_p, c_i, n_arr_p, ii+1, mod, tp_cd, vpall);
			}

			if (!i_info->is_free)
				break;
		}

		/* Hier reservierten Set wieder freigeben */
		if (set_ea && !set_ea->is_gl_mem)
			free(set_ea);

		/* Nach Abschluss der Rekursion alle Sets im Index-Info freigeben, die nicht global reserviert sind */
		if (ii == 0)
		{
			for (n = 0, i_info = glvar->ind_inf_ptr->i_ind; n < glvar->ind_inf_ptr->i_anz_ind; n++, i_info++)
			{
				if(i_info->is_free && SET_USE_POINTER(i_info->iter_set.set.t) && !i_info->iter_set.set.v.set->is_gl_mem)
				{
					free(i_info->iter_set.set.v.set);
					i_info->iter_set.set.v.set = (set_ea_str *)0;
					i_info->iter_set.set.t = TP_SET_EMPTY;
				}
			}
		}
	}

	/**
	 * definition of an array of parameters or variables defined over a tupelset.
	 * @param sym_p     data structure for the symbol
	 * @param mod       definition of parameter or variable
	 * @param tp_cd     explicit given type for parameter or TPP_EMPTY for none
	 * @param tps       tupelset with defining tupels of the array
	 * @param indices   not only the values but also the indices come from the stack (can only be used with dim_rhs == 1; cannot used with mod == DEF_VAR or vpall != NULL)
	 * @param vpall		use this value / NULL: get the values from stack
	 */
	void IntFuncDefBase::fdef_arr_tps(symbol_str *sym_p, int mod, enum etp tp_cd, Tupelset *tps, bool indices, val_str *vpall)
	{
		// Anzahl der Elemente bestimmen
		val_str *lst;
                int cnt, n, d, i, rank;

		if (mod != DEF_VAR && !vpall)
		{
			// Werte als Liste vom Stack holen
			long l;
			cnt = pop_int();
			lst = pop_list_elems(cnt, false, &l, &d);

			if (!indices)
			{
				// Wenn nicht indices, dann muss genau fuer jedes Tupelsetelement ein Wert da sein
				if (cnt != tps->len())
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "count of elements doesn't match length of tupelset");	//TODO
					return;
				}
			}
			else
			{
				// Wenn indices, dann pro Element erst alle Indizes (dafuer muss der Rang bekannt sein), dann der Wert
				rank = tps->rank();
				if (rank < 0)
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "tupelset has no known rank");	//TODO
					return;
				}
				if (cnt % (rank + 1) != 0)
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "count of elements doesn't match the rank of tupelset");	//TODO
					return;
				}
				
				cnt /= (rank + 1);
			}
		}
		else
		{
			cnt = tps->len();
		}

		if (cnt == 0)
			return;

		// Platz fuer neues Indexinfo bereitstellen
		(glvar->ind_inf_ptr)++;
		PROTO_OUT_2("  index info stackptr : %d", (int)(glvar->ind_inf_ptr-glvar->ind_inf_table));

		if(glvar->ind_inf_ptr >= glvar->ind_inf_table + MAX_IND_INF)
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_REK_SET_IND_MAX);

		glvar->ind_inf_ptr->i_free_ind = 0;
		glvar->ind_inf_ptr->i_is_transp = false;
		glvar->ind_inf_ptr->i_is_tupelset = false;

		// Iteration über das Tupelset vorbereiten
		TupelsetIteratorBase *iter = (indices ? NULL : tps->newIterContext(glvar, true));
		val_str curtup;
		ind_inf_one_str *indp;
		val_str *vp = vpall;

		// Alle Werte durchgehen
		for (n = 0; n < cnt; n++)
		{
			if (!indices)
			{
				// Iteration ueber das Tupelset
				if (n == 0)
					iter->start_iter(&curtup);
				else
					iter->set_next(&curtup);

				rank = RANK_TP_TUPEL(curtup.t);
				
				// Neues Indexinfo aus dem Tupel fuellen, da fdef_vekts Indizes als Indexinfo erwartet
				glvar->ind_inf_ptr->i_anz_ind = rank;
				indp = glvar->ind_inf_ptr->i_ind;

				if (rank == 1)
				{
					indp->is_free = false;
					indp->cur_val = TupelUtil::tupelvalToIntTp(curtup.v.i, &(indp->cur_tp));
				}
				else if (rank > 1)
				{
					for (d = 0; d < rank; d++, indp++)
					{
						indp->is_free = false;
						indp->cur_val = TupelUtil::tupelvalToIntTp(curtup.v.tp[d+1], &(indp->cur_tp));
					}
				}

				if (mod != DEF_VAR && !vpall)
					vp = lst + n;
			}
			else
			{
				// Neues Indexinfo aus den naechsten Werten fuellen
				glvar->ind_inf_ptr->i_anz_ind = rank;
				indp = glvar->ind_inf_ptr->i_ind;

				for (d = 0, i = n * (rank + 1); d < rank; d++, i++, indp++)
				{
					indp->is_free = false;
					indp->cur_val = test_val_index(lst + i, &(indp->cur_tp));
				}

                                 vp = lst + i;
			}

			// Wert eintragen
			fdef_vekts(sym_p, rank, (array_str *)0, 0, mod, tp_cd, vp);
		}

		(glvar->ind_inf_ptr)--;
		if (iter)
			TupelsetIteratorBase::delIterContext(iter);
	}

	/* Definition einer einzelnen Variablen oder Konstanten */
	void IntFuncDefBase::fdef_ins_val(symbol_str *sym_p, union ptr_sym_u *up, int mod, enum etp tp_cd, val_str *vp)
	{
		val_str v;
		bool s, d;

		switch(mod) {
			case DEF_CONST:
				if (sym_p->tp_sym == TPK_VALUE)
					fdef_const_val(up, tp_cd, vp);
				else if (sym_p->tp_sym == TPK_SET)
					fdef_set_val(up, tp_cd, vp);
				else if (sym_p->tp_sym == TPK_PROBLEM_MAIN)
					fdef_problem_val(up, tp_cd, sym_p->sub_symtab, vp);
				else if (TP_IS_TUPEL(sym_p->tp_sym))
					fdef_tupel_val(up, tp_cd, false, RANK_TP_TUPEL(sym_p->tp_sym), vp);
				else if (TP_IS_TUPELSET(sym_p->tp_sym))
					fdef_tupel_val(up, tp_cd, true, RANK_TP_TUPEL(sym_p->tp_sym), vp);
				break;
			case DEF_VAR:
				fdef_var(up);
				break;
		}
	}

	/* Definition Konstante */
	void IntFuncDefBase::fdef_const_val(union ptr_sym_u *up, enum etp tp_cd, val_str *vp)
	{
		if(! up->c) {
			up->c = (const_str *) glvar->c_getmem(sizeof(const_str));
			up->c->tp = TPP_EMPTY;
		}
		else if (up->c->c_val.t != TPP_EMPTY)
		{
			if (tp_cd != TPP_EMPTY && up->c->tp != TPP_EMPTY && tp_cd != up->c->tp)
			{
				FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
				SPRINT_ERR_2(ERROR_DEFTYPE_CONST, glvar->temp_buf);
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);
			}
            else if (glvar->args->constDefWarnings())
			{
				FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
				SPRINT_ERR_2(ERROR_DEF_CONST, glvar->temp_buf);
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);
			}
		}

		if (vp)
			up->c->c_val = *vp;
		else
			pop_val(&(up->c->c_val));

		if (tp_cd != TPP_EMPTY)
			up->c->tp = tp_cd;

		/* Noetigenfalls noch Typ konvertieren: in vorgegebenen Typ oder wenn string in gespeicherten string */
		if (up->c->tp != TPP_EMPTY && up->c->tp != up->c->c_val.t)
		{
			val_str vd;
			fconv_to_etp(&(up->c->c_val), &vd, up->c->tp, (char *)0, (tp_cd != TPP_EMPTY));
			if (up->c->c_val.t == TP_STR)
				free(up->c->c_val.v.s);
			up->c->c_val = vd;
		}
		else if (up->c->c_val.t == TP_STR)
		{
			char *str = up->c->c_val.v.s;
			if (!glvar->string_to_istr(&(up->c->c_val), str, false))
				free(str);

			if (up->c->tp == TP_STR)
				up->c->tp = TP_ISTR;
		}

		/* Typklasse muss nun einfacher Wert sein */
		if (TYPECLASS(up->c->c_val.t) != TPK_VALUE || (up->c->tp != TPP_EMPTY && TYPECLASS(up->c->tp) != TPK_VALUE))
		{
			FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
			SPRINT_ERR_2(ERROR_NOT_TPK_VALUE, glvar->temp_buf);
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
		}

#ifdef PROTO
		if(glvar->is_protokoll) {
			FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
			PROTO_OUT_2(" constant %s defined", glvar->errbuf);
			if(up->c->c_val.t != TP_DBL) { PROTO_OUT_2(" = %d", up->c->c_val.v.i); }
                       else { PROTO_OUT_2(" = %f", up->c->c_val.v.f); }
		}
#endif
	}

	/* Definition Set */
	void IntFuncDefBase::fdef_set_val(union ptr_sym_u *up, enum etp tp_cd, val_str *vp)
	{
		if(! up->s) {
			up->s = (val_str *) glvar->c_getmem(sizeof(val_str));
		}
		else if (up->s->t != TPP_EMPTY)
		{
            if (glvar->args->constDefWarnings())
			{
				FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
				SPRINT_ERR_2(ERROR_DEF_CONST, glvar->temp_buf);
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);
			}
		}

		if (vp)
			*(up->s) = *vp;
		else
			pop_val(up->s);

		/* Noetigenfalls noch Typ konvertieren */
		if (tp_cd != TPP_EMPTY)
		{
			val_str vd;
			fconv_to_etp(up->s, &vd, tp_cd, (char *)0, true);
			*(up->s) = vd;
		}

		/* Typklasse muss nun Set sein */
		if (TYPECLASS(up->s->t) != TPK_SET)
		{
			FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
			SPRINT_ERR_2(ERROR_NOT_TPK_SET, glvar->temp_buf);
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
		}

		/* Set fuer Array muss umgewandelt werden, damit er unabhaengig von Aenderungen des Arrays wird */
		if (SET_USE_ARRAY(up->s->t))
		{
			SetIterator iter_set(glvar);
			val_str set;

			iter_set.set.t = TP_SET_ALL;
			set = defset_array(true, up->s->v.array, &iter_set);
			*(up->s) = set;
		}

		if (SET_USE_POINTER(up->s->t) && !up->s->v.set->is_gl_mem)
		{
			/* Reservierten Speicher fuer den Set jetzt in Speicherverwaltung aufnehmen */
			glvar->ins_mem((void *)(up->s->v.set));
			up->s->v.set->is_gl_mem = true;
		}

#ifdef PROTO
		if(glvar->is_protokoll) {
			char buf[101];
			FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
			FILEIO->write_set_to_str(buf, up->s, 100);
			PROTO_OUT_3(" set %s defined = %s", glvar->errbuf, buf);
		}
#endif
	}

	/* Definition CMPL-Problem */
	void IntFuncDefBase::fdef_problem_val(union ptr_sym_u *up, enum etp tp_cd, SymbolTab *sub_symtab, val_str *vp)
	{
		if (!sub_symtab)
		{
			FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
			SPRINT_ERR_2("cmpl problem '%s' invalid", glvar->temp_buf);	//TODO
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, glvar->errbuf);
		}

		if (! up->s) {
			up->s = (val_str *) glvar->c_getmem(sizeof(val_str));
		}
		else if (up->s->t != TPP_EMPTY)
		{
			if (up->s->v.subsyms->symtab && up->s->v.subsyms->symtab != sub_symtab)
			{
				FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
				SPRINT_ERR_2("incompatible assignment to cmpl problem '%s'", glvar->temp_buf);	//TODO
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			}
                        else if (glvar->args->constDefWarnings())
			{
				FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
				SPRINT_ERR_2(ERROR_DEF_CONST, glvar->temp_buf);
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);
			}
		}

		if (vp)
			*(up->s) = *vp;
		else
			pop_val(up->s);

		/* Noetigenfalls noch Typ konvertieren */
		if (tp_cd != TPP_EMPTY)
		{
			val_str vd;
			fconv_to_etp(up->s, &vd, tp_cd, (char *)0, true);
			*(up->s) = vd;
		}

		/* Typklasse muss nun cmpl-Problem sein */
		if (TYPECLASS(up->s->t) != TPK_PROBLEM_MAIN)
		{
			FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
			SPRINT_ERR_2("value to assign '%s' is not a cmpl problem", glvar->temp_buf);
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
		}

		if (up->s->v.subsyms->symtab)
		{
			// up->s.v.subsyms kopieren
			up->s->v.subsyms = up->s->v.subsyms->copy(glvar);
		}
		else
		{
			// Subsymboltabelle initialisieren
			up->s->v.subsyms->symtab = sub_symtab;
			up->s->v.subsyms->symvalInit(glvar);
		}
	}

	/* Definition Tupel oder Tupelset */
	void IntFuncDefBase::fdef_tupel_val(union ptr_sym_u *up, enum etp tp_cd, bool tps, int rank, val_str *vp)
	{
		if (! up->s) {
			up->s = (val_str *) glvar->c_getmem(sizeof(val_str));
		}
		else if (up->s->t != TPP_EMPTY)
		{
            if (glvar->args->constDefWarnings())
			{
				FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
				SPRINT_ERR_2(ERROR_DEF_CONST, glvar->temp_buf);
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);
			}
		}

		if (vp)
			*(up->s) = *vp;
		else
			pop_val(up->s);

		/* Typ muss stimmen (tp_cd wird hier derzeit nicht berücksichtigt) */
		if ((tps && !TP_IS_TUPELSET(up->s->t)) || (!tps && !TP_IS_TUPEL(up->s->t)) || rank != RANK_TP_TUPEL(up->s->t))
		{
			// Tupel anstelle Tupelset doch akzeptieren, weil Operation INTERSECT und INTERSECT_RR abhängig von den Operanden Tupel liefert
			// Falschen Rang doch akzeptieren, weil Ergebnisrang fuer Operation INTERSECT_RR fuer den Compiler nicht ableitbar ist
			// (Das kann in Abhaengigkeit von der Verwendung des zugewiesenen Symbols zu Folgefehlern fuehren, ist aber innerhalb des derzeitigen Konzepts nicht vermeidbar)
			if ((!TP_IS_TUPELSET(up->s->t) && !TP_IS_TUPEL(up->s->t)) || (!tps && !TP_IS_TUPEL(up->s->t)))
			{
				FILEIO->ferror_symname(glvar->temp_buf, false, ERRBUFLEN);
				SPRINT_ERR_4("value assigned to '%s' is not a %s of rank %d", glvar->temp_buf, (tps ? "tupelset" : "tupel"), rank);
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			}
			else
			{
#ifdef PROTO
				FILEIO->ferror_symname(glvar->temp_buf, false, ERRBUFLEN);
				if (tps && !TP_IS_TUPELSET(up->s->t))
					PROTO_OUT_2("\nWarning: symbol '%s': conflict tupel/tupelset between compiler and interpreter\n", glvar->temp_buf);
				if (rank != RANK_TP_TUPEL(up->s->t))
					PROTO_OUT_2("\nWarning: symbol '%s': conflict tupel rank between compiler and interpreter\n", glvar->temp_buf);
#endif

				// Wenn skalares Symbol, dann jetzt nachträglich noch Typ anpassen, falls fuer spaetere Operationen der Typ eine Rolle spielt
				// (wenn nicht skalar, dann nicht moeglich, da ja noch andere Werte, moeglicherweise fuehrt das dann spaeter zu Fehlern)
				if (glvar->symbol_akt_p->dim == 0)
					glvar->symbol_akt_p->tp_sym = up->s->t;
			}
		}

		/* Reservierten Speicher jetzt in globale Speicherverwaltung aufnehmen */
		if (TP_IS_TUPELSET(up->s->t))
		{
			Tupelset *t = up->s->v.tps;
			if (! t->globAlloc())
				t->setToGlobAlloc(glvar);
		}
        else if (TP_IS_TUPEL(up->s->t) && RANK_TP_TUPEL(up->s->t) > 1)
		{
			tupel_t t = up->s->v.tp;
			if (! TupelUtil::globAlloc(t))
				TupelUtil::setToGlobAlloc(glvar, t);
		}

#ifdef PROTO
		if(glvar->is_protokoll) {
			char buf[101];
			FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
			PROTO_OUT_2(" tupel/tupelset %s defined", glvar->errbuf);
		}
#endif
	}

	/* Definition Variable */
	void IntFuncDefBase::fdef_var(union ptr_sym_u *up)
	{
		if(! up->v)
		{
			up->v = (var_str *) glvar->c_getmem(sizeof(var_str));
			up->v->v_type = TPP_EMPTY;
		}

		if (up->v->v_type == TPP_EMPTY)
			up->v->v_type = TP_VAR_F_NEW;

		else
		{
			/* Variable darf nicht mehrfach definiert werden */
			FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
			if(up->v->v_is_koeff)
			{
				SPRINT_ERR_2(ERROR_USED_VAR, glvar->temp_buf);
			}
			else
			{
				SPRINT_ERR_2(ERROR_DEF_VAR, glvar->temp_buf);
			}
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);

			up->v->v_type = TP_VAR_NEW;
		}
	}



	//********************************************************
	// IntFuncDefConst
	//********************************************************

	/* Definition Konstante (auch Matrix) */
	void IntFuncDefConst::exec(val_str *vp)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			enum etp tp = (enum etp) vp->v.c.par;
			unsigned short sub = vp->v.c.sub & 0x00ff;
			unsigned short dim_rhs = (vp->v.c.sub & 0xff00) >> 8;
			int i = vp[1].v.i;
			symbol_str *s_p;

			if (sub == SYM_UNDEF_ERROR)
			{
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "Invalid use of undefined symbol"); //TODO
				return;
			}
			else if (sub == SYM_MAIN)
			{
				s_p = glvar->symbol_table + (i / MAX_ANZ_IND_1);
			}
			else
			{
				// Subsymboltabelle liegt als Wert auf dem Stack, aber darüber liegen die zuzuweisenden Werte
				//TODO: vorher SWAP-Zwischencodebefehl, der über Listenkennzeichnung die zuzuweisenden Werte identifiziert
				
				//vorläufig: Wert im Stack suchen
				val_str *vp = searchStackList(glvar->value_stack + (glvar->stack_ptr - 1), glvar->ind_inf_ptr[-1].i_anz_ind);
				PROTO_OUT_3("\n  stack %d (%d)", (int)(glvar->stack_ptr - 1), (int)(glvar->ind_inf_ptr[-1].i_anz_ind));
				PROTO_OUT_3("\n  subsym %d (%d)", (int)(vp - glvar->value_stack), (int)(vp->t));


				if (!TP_WITH_SUBSYM(vp->t) || !vp->v.subsyms || !vp->v.subsyms->symtab || !vp->v.subsyms->symval || i >= vp->v.subsyms->symtab->count())
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "Ungültiger Zugriff auf Subsymbol"); //TODO
					return;
				}

				s_p = vp->v.subsyms->symval + i;
			}

			int c_i = s_p->dim;
			PROTO_OUT_4("  %d (%s: %d)", i, s_p->s_name, c_i);

			glvar->symbol_akt_p = s_p;
			if((--(glvar->ind_inf_ptr)) < glvar->ind_inf_table)
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_PRV_SET_IND);

			fdef_val(s_p, c_i, (int)DEF_CONST, tp, dim_rhs, false, NULL);

			//vorläufig: da Subsymboltabelle im Stack gesucht und noch nicht entfernt, nun entfernen
			if (sub != SYM_MAIN)
			{
				val_str v;
				pop_val(&v);
			}

			ffree_index_sets(glvar->ind_inf_ptr);
		}
	}

	/* Sucht den Wert auf dem Stack unter der Wertliste, die bis vp reicht und dim Dimensionen hat */
	//TODO: entfällt, stattdessen richtige Zwischencodefunktion für SWAP (wenn Wertliste tatsächlich als Liste gekennzeichnet ist)
	val_str *IntFuncDefConst::searchStackList(val_str *vp, int dim)
	{
		if (dim == 0)
			return vp-1;

		int n = vp->v.i;
		if (dim == 1)
			return vp-n-1;

		vp--;
		for (int i = 0; i < n; i++)
			vp = searchStackList(vp, dim - 1);

		return vp;
	}


	//********************************************************
	// IntFuncDefVar
	//********************************************************

	/* Definition Variable (auch Matrix) */
	void IntFuncDefVar::exec(val_str *vp)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			int i = vp[1].v.i;
			symbol_str *s_p = glvar->symbol_table + (i / MAX_ANZ_IND_1);
			int c_i = i % MAX_ANZ_IND_1;
			PROTO_OUT_2("  %d", i);

			glvar->symbol_akt_p = s_p;
			if((--(glvar->ind_inf_ptr)) < glvar->ind_inf_table)
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_PRV_SET_IND);

			fdef_val(s_p, c_i, (int)DEF_VAR, TPP_EMPTY, -1, false, NULL);

			ffree_index_sets(glvar->ind_inf_ptr);
		}
	}



	//********************************************************
	// IntFuncResetVarB
	//********************************************************

	/* Vorgesehene Schranken fuer Variablen zuruecksetzen */
	void IntFuncResetVarB::exec(val_str *vp)
	{
		glvar->var_dn_b.b_is_b = false;
		glvar->var_up_b.b_is_b = false;
	}


	//********************************************************
	// IntFuncDefDnVar
	//********************************************************

	/* Sieht untere Schranke fuer Variablen vor */
	void IntFuncDefDnVar::exec(val_str *vp)
	{
		val_str v;
		glvar->var_dn_b.b_is_b = true;
		pop_val(&v);
		fconv_to_etp(&v, &(glvar->var_dn_b.b_val_b), TPU_NUM, (char *)0, false);
	}


	//********************************************************
	// IntFuncDefUpVar
	//********************************************************

	/* Sieht obere Schranke fuer Variablen vor */
	void IntFuncDefUpVar::exec(val_str *vp)
	{
		val_str v;
		glvar->var_up_b.b_is_b = true;
		pop_val(&v);
		fconv_to_etp(&v, &(glvar->var_up_b.b_val_b), TPU_NUM, (char *)0, false);
#ifndef UPPER_BOUND
		FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, ERROR_BOUND_UP_IGNORE);
#endif
	}


	//********************************************************
	// IntFuncDefVarType
	//********************************************************

	/* Variablen mit Typ und Schranken versehen */
	void IntFuncDefVarType::exec(val_str *vp)
	{
		/* Variablentyp als Unterkommando */
		enum etp vtype = (enum etp)(vp->v.c.sub);
		int n,m;
		array_str *arr_p;
		nonarray_str *narr_p;

		PROTO_OUT_2("  %d", (int)vtype);

		for(glvar->symbol_akt_p=glvar->symbol_table, n=0; n<glvar->symbol_anzahl; n++, glvar->symbol_akt_p++)
		{
			if(glvar->symbol_akt_p->tp_sym == TPK_VAR)
			{
				/* Ohne Index */
				narr_p = glvar->symbol_akt_p->val_ip0;
				if (narr_p && narr_p->attr.s_newvar && VAR_IS_NEW(narr_p->elem.v->v_type))
				{
					glvar->ind_inf_ptr->i_anz_ind = 0;
					set_vartype_bounds(narr_p->elem.v, vtype);
					narr_p->attr.s_newvar = false;
				}

				/* Mit Indizes */
				for(m=1; m<MAX_ANZ_IND_1; m++)
				{
					arr_p = glvar->symbol_akt_p->val_ip[m-1];
					if (arr_p && arr_p->attr.s_newvar)
					{
						glvar->ind_inf_ptr->i_anz_ind = m;
						fdef_vartype(arr_p, 0, vtype);
						arr_p->attr.s_newvar = false;
					}
				}
			}
		}
	}

	/* Variablen-Array mit Typ und Schranken versehen */
	void IntFuncDefVarType::fdef_vartype(array_str *array_p, int ii, enum etp vtype)
	{
		int a, n;
		struct a_str *arr_p;
		var_str *vp;
		array_str *ap;

		if (!array_p)
			return;

		for (a = 0; a < 3; a++)
		{
			arr_p = (a==0 ? &(array_p->a_dir) : (a==1 ? &(array_p->a_num) : &(array_p->a_str)));
			if (!arr_p || arr_p->cap == 0)
				continue;

#ifdef PROTO
			glvar->ind_inf_ptr->i_ind[ii].cur_tp = (a == 2 ? TP_ISTR : TP_INT);
#endif

			if (array_p->attr.t == PTR_TP_VAR)
			{
				/* Fuer Variablen Typ setzen */
				for (n = 0, vp = arr_p->array.v; n < arr_p->len; n++, vp++)
				{
					if (VAR_IS_NEW(vp->v_type))
					{
#ifdef PROTO
						glvar->ind_inf_ptr->i_ind[ii].cur_val = (a == 0 ? n : arr_p->index[n]);
#endif
						set_vartype_bounds(vp, vtype);
					}
				}
			}
			else if (array_p->attr.t == PTR_TP_ARRAY)
			{
				/* Rekursiv fuer naechsten Index */
				for (n = 0, ap = arr_p->array.a; n < arr_p->len; n++, ap++)
				{
#ifdef PROTO
					glvar->ind_inf_ptr->i_ind[ii].cur_val = (a == 0 ? n : arr_p->index[n]);
#endif
					fdef_vartype(ap, ii+1, vtype);
				}
			}
		}
	}

	/* Setzt fuer eine Variable den Typ und die Schranken */
	void IntFuncDefVarType::set_vartype_bounds(var_str *vp, enum etp vtype)
	{
		if(vp->v_type == TP_VAR_F_NEW) {
			vp->v_defnum = glvar->akt_defnum;
			vp->v_is_koeff = false;
			glvar->akt_defnum++;
		}
		vp->v_type = vtype;
#ifdef VAR_PROD
		vp->v_otype = vtype;
#endif
		if(vtype == TP_VAR_BIN) {
			vp->v_is_dn_b = vp->v_is_up_b = true;
			vp->v_dn_b.t = TP_INT;
			vp->v_dn_b.v.i = 0;
			vp->v_up_b.t = TP_INT;
			vp->v_up_b.v.i = 1;
		}
		else {
			if(glvar->var_dn_b.b_is_b) {
				vp->v_is_dn_b = true;
				vp->v_dn_b = glvar->var_dn_b.b_val_b;
			}
			else vp->v_is_dn_b = false;

			if(glvar->var_up_b.b_is_b) {
				vp->v_is_up_b = true;
				vp->v_up_b = glvar->var_up_b.b_val_b;
			}
			else vp->v_is_up_b = false;
		}
#ifdef PROTO
		if(glvar->is_protokoll) {
			FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
			PROTO_OUT_4("\n      variable %s defined(%d, Typ %d)", glvar->errbuf, vp->v_defnum, (int)vtype);
		}
#endif
	}

	
	//********************************************************
	// IntFuncInitCmplData
	//********************************************************

	/* Initialisierung Symbole aus cmplData ausfuehren */
	void IntFuncInitCmplData::exec(val_str *vp)
	{
        if (!glvar->cmplData->cmplDataInterpreter(this))
                //FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->cmplData->errorMsg().c_str());
                FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, glvar->cmplData->errorMsg().c_str());
	}

	/* Initialisierung fuer ein Symbol aus cmplData, Werte dafuer muessen schon auf dem Stack liegen */
	void IntFuncInitCmplData::insertCmplDataValues(int symnr, int *sets, int count, enum etp settp, bool modeIndices, bool withDefault)
	{
		symbol_str *s_p = glvar->symbol_table + (symnr / MAX_ANZ_IND_1);
		int dim = s_p->dim;
		int errcnt = 0;
		const char *errstr = NULL;
		glvar->symbol_akt_p = s_p;

		PROTO_OUT_4("  init data Symbol %d (%s: %d)", symnr, s_p->s_name, dim);

		// Sets/Tupelsets holen und Dimensionalitaet testen
		Tupelset *tps = tupelsetForSetArray(sets);
		if (sets[0] >= 0 && (!tps || tps->rank() != dim))
		{
			errstr = (tps ? "array rank from cmplData inconsistent" : "sets or tupelsets from cmplData inconsistent");
			errcnt = count;
		}
		else
		{
			// Indexinfo aus Tupelset fuellen
			if (!tps)
				push_int(0);
			set_index_info(true, false, tps, true);

            // In der weiteren Verarbeitung wird der Indexinfo-Stackpointer direkt auf dem Element erwartet, statt auf dem naechsten freien, deshalb hier schon runtersetzen
			// TODO: Moeglicherweise ist die aufgerufene Verarbeitung sowieso in dieser Hinsicht problematisch und die Anpassungen sollten lieber dort erfolgen
            (glvar->ind_inf_ptr)--;

			// Eigentliche Initialisierung fuer das Symbol aus cmplData ausfuehren
			errstr = doInsertCmplDataValues(s_p, count, settp, modeIndices, withDefault, &errcnt);
		}
		
		// Wenn Fehler, dann einfach die Werte vom Stack, ohne weiter etwas damit zu machen
		if (errcnt > 0 || errstr)
		{
			if (errcnt > 0)
			{
				long len;
				int d;
				pop_list_elems(errcnt, false, &len, &d);
			}

			// Fehlermeldung ausgeben
			if (errstr)
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, errstr);
		}

		// Falls Tupelset hier lokal reserviert, jetzt wieder freigeben
		if (tps && !tps->globAlloc())
			Tupelset::freeTupelset(glvar, tps);
	}

	/* Initialisierung fuer ein Symbol aus cmplData, Werte dafuer muessen schon auf dem Stack liegen */
	const char *IntFuncInitCmplData::doInsertCmplDataValues(symbol_str *s_p, int count, enum etp settp, bool modeIndices, bool withDefault, int *errcnt)
	{
		// Wenn Set oder Tupelset, dann val_str-Wert dafür aus Elementen konstruieren und als einzigen Wert wieder auf den Stack
		val_str v;
		if (TYPECLASS(s_p->tp_sym) == TPK_SET || TP_IS_TUPELSET(s_p->tp_sym))
		{
			// Arrays aus Sets oder Tupelssets in cmplData nicht vorgesehen
			if (s_p->dim > 0)
			{
				*errcnt = count;
				return "arrays of sets or tupelsets are not allowed in cmplData";
			}

			if (modeIndices || withDefault)
			{
				*errcnt = count;
				return "invalid syntax for set or tupelset in cmplData";
			} 

			// Unterscheidung Set und Tupelset
			if (TYPECLASS(s_p->tp_sym) == TPK_SET)
			{
				// Nur folgende Typen der Set-Darstellung in cmplData vorgesehen
				if (settp != TP_SET_ALG && settp != TP_SET_ALG_S1 && settp != TP_SET_ENUM)
				{
					*errcnt = count;
					return "invalid set type from cmplData";
				}

				// Anzahl Werte auf dem Stack muss zur Set-Darstellung passen
				if ((settp == TP_SET_ALG && count != 3) || (settp == TP_SET_ALG_S1 && count != 2))
				{
					*errcnt = count;
					return "invalid value count for algorithmic set from cmplData";
				}

				// Fuer Enumerationsset noch Anzahl auf den Stack
				if (settp == TP_SET_ENUM)
					push_int(count);

				// Werte vom Stack in Set wandeln
				construct_set(settp, &v);
			}
			else
			{
				// Tupelset
				int rank = RANK_TP_TUPEL(s_p->tp_sym);
				if (rank == 0)
				{
					if (count != 0)
					{
						*errcnt = count;
						return "null tupelset cannot have index values";
					}

					v.t = TPK_TUPELSET_N(0);
					v.v.tps = glvar->gl_nulltupelset;
				}
				else
				{
					// Werte fuer Tupelset vom Stack, daraus Tupelset erstellen
					long len;
					int d;
					val_str *arr = pop_list_elems(count, false, &len, &d);
					Tupelset::constructValEnum(glvar, arr, count, rank, true, &v);
				}
			}

			// Erstellten set oder tupelset wieder auf den Stack
			push_val(&v);
			count = 1;
		}

		// Gegebenenfalls Defaultwert uebernehmen (nur fuer modeIndices sinnvoll)
		if (withDefault && modeIndices)
		{
			count--;
			val_str *vp = stack_rel_val(count);
			fdef_val(s_p, s_p->dim, DEF_CONST, TPP_EMPTY, 0, false, vp);
		}

		// Werte vom Stack in das Symbol übernehmen
		push_int(count);
		fdef_val(s_p, s_p->dim, DEF_CONST, TPP_EMPTY, 1, modeIndices, NULL);

		// Gegebenenfalls noch Defaultwert vom Stack raeumen
		if (withDefault)
			pop_val(&v);

		return NULL;
	}

	/* Holt Tupelset aus Sets oder Tupelsets, die als Symbolnummer in einem Array vorliegen (Ende darin mit -1 markiert) */
	Tupelset *IntFuncInitCmplData::tupelsetForSetArray(int *sets)
	{
		symbol_str *s_p;
		int i, indLen = 0;
		union ptr_sym_u up;
		bool found;
		enum ptrtp pt;

		// Wenn gar kein Array, dann NULL
		if (sets[0] < 0)
			return NULL;

		for (i = 0; i < MAX_ANZ_IND_1 && sets[i] >= 0; i++)
		{
			s_p = glvar->symbol_table + (sets[i] / MAX_ANZ_IND_1);

			// Einzelne Symbole muessen skalar sein
			if (s_p->dim != 0)
				return NULL;

			// Set/Tupelset holen aus Symbol holen
			glvar->ind_inf_ptr->i_anz_ind = 0;
			up = ffetch_p(s_p, (array_str *)0, 0, &found, &pt);
			if (!found || pt != PTR_TP_VALUE || !up.s || (!TP_IS_TUPELSET(up.s->t) && TYPECLASS(up.s->t) != TPK_SET))
				return NULL;

			// Wenn nur ein Wert und der schon Tupelset, dann direkt zurück
			if (i == 0 && sets[1] < 0 && TP_IS_TUPELSET(up.s->t))
				return up.s->v.tps;

			// Sonst erstmal auf den Stack
			push_val(up.s);
			indLen += (TP_IS_TUPELSET(up.s->t) ? up.s->v.tps->cnt() : 1);
		}

		// Tupelset erstellen
		long len;
		int d;
		val_str *arr = pop_list_elems(i, false, &len, &d);
		return Tupelset::constructTupel(glvar, indLen, arr, i, false);
	}
}


/*

SET_INDIZES:
	Unterscheidung:
		A) Alle Indizes keine Tupelsets -> wie bisher
		B) Nur Tupelsets ohne Enumerationsanteil unter den Indizes -> disaggregieren in einzelne Sets, dann wie bisher
		C) Einzelner Tupelset mit Enumerationsanteil -> Indexinfo aus diesem Tupelset
		D) Tupelset mit Enumerationsanteil und weitere Indizes -> Neuen gesamthaften Tupelset konstruieren, Indexinfo aus diesem neuen Tupelset
	Auch Funktion mit Funktionalität für C), auch aufgerufen für eigentlich disaggregierbare Tupelsets (für Aufruf aus IntFuncInitCmplData, für Tupelsets mit Rang >= 2)
	1. Indexinfo vom Stack füllen: Wenn ganz disaggregierbar, dann als einzelne Sets/Skalare; sonst als einen Tupelset
			Neues Tupelset lokal allokieren; Freigabe wenn vom Indexstack entfernt
	2a. Für IntFuncInitCmplData, kein Tupelset (oder Rang 0): Indexinfo mit 0 Indizes
	2b. Für IntFuncInitCmplData, Tupelset Rang 1: Tupelset disaggregieren
	2c. Für IntFuncInitCmplData, Tupelset Rang >= 2: als ein Tupelset
			Neues Tupelset lokal allokieren; NICHT freigeben wenn vom Indexstack entfernt

	2. ->
				if (!tps)
					push_int(0);
				set_index_info(true, false, tps, true);

Zusatzkennzeichen für fdef_vekts:
	Dimensionalität rechte Seite

Erweiterung fdef_vekts:
	Zusätzliches Kennzeichen Dimensionalität rechts:
		Dimensionalität rechts != 1:
			Indexinfo ist Tupelset -> Fehler
			Indexinfo hat andere Dimensionalität -> Fehler
			Ausführung wie bisher
		Dimensionalität rechts == 1:
			Indexinfo hat Dimensionalität 0:
				Linke Seite vom Stack holen
				Wenn Länge ungleich 1 -> Fehler
				Linke Seite skalar zuweisen (wie bisher)
			Indexinfo hat Dimensionalität 1 und ist kein Tupelset:
				Ausführung wie bisher
			Indexinfo ist kein Tupelset -> Indexinfo in Tupelset wandeln, weiter
			Ausführung für Tupelset:
				Werte als Liste vom Stack holen
				Iteration über das Tupelset:
					Neues Indexinfo aus dem Tupel füllen
					Jeweiligen Wert aus der Liste skalar zuweisen
					(vielleicht direkter ausführbar ohne zusätzliches Indexinfo und ohne Wert nochmal auf den Stack zu legen)

IntFuncInitCmplData:
	Indexinfo bereitstellen:
		Wenn Tupelset Rang 0 oder 1 -> disaggregieren in Indexinfo ohne Tupelset
		Sonst -> Indexinfo mit Tupelset erstellen
	Werte/Defaultwert eintragen:
		Rang 0 -> Als Einzelwert (Dimensionalität 0)
		Sonst -> Anzahl hinzufügen, dann als Dimensionalität 1
		Dann fdef_vekts
	Einzelzuordnungen eintragen:
		Sondermodus (den auch fdef_vekts in Fall A) aufruft)


*/
