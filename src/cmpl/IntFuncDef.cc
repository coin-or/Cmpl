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
#include "gencode.h"


namespace cmpl
{
	//********************************************************
	// IntFuncDefBase
	//********************************************************

	/* Definition Variable oder Konstante
	   (auch Matrix, dann auch Aufbau Vektorbaum) */
	void IntFuncDefBase::fdef_vekts(symbol_str *sym_p, int c_i, array_str *arr_p, int ii, int mod, enum etp tp_cd)
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

			fdef_ins_val(sym_p, &(sym_p->val_ip0->elem), mod, tp_cd);
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
			if(mod==DEF_VAR)
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
				fdef_ins_val(sym_p, &up, mod, tp_cd);
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
				fdef_vekts(sym_p, c_i, n_arr_p, ii+1, mod, tp_cd);
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

	/* Definition einer einzelnen Variablen oder Konstanten */
	void IntFuncDefBase::fdef_ins_val(symbol_str *sym_p, union ptr_sym_u *up, int mod, enum etp tp_cd)
	{
		val_str v;
		bool s, d;

		switch(mod) {
			case DEF_CONST:
				if (sym_p->tp_sym == TPK_VALUE)
					fdef_const_val(up, tp_cd);
				else if (sym_p->tp_sym == TPK_SET)
					fdef_set_val(up, tp_cd);
				else if (sym_p->tp_sym == TPK_PROBLEM_MAIN)
					fdef_problem_val(up, tp_cd, sym_p->sub_symtab);
				else if (TP_IS_TUPEL(sym_p->tp_sym))
					fdef_tupel_val(up, tp_cd, false, RANK_TP_TUPEL(sym_p->tp_sym));
				else if (TP_IS_TUPELSET(sym_p->tp_sym))
					fdef_tupel_val(up, tp_cd, true, RANK_TP_TUPEL(sym_p->tp_sym));
				break;
			case DEF_VAR:
				fdef_var(up);
				break;
		}
	}

	/* Definition Konstante */
	void IntFuncDefBase::fdef_const_val(union ptr_sym_u *up, enum etp tp_cd)
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
	void IntFuncDefBase::fdef_set_val(union ptr_sym_u *up, enum etp tp_cd)
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
	void IntFuncDefBase::fdef_problem_val(union ptr_sym_u *up, enum etp tp_cd, SymbolTab *sub_symtab)
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
	void IntFuncDefBase::fdef_tupel_val(union ptr_sym_u *up, enum etp tp_cd, bool tps, int rank)
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

		pop_val(up->s);

		/* Typ muss stimmen (tp_cd wird hier derzeit nicht berücksichtigt) */
		if ((tps && !TP_IS_TUPELSET(up->s->t)) || (!tps && !TP_IS_TUPEL(up->s->t)) || rank != RANK_TP_TUPEL(up->s->t))
		{
			FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
			SPRINT_ERR_4("value to assign '%s' is not a %s of rank %d", glvar->temp_buf, (tps ? "tupelset" : "tupel"), rank);
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
		}

		/* Reservierten Speicher jetzt in globale Speicherverwaltung aufnehmen */
		if (tps)
		{
			Tupelset *t = up->s->v.tps;
			if (! t->globAlloc())
				t->setToGlobAlloc(glvar);
		}
		else
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
			unsigned short sub = vp->v.c.sub;
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

			fdef_vekts(s_p, c_i, (array_str *)0, 0, (int)DEF_CONST, tp);

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

			fdef_vekts(s_p, c_i, (array_str *)0, 0, (int)DEF_VAR, TPP_EMPTY);

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

}

