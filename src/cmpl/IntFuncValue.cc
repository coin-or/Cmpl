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


#include "IntFuncValue.hh"
#include "gencode.h"


namespace cmpl
{
	//********************************************************
	// IntFuncLitVal
	//********************************************************

	/* Literaler Wert, Typ entsprechend Parameter */
	void IntFuncLitVal::exec(val_str *vp)
	{
		if (vp[1].t == TP_INT)
		{
			PROTO_OUT_2("  %ld", vp[1].v.i);
			push_int(vp[1].v.i);
		}
		else if (vp[1].t == TP_DBL)
		{
                       PROTO_OUT_2("  %f", vp[1].v.f);
			push_dbl(vp[1].v.f);
		}
		else if (vp[1].t == TP_ISTR || vp[1].t == TP_BIN || vp[1].t == TPP_NULL)
		{
			if (vp[1].t == TP_ISTR)
				{ PROTO_OUT_3("  (%ld) %s", vp[1].v.i, glvar->strings.strings_ind[vp[1].v.i]); }
			else
				{ PROTO_OUT_3("  %ld (%s)", vp[1].v.i, (vp[1].t == TP_BIN ? "binary" : "null")); }
			push_val(vp+1);
		}
		else
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
	}


	//********************************************************
	// IntFuncLitValShort
	//********************************************************

	/* Literaler int-Wert als eingebetteter Parameter */
	void IntFuncLitValShort::exec(val_str *vp)
	{
		PROTO_OUT_2("  %d", (int)(vp->v.c.par));
		push_int((long)(vp->v.c.par));
	}


	//********************************************************
	// IntFuncLitSet
	//********************************************************

	/* Literalen Set auf den Stack */
	void IntFuncLitSet::exec(val_str *vp)
	{
		val_str set;
		enum etp tp = (enum etp) vp->v.c.par;	/* Eingebetteter Parameter ist Typ des Sets */

		if (TYPECLASS(tp) == TPK_SET)
		{
			construct_set(tp, &set);
		}
		else
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
			set.t = TP_SET_EMPTY;
		}

		push_val(&set);
	}


	//********************************************************
	// IntFuncConstructTupel
	//********************************************************

	/* construct a new tupel or tupelset by tupel construction and push it on the value stack */
	void IntFuncConstructTupel::exec(val_str *vp)
	{
		val_str tup;

		// Liste holen
		long cnt, len;
		val_str *arr = pop_list(&cnt, &len);

		// Werte koennen keine Listen sein
		if (cnt != len)
		{
            SPRINT_ERR_2("wrong elements", write_val_to_temp_buf(arr));    //TODO
            FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			//TODO
		}

		// Wenn nur 1 Element und das schon Tupel oder Tupelset, dann ist das das Ergebnis
		if (cnt == 1 && (TP_IS_TUPEL(arr->t) || TP_IS_TUPELSET(arr->t)))
		{
			repush();
			return;
		}

		// Werte pruefen, Test ob Ergebnis Tupel oder Tupelset und mit welchem Rang
		val_str *p;
		int i;
		int rank = 0, fix = 0, indLen = 0;
		bool hasRank = true;

		for (i = 0, p = arr; i < cnt; i++, p++)
		{
			if (TP_IS_TUPEL(p->t))
			{
				rank += RANK_TP_TUPEL(p->t);
				fix += RANK_TP_TUPEL(p->t);
				indLen += RANK_TP_TUPEL(p->t);
			}
			else if (TP_IS_TUPELSET(p->t))
			{
				fix += p->v.tps->fix();
				indLen += p->v.tps->cnt();
				if (p->v.tps->hasRank())
					rank += p->v.tps->rank();
				else
					hasRank = false;
			}
			else
			{
				rank++;
				indLen++;
				if (TYPECLASS(p->t) != TPK_SET)
					fix++;
			}
		}

		if (hasRank && rank == fix)
		{
			// Tupel
			TupelUtil::constructVal(glvar, rank, arr, cnt, false, &tup);
		}
		else
		{
			// Tupelset
			Tupelset::constructValTupel(glvar, indLen, arr, cnt, false, &tup);
		}

		push_val(&tup);
	}


	//********************************************************
	// IntFuncConstructTupelset
	//********************************************************

	/* construct a new tupelset from an enumeration of tupels or tupelsets and push it on the value stack */
	void IntFuncConstructTupelset::exec(val_str *vp)
	{
		val_str tup;

		// Liste holen
		long cnt, len;
		val_str *arr = pop_list(&cnt, &len);

		// Werte koennen keine Listen sein
		if (cnt != len)
		{
            SPRINT_ERR_2("wrong elements", write_val_to_temp_buf(arr));    //TODO
            FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			//TODO
		}

		Tupelset::constructValEnum(glvar, arr, cnt, false, &tup);
		push_val(&tup);
	}


	//********************************************************
	// IntFuncFetchConst
	//********************************************************

	/* Holen Wert Konstante, auf den Stack legen */
	void IntFuncFetchConst::exec(val_str *vp)
	{
		ffetch_const(vp, false);
	}

	/* Holen Wert einer Konstante oder alle Werte eines Arrays aus Konstanten, auf den Stack legen */
	void IntFuncFetchConst::ffetch_const(val_str *vp, bool fl)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			int i = vp[1].v.i;
			unsigned short sub = vp->v.c.sub;
			val_str v;
			ind_inf_one_str *inf_p;
			union ptr_sym_u up;
			bool found;
			enum ptrtp pt;
			symbol_str *s_p;
			int n, c_i;

			// Indexanzahl und Indizes liegen auf dem Stack (aber wenn fl, dann schon geholt)
			if (fl)
			{
				fset_free_ind(i);
			}
			else
			{
				c_i = pop_int();
				for(n=c_i-1; n>=0; n--)
				{
					inf_p = glvar->ind_inf_ptr->i_ind + n;
					inf_p->is_free = false;
					inf_p->cur_val = pop_index(&(inf_p->cur_tp));
				}
			}
						
			if (sub == SYM_UNDEF_ERROR)
			{
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "Invalid use of undefined symbol"); //TODO
				push_int(1);
				return;
			}
			else if (sub == SYM_MAIN)
			{
				s_p = glvar->symbol_table + (i / MAX_ANZ_IND_1);
			}
			else
			{
				// Subsymboltabelle liegt als Wert auf dem Stack
				val_str v;
				pop_val(&v);
				if (!TP_WITH_SUBSYM(v.t) || !v.v.subsyms || !v.v.subsyms->symtab || !v.v.subsyms->symval || i >= v.v.subsyms->symtab->count())
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "Ungültiger Zugriff auf Subsymbol"); //TODO
					push_int(1);
					return;
				}

				s_p = v.v.subsyms->symval + i;
			}

			// Indexanzahl muss passen
			if (!fl && c_i != s_p->dim)
			{
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "Indexanzahl stimmt nicht"); //TODO (auch Symbolname)
				push_int(1);
				return;
			}

			c_i = s_p->dim;

			PROTO_OUT_4("  %d (%s: %d)", i, s_p->s_name, c_i);

			glvar->symbol_akt_p = s_p;
			glvar->ind_inf_ptr->i_anz_ind = c_i;

			up = ffetch_p(s_p, (array_str *)0, 0, &found, &pt);
			if (!found || pt == PTR_TP_EMPTY || pt == PTR_TP_ARRAY || pt == PTR_TP_VAR ||
					(pt == PTR_TP_CONST && (!up.c || up.c->c_val.t == TPP_EMPTY)) ||
					(pt == PTR_TP_VALUE && (!up.s || up.s->t == TPP_EMPTY)))
			{
				FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
				SPRINT_ERR_2(ERROR_NODEF_CONST, glvar->temp_buf);
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
				push_int(1);
				return;
			}

			if (pt == PTR_TP_CONST)
			{
				const_str *cp = up.c;
				push_val(&(cp->c_val));
			}
			else if (pt == PTR_TP_VALUE)
			{
				push_val(up.s);
			}
		}
	}
	

	//********************************************************
	// IntFuncFConstFree
	//********************************************************

	/* Holen alle Werte eines Arrays aus Konstanten, auf den Stack legen */
	void IntFuncFConstFree::exec(val_str *vp)
	{
		ffetch_const(vp, true);
	}


	//********************************************************
	// IntFuncFetchVals
	//********************************************************

	/* Holen Werte Konstantenvektor */
	void IntFuncFetchVals::exec(val_str *vp)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			int i = vp[1].v.i;
			unsigned short sub = vp->v.c.sub;
			symbol_str *s_p;
			long res = 0;

			if (sub == SYM_UNDEF_ERROR)
			{
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "Invalid use of undefined symbol"); //TODO
				push_int(1);
				return;
			}
			else if (sub == SYM_MAIN)
			{
				s_p = glvar->symbol_table + (i / MAX_ANZ_IND_1);
			}
			else
			{
				// Subsymboltabelle liegt als Wert auf dem Stack
				val_str v;
				pop_val(&v);
				if (!TP_WITH_SUBSYM(v.t) || !v.v.subsyms || !v.v.subsyms->symtab || !v.v.subsyms->symval || i >= v.v.subsyms->symtab->count())
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "Ungültiger Zugriff auf Subsymbol"); //TODO
					push_int(1);
					return;
				}

				s_p = v.v.subsyms->symval + i;
			}

			int c_i = s_p->dim;
			PROTO_OUT_4("  %d (%s: %d)", i, s_p->s_name, c_i);


			glvar->symbol_akt_p = s_p;
			if((--(glvar->ind_inf_ptr)) < glvar->ind_inf_table)
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_PRV_SET_IND);

			if (c_i == 0)
				res = (s_p->val_ip0 ? ffetch_val(s_p->val_ip0->elem, s_p->val_ip0->attr.t) : 0);
			else
				res = ffetch_vals(s_p->val_ip[c_i-1], c_i, 0);

			push_int(res);

			ffree_index_sets(glvar->ind_inf_ptr);
		}
	}

	/* Legt Elemente einer Teilmatrix auf den Stack (nur fuer Konstanten) */
	long IntFuncFetchVals::ffetch_vals(array_str *array, int mi, int ii)
	{
		long res;
		union ptr_sym_u up;
		bool found;
		ind_inf_one_str *inf_p = glvar->ind_inf_ptr->i_ind + ii;

		if (!array)
			return 0;

		if (inf_p->is_free)
		{
			/* Freier Index: Iteration ueber Durchschnitt des Sets des freien Index und des Definitionssets */
			val_str arrset;
			arrset.t = TP_SET_SYM_ARR;
			arrset.v.array = array;

			res = 0;
			inf_p->iter_set.iter_set2_start(&arrset, true);
			while (!inf_p->iter_set.set_end)
			{
				inf_p->cur_tp = inf_p->iter_set.cur_val.t;
				inf_p->cur_val = inf_p->iter_set.cur_val.v.i;

				up = search_ind_elem(array, inf_p->cur_val, inf_p->cur_tp, &found, (array_ins_info_str *)0);
				if (found)
				{
					if (ii < mi - 1)
						res += ffetch_vals(up.a, mi, ii+1);
					else
						res += ffetch_val(up, array->attr.t);
				}

				inf_p->iter_set.iter_set_next();
			}
		}

		else
		{
			/* Index ist nicht frei: Wenn Element existiert, dann rekursiv zu naechstem Index bzw. bei letztem Index Wert holen */
			up = search_ind_elem(array, inf_p->cur_val, inf_p->cur_tp, &found, (array_ins_info_str *)0);
			if (found)
			{
				if (ii < mi - 1)
					res = ffetch_vals(up.a, mi, ii+1);
				else
					res = ffetch_val(up, array->attr.t);
			}
		}

		return res;
	}

	/* Legt Konstantenwert auf den Stack */
	long IntFuncFetchVals::ffetch_val(union ptr_sym_u up, enum ptrtp tp)
	{
		long res = 0;
		if (tp == PTR_TP_CONST && up.c)
		{
			push_val(&(up.c->c_val));
			res++;
		}
		else if (tp == PTR_TP_VALUE && up.s)
		{
			push_val(up.s);
			res++;
		}

		return res;
	}


	//********************************************************
	// IntFuncFetchTmp
	//********************************************************

	/* Holen Wert Laufkonstante */
	void IntFuncFetchTmp::exec(val_str *vp)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			int i = vp[1].v.i;
			PROTO_OUT_2("  %d", i);
			push_val(&(glvar->tmp_table[i].iter_set.cur_val));
		}
	}


	//********************************************************
	// IntFuncStoreTmp
	//********************************************************

	/* Setzen Wert Laufkonstante */
	void IntFuncStoreTmp::exec(val_str *vp)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			int i = vp[1].v.i;
			loop_ctrl_str *loopp = glvar->tmp_table + i;
			PROTO_OUT_2("  %d", i);

			pop_val(&(loopp->iter_set.cur_val));
			loopp->is_lokal_assign = true;
		}
	}

}

