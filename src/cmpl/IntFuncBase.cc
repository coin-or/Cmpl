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


#include "IntFuncBase.hh"


namespace cmpl
{
	/* Wert auf Stack */
	void IntFuncBase::push_val(val_str *p)
	{
		glvar->value_stack[glvar->stack_ptr] = *p;
#ifdef PROTO
		if(glvar->is_protokoll)
			PROTO_OUT_3("\n    push(%d) : %s", glvar->stack_ptr, write_val_to_temp_buf(p));
#endif
		if(++(glvar->stack_ptr) >= glvar->stack_max)
			glvar->incmem((void **) &(glvar->value_stack), &(glvar->stack_max), MAX_LEN_STACK, sizeof(val_str));
	}

	/* Wert vom Stack */
	void IntFuncBase::pop_val(val_str *p)
	{
		if(glvar->stack_ptr <= 0)
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_STACK_UF);

		*p = glvar->value_stack[--(glvar->stack_ptr)];
#ifdef PROTO
		if(glvar->is_protokoll)
			PROTO_OUT_3("\n    pop(%d) : %s", glvar->stack_ptr, write_val_to_temp_buf(p));
#endif
	}

	/* int auf Stack */
	void IntFuncBase::push_int(long v)
	{
		val_str *p = glvar->value_stack + glvar->stack_ptr;
		p->t = TP_INT;
		p->v.i = v;

		PROTO_OUT_3("\n    push(%d) : %ld", glvar->stack_ptr, v);

		if(++(glvar->stack_ptr) >= glvar->stack_max)
			glvar->incmem((void **) &(glvar->value_stack), &(glvar->stack_max), MAX_LEN_STACK, sizeof(val_str));
	}

	/* double auf Stack */
	void IntFuncBase::push_dbl(double v)
	{
		val_str *p = glvar->value_stack + glvar->stack_ptr;
		p->t = TP_DBL;
		p->v.f = v;

                PROTO_OUT_3("\n    push(%d) : %f", glvar->stack_ptr, v);

		if(++(glvar->stack_ptr) >= glvar->stack_max)
			glvar->incmem((void **) &(glvar->value_stack), &(glvar->stack_max), MAX_LEN_STACK, sizeof(val_str));
	}

	/* Wert als double oder als long auf den Stack */
	void IntFuncBase::push_rund(double v, bool to_lng)
	{
		if (to_lng)
		{
			if (v < LONG_MAX && v > LONG_MIN)
			{
				push_int(RUND(v));
				return;
			}
                        else if ( glvar->args->numOpMode() == NUM_OP_CHECK_ERR)
			{
				/* Bei ganzzahliger Bereichsüberschreitung Fehlermeldung, und dann doch als double */
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INT_MAX_MIN);
			}
		}

		push_dbl(v);
	}

	/* Wert als TP_ISTR auf den Stack */
	void IntFuncBase::push_string(const char *s)
	{
		bool found;
		val_str *p = glvar->value_stack + glvar->stack_ptr;
		p->t = TP_ISTR;
		p->v.i = glvar->strings.search_insert(glvar, s, true, &found);

		PROTO_OUT_3("\n    push(%d) : %s", glvar->stack_ptr, s);

		if(++(glvar->stack_ptr) >= glvar->stack_max)
			glvar->incmem((void **) &(glvar->value_stack), &(glvar->stack_max), MAX_LEN_STACK, sizeof(val_str));
	}

	/* Indizes eines Tupels oder Tupelsets einzeln auf den Stack */
	int IntFuncBase::push_tupel_indices(val_str *p, bool freemem, bool rev)
	{
		if (!TP_IS_TUPEL(p->t) && !TP_IS_TUPELSET(p->t))
		{
			// Kein Tupel: Wert direkt auf den Stack
			push_val(p);
			return 1;
		}

		bool isTupel = TP_IS_TUPEL(p->t);
		int rank = RANK_TP_TUPEL(p->t);
		int cnt = (isTupel || rank == 0 ? rank : p->v.tps->cnt());
		val_str tv;
		val_str *tvp = &tv;

		if (cnt == 1)
		{
			if (isTupel)
				TupelUtil::tupelvalToVal(p->v.i, tvp);
			else if (p->v.tps->isEnum())
			{
				tvp = p;		// p ist reiner enumeration tupelset, kann nicht in Indizes zerlegt werden
				freemem = false;
			}
			else
				tvp = p->v.tps->ind(0);

			push_val(tvp);
		}
		else if (cnt > 1)
		{
			for (int n = (rev ? rank : 1); n <= rank && n >= 1; n += (rev ? -1 : 1))
			{
				if (isTupel)
					TupelUtil::tupelvalToVal(p->v.tp[n], tvp);
				else
				{
                    tvp = p->v.tps->ind(n-1);

					if (TP_IS_TUPELSET(tvp->t))
					{
						// tvp enthaelt TupelsetEnum, muss noch in Tupelset verpackt werden
						tv.v.tps = Tupelset::constructForEnum(glvar, (TupelsetEnum *)(tvp->v.p), false);
						tv.t = tvp->t;
						tvp = &tv;
					}
				}

				push_val(tvp);
			}
		}

		if (freemem && ((isTupel && cnt > 1 && !TupelUtil::globAlloc(p->v.tp)) || (!isTupel && cnt > 0 && !p->v.tps->globAlloc())))
		{
			if (isTupel)
				free(p->v.tp);
			else
				Tupelset::freeTupelset(glvar, p->v.tps);

			p->t = TPP_EMPTY;
		}

		return cnt;
	}

	/* increments the stack pointer (i.e. pushs the last poped value back to the stack) */
	void IntFuncBase::repush()
	{
		PROTO_OUT_2("\n    repush(%d)", glvar->stack_ptr);

		if(++(glvar->stack_ptr) >= glvar->stack_max)
			glvar->incmem((void **) &(glvar->value_stack), &(glvar->stack_max), MAX_LEN_STACK, sizeof(val_str));
	}

	/* int von Stack */
	long IntFuncBase::pop_int()
	{
		long res;
		val_str *p;

		if(glvar->stack_ptr <= 0)
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_STACK_UF);

		/* Wert muss nummerisch sein */
		p = glvar->value_stack + (--(glvar->stack_ptr));
		if (p->t == TP_INT || p->t == TP_DBL)
		{
			res = (p->t == TP_INT ? p->v.i : RUND(p->v.f));
		}
		else
		{
			SPRINT_ERR_2(ERROR_NOT_A_NUMBER, write_val_to_temp_buf(p));
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			res = 1;
		}

		PROTO_OUT_3("\n    pop-int(%d) : %ld", glvar->stack_ptr, res);

		return(res);
	}

	/* pop list count from the stack */
	long IntFuncBase::pop_list_cnt(bool *lstp)
	{
		long res;
		val_str *p;

		if(glvar->stack_ptr <= 0)
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_STACK_UF);

		/* Wert muss Listenzaehler sein */
		p = glvar->value_stack + (--(glvar->stack_ptr));
		if (TP_IS_LIST(p->t))
		{
			res = p->v.i;
		}
		else
		{
			SPRINT_ERR_2("no list count", write_val_to_temp_buf(p));	//TODO
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			res = 0;
		}

		*lstp = (p->t == TPP_LIST_LIST && res > 0);

		PROTO_OUT_4("\n    pop-list_cnt(%d) : %ld %d", glvar->stack_ptr, res, (int)(*lstp));

		return(res);
	}

	/* pop a list from the value stack */
	val_str *IntFuncBase::pop_list(long *cntp, long *lenp)
	{
		// get list count
		bool lst;
		long cnt = pop_list_cnt(&lst);
		*cntp = cnt;

		// pop list elements
		int d;
		return pop_list_elems(cnt, lst, lenp, &d);
	}

	/* pop elements of a list from the value stack */
	val_str *IntFuncBase::pop_list_elems(long cnt, bool lst, long *lenp, int *dp)
	{
		// Wenn die Liste andere Listen enthalten kann, muessen die Werte einzeln geholt werden
		val_str *p;
		if (lst)
		{
			val_str v;
			long len;
			int d, md = 0;
			for ((*lenp) = 0; cnt > 0; cnt--, (*lenp)++)
			{
				if(glvar->stack_ptr <= 0)
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_STACK_UF);

				p = glvar->value_stack + (--(glvar->stack_ptr));

				if (TP_IS_LIST(p->t))
				{
					if (p->v.i > 0)
					{
						p = pop_list_elems(p->v.i, (p->t == TPP_LIST_LIST), &len, &d);
						(*lenp) += len;
						if (d >= md)
							md = d + 1;
					}
				}
			}
			*dp = md;
		}
		else
		{
			if (glvar->stack_ptr < cnt)
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_STACK_UF);
			
			glvar->stack_ptr -= cnt;
			p = glvar->value_stack + glvar->stack_ptr;
			*lenp = cnt;
			*dp = 0;
		}

		return p;
	}

	/* Index von Stack */
	long IntFuncBase::pop_index(enum etp *tp)
	{
		long res;
		val_str *p;

		if(glvar->stack_ptr <= 0)
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL,
					(char *)0, ERROR_INTERN_STACK_UF);

		/* Wert muss nummerisch oder String sein */
		p = glvar->value_stack + (--(glvar->stack_ptr));
		res = test_val_index(p, tp);

		PROTO_OUT_4("\n    pop-index(%d) : %ld (%d)", glvar->stack_ptr, res, (int)*tp);

		return(res);
	}

	/* Sichert das Wert ein Indexwert ist, gibt Indexwert und Typ (immer TP_INT oder TP_ISTR) zurueck */
	long IntFuncBase::test_val_index(val_str *valp, enum etp *tp)
	{
		long res;

		if (valp->t != TP_INT && valp->t != TP_DBL && valp->t != TP_BIN && valp->t != TP_ISTR && valp->t != TP_STR)
		{
			SPRINT_ERR_2(ERROR_NOT_AN_INDEX, write_val_to_temp_buf(valp));
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			res = 1;
			*tp = TP_INT;
		}
		else
		{
			if (valp->t == TP_STR)
			{
				val_str v;
				if (!glvar->string_to_istr(&v, valp->v.s, false))
					free(valp->v.s);
				res = v.v.i;
				*tp = TP_ISTR;
			}
			else
			{
				res = (valp->t == TP_DBL ? RUND(valp->v.f) : valp->v.i);
				*tp = (valp->t == TP_DBL || valp->t == TP_BIN ? TP_INT : valp->t);
			}
		}

		return(res);
	}

	/* Gibt Wert relativ zum obersten Stackelement */
	val_str *IntFuncBase::stack_rel_val(int pos)
	{
            if (pos > glvar->stack_ptr - 1)
                FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL,
                                     (char *)0, ERROR_INTERN_STACK_UF);

            return glvar->value_stack + glvar->stack_ptr - 1 - pos;
	}


	/* Wert als String fuer Ausgabe in temp_buf schreiben */
	char* IntFuncBase::write_val_to_temp_buf(val_str *p)
	{
		if (!p)
		{
			SPRINT_TEMP_1("(null)");
		}
		else
		{
			switch (p->t)
			{
				case TPP_CMD:
					SPRINT_TEMP_2("{cmd:%d}", (int)p->v.c.cmd);
					break;
				case TP_INT:
					SPRINT_TEMP_2("%ld", p->v.i);
					break;
				case TP_DBL:
                    SPRINT_TEMP_2("%f", p->v.f);
					break;
				case TP_STR:
					SPRINT_TEMP_1(p->v.s);
					break;
				case TP_ISTR:
					SPRINT_TEMP_2("\"%s\"", glvar->strings.strings_ind[p->v.i]);
					break;
				case TPP_PTR:
					SPRINT_TEMP_2("%lu (ptr)", (unsigned long)p->v.p);
					break;
				case TPP_ADDR:
					SPRINT_TEMP_2("%ld (addr)", (unsigned int)p->v.a);
					break;
				case TPP_LIST_VAL:
				case TPP_LIST_LIST:
					SPRINT_TEMP_2("list len %ld", p->v.i);
					break;
				case TP_SET_EMPTY:
					SPRINT_TEMP_1("set empty");
					break;
				case TP_SET_HALF_LB:
					SPRINT_TEMP_2("set %ld..", p->v.i);
					break;
				case TP_SET_HALF_UB:
					SPRINT_TEMP_2("set ..%ld", p->v.i);
					break;
				case TP_SET_ALL:
					SPRINT_TEMP_1("set all");
					break;
				case TP_SET_ALL_NUM:
					SPRINT_TEMP_1("set all integers");
					break;
				case TP_SET_ALL_STR:
					SPRINT_TEMP_1("set all strings");
					break;
				case TP_SET_ENUM:
					SPRINT_TEMP_2("set enum len %ld", p->v.set->len);
					break;
				case TP_SET_SYM_ARR:
					SPRINT_TEMP_1("set for array");
					break;
				case TP_SET_ALG:
					SPRINT_TEMP_2("set alg len %ld", p->v.set->len);
					break;
				case TP_SET_ALG0_1:
					SPRINT_TEMP_2("set 0..%ld", p->v.i);
					break;
				case TP_SET_ALG1_1:
					SPRINT_TEMP_2("set 1..%ld", p->v.i);
					break;
				case TPK_PROBLEM_MAIN:
					SPRINT_TEMP_2("problem '%s'", ((CMPLProblemMain*)(p->v.subsyms))->filename);
					break;
				case TPK_PROBLEM_SOL:
				case TPK_PROBLEM_VC:
				case TPK_PROBLEM_RES:
					SPRINT_TEMP_1("(cmpl problem result values)");
					break;
				default:
					if (TP_IS_TUPEL(p->t)) {
						SPRINT_TEMP_2("tupel rank %d", RANK_TP_TUPEL(p->t));
					}
					else if (TP_IS_TUPELSET(p->t)) {
						SPRINT_TEMP_2("tupelset rank %d", RANK_TP_TUPEL(p->t));
					}
					else {
						SPRINT_TEMP_1("(empty)");
					}
					break;
			}
		}

		return glvar->temp_buf;
	}


	/* Erstellt set aus Werten vom Stack; tp bestimmt, wie die Werte auf dem Stack erwartet werden */
	void IntFuncBase::construct_set(enum etp tp, val_str *set)
	{
		set_ea_str *set_ea_p;
		long start_val, step_val, end_val;

		if (SET_IS_INFINITE(tp) || SET_IS_EMPTY(tp) || tp == TP_SET_ALG0_1 || tp == TP_SET_ALG1_1)
		{
			/* Sets ohne oder mit nur einem int-Parameter */
			set->t = tp;
			if (SET_USE_VALINT(tp))
				set->v.i = pop_int();
		}
		else if (tp == TP_SET_ALG || tp == TP_SET_ALG_S1)
		{
			end_val = pop_int();
			step_val = (tp == TP_SET_ALG_S1 ? 1 : pop_int());
			start_val = pop_int();
			tp = TP_SET_ALG;
		}
		else if (tp == TP_SET_ENUM)
		{
			int len = pop_int();
			long d1;
			int d2;

			val_str *arr = pop_list_elems(len, false, &d1, &d2);
			SetUtil::constructSetEnum(glvar, arr, len, false, set);

/*
			int ind, ind2;
			long elem, elem1, elem2, min;
			enum etp elemtp, elemtp1, elemtp2, mintp;
			val_str *vp, *vp2;

			if (len == 0)
			{
				set->t = TP_SET_EMPTY;
			}
			else
			{
				// Erstmal beide oberste Elemente testen, ob algorithmischer Set in Frage kommt
				elemtp = TPP_EMPTY;
				elem1 = pop_index(&elemtp1);
				if (len == 1)
					elemtp2 = TP_INT;
				else
					elem2 = pop_index(&elemtp2);

				ind = len - 2;
				if (elemtp1 == TP_INT && elemtp2 == TP_INT)
				{
					end_val = elem1;
					start_val = (len == 1 ? elem1 : elem2);
					step_val = (len == 1 ? 1 : (end_val - start_val));

					// Uebrige Elemente pruefen, ob in den algorithmischen Set passend
					for (; ind > 0; ind--)
					{
						start_val -= step_val;
						elem = pop_index(&elemtp);
						if (elemtp != TP_INT || elem != start_val)
							break;
					}

					if (ind <= 0)
						tp = TP_SET_ALG;
				}

				if (tp == TP_SET_ENUM)
				{
					// Speicher fuer Set direkt reservieren statt ueber c_getmem, da (erstmal) nur temporaer verwendet
					set_ea_p = (set_ea_str *) calloc(1, sizeof(set_ea_str) + len*(sizeof(val_str) + sizeof(int)));
					set_ea_p->is_enum = true;
					set_ea_p->is_gl_mem = false;
					set_ea_p->len = len;

					set_ea_p->u.e.array = (val_str *)(set_ea_p + 1);
					set_ea_p->u.e.index = (int *)(set_ea_p->u.e.array + len);

					set->t = TP_SET_ENUM;
					set->v.set = set_ea_p;

					// Schon beim Test auf algorithmischen Set gelesene Werte eintragen
					vp = set_ea_p->u.e.array + len - 1;
					vp->t = elemtp1;
					vp->v.i = elem1;

					if (len > 1)
					{
						vp--;
						vp->t = elemtp2;
						vp->v.i = elem2;

						if (elemtp != TPP_EMPTY)
						{
							// Werte entsprechend Teil, der fuer algorithmischen Set passt
							start_val = elem2 - step_val;
							for (ind2 = len - 2; ind2 > ind; ind2--, start_val -= step_val)
							{
								vp--;
								vp->t = TP_INT;
								vp->v.i = start_val;
							}

							// Ersten unpassenden Wert
							vp--;
							vp->t = elemtp;
							vp->v.i = elem;
							ind--;
						}

						// Restliche Elemente noch vom Stack holen
						for (; ind > 0; ind--)
						{
							elem = pop_index(&elemtp);
							vp--;
							vp->t = elemtp;
							vp->v.i = elem;
						}
					}

					// Kanonische Ordnung der Elemente feststellen
					set_ea_p->u.e.index[0] = 0;
					set_ea_p->u.e.string_start = (set_ea_p->u.e.array[0].t == TP_ISTR ? 0 : 1);

					for (ind = 1, vp = set_ea_p->u.e.array + 1; ind < len; ind++, vp++)
					{
						vp2 = set_ea_p->u.e.array + set_ea_p->u.e.index[ind-1];
						if ((vp->t == TP_ISTR && vp2->t == TP_INT) ||
								(vp->t == TP_INT && vp2->t == TP_INT && vp->v.i >= vp2->v.i) ||
								(vp->t == TP_ISTR && vp2->t == TP_ISTR && strcmp(glvar->strings.strings_ind[vp->v.i], glvar->strings.strings_ind[vp2->v.i]) >= 0))
						{
							// Element passt in der Reihenfolge
							set_ea_p->u.e.index[ind] = ind;
							if (vp->t == TP_INT)
								set_ea_p->u.e.string_start = ind + 1;
						}
						else
						{
							// Passende Einfuegeposition suchen
							for (ind2 = ind - 1; ind2 > 0; ind2--)
							{
								vp2 = set_ea_p->u.e.array + set_ea_p->u.e.index[ind2-1];
								if ((vp->t == TP_ISTR && vp2->t == TP_INT) ||
										(vp->t == TP_INT && vp2->t == TP_INT && vp->v.i >= vp2->v.i) ||
										(vp->t == TP_ISTR && vp2->t == TP_ISTR && strcmp(glvar->strings.strings_ind[vp->v.i], glvar->strings.strings_ind[vp2->v.i]) >= 0))
									break;
							}
							memmove(set_ea_p->u.e.index + ind2 + 1, set_ea_p->u.e.index + ind2, sizeof(int) * (ind - ind2));

							set_ea_p->u.e.index[ind2] = ind;
							if (vp->t == TP_INT)
								set_ea_p->u.e.string_start++;
						}
					}
				}
			}
*/
		}
		else
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
			set->t = TP_SET_EMPTY;	/* Ersatzweise leerer Set */
		}

		/* Fuer algorithmischen Set bisher nur Parameter bestimmt, nun noch anlegen */
		if (tp == TP_SET_ALG)
		{
			/* Auf spezielle algorithmische Sets testen */
			if (step_val == 1 && (start_val == 0 || start_val == 1))
			{
				set->t = (start_val == 0 ? TP_SET_ALG0_1 : TP_SET_ALG1_1);
				set->v.i = end_val;
			}
			else
			{
				/* Speicher fuer Set direkt reservieren statt ueber c_getmem, da (erstmal) nur temporaer verwendet */
				set_ea_p = (set_ea_str *) calloc(1, sizeof(set_ea_str));
				set_ea_p->is_enum = false;
				set_ea_p->is_gl_mem = false;

				set_ea_p->u.a.end_val = end_val;
				set_ea_p->u.a.step_val = step_val;
				set_ea_p->u.a.start_val = start_val;

				if (set_ea_p->u.a.step_val == 0)
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_SET_STEP_0);
					set_ea_p->u.a.step_val = 1;		/* Ersatzweise Schrittweite 1 */
				}

				set_ea_p->len = (set_ea_p->u.a.end_val - set_ea_p->u.a.start_val) / set_ea_p->u.a.step_val + 1;

				set->t = TP_SET_ALG;
				set->v.set = set_ea_p;
			}
		}
	}

	/* Gibt den Definitionsset eines Arrays, als Durchschnittsmenge zum angegebenen Iterationsset */
	val_str IntFuncBase::defset_array(bool no_array_set, array_str *array, SetIterator *iter_set)
	{
		val_str arrset, defset;
		long len;

		if (array->defset.t != TPP_EMPTY)
		{
			arrset = array->defset;
		}
		else
		{
			arrset.t = TP_SET_SYM_ARR;
			arrset.v.array = array;
		}

		/* Wenn expliziter Definitionsset oder Array als Set erlaubt, und angegebener Iterationsset TP_SET_ALL, dann direkt zurueck */
		if ((!no_array_set || arrset.t != TP_SET_SYM_ARR) && SET_IS_INFINITE(iter_set->set.t))
		{
			if (iter_set->set.t == TP_SET_ALL ||
					(iter_set->set.t == TP_SET_ALL_NUM && array->a_str.cnt == 0) ||
					(iter_set->set.t == TP_SET_ALL_STR && array->a_dir.cnt == 0 && array->a_num.cnt == 0))
				return arrset;
		}

		/* Iteration ausfuehren, und alle Elemente des Sets auf den Stack */
		len = 0;
		iter_set->iter_set2_start(&arrset, true);
		
		while (!iter_set->set_end)
		{
			push_val(&(iter_set->cur_val));
			len++;
			iter_set->iter_set_next();
		}

		/* Set aus den Werten auf dem Stack erstellen */
		push_int(len);
		construct_set(TP_SET_ENUM, &defset);

		/* Wenn bisher kein expliziter Definitionsset, und angegebener Iterationsset TP_SET_ALL, dann als expliziter Definitionsset eintragen */
		if (array->defset.t == TPP_EMPTY && iter_set->set.t == TP_SET_ALL)
		{
			if (SET_USE_POINTER(defset.t) && !defset.v.set->is_gl_mem)
			{
				/* Reservierten Speicher fuer den Set jetzt in Speicherverwaltung aufnehmen */
				glvar->ins_mem((void *)(defset.v.set));
				defset.v.set->is_gl_mem = true;
			}
			array->defset = defset;
		}

		return defset;
	}


	/* Suche Element in Matrix. glvar->ind_inf_ptr muss mit den Indexinformationen gefuellt sein */
	union ptr_sym_u IntFuncBase::ffetch_p(symbol_str *s_p, array_str *array, int mi, bool *found, enum ptrtp *ptp)
	{
		ind_inf_one_str *inf_p;
		union ptr_sym_u up;

		if (mi == 0)
		{
			if (glvar->ind_inf_ptr->i_anz_ind == 0)
			{
				/* Element ohne Indizes */
				if (s_p->val_ip0)
				{
					*found = true;
					*ptp = s_p->val_ip0->attr.t;
					up = s_p->val_ip0->elem;
				}
				else
					*found = false;

				return up;
			}

			/* Start Rekursion */
			array = s_p->val_ip[glvar->ind_inf_ptr->i_anz_ind - 1];
			if (!array)
			{
				*found = false;
				return up;
			}
		}

		*ptp = array->attr.t;

		/* Element zu Index mi suchen */
		inf_p = glvar->ind_inf_ptr->i_ind + mi;
		up = search_ind_elem(array, inf_p->cur_val, inf_p->cur_tp, found, (array_ins_info_str *)0);
		if (!(*found))
			return up;

		/* Rekursiv fuer naechsten Index */
		if (mi < glvar->ind_inf_ptr->i_anz_ind - 1)
			up = ffetch_p(s_p, up.a, mi+1, found, ptp);

		return up;
	}

	/* Fuegt ein indiziertes Element ein (ind_tp ist Typ des Index und kann nur TP_INT oder TP_ISTR sein) */
	union ptr_sym_u IntFuncBase::insert_ind_elem(array_str *arr_p, long ind, enum etp ind_tp, SetIterator *iter_p, array_ins_info_str *ins_info)
	{
		struct a_str *arr;
		union ptr_sym_u ret;
		void *vp, *vp_dst;
		bool found, setg;
		t_addr cap_inc, old_cap, cap2, pos_ins, setl, n;
		size_t size;
		int setm, ind2, i;

		PROTO_OUT_3("\n  insert_ind_elem %ld (%d)", ind, (int)ind_tp);

		/* Test ob Einfuegen in direkt indiziertes Array moeglich */
		if (ind_tp == TP_INT && ind >= 0)
		{
			/* Wenn iter_p, dann Array auch in Hinsicht auf andere Set-Elemente pruefen, da dafuer auch noch Einfuegen folgt */
			setm = setl = 0;
			setg = false;
			if (iter_p && (iter_p->set.t == TP_SET_ALG || iter_p->set.t == TP_SET_ALG0_1 || iter_p->set.t == TP_SET_ALG1_1))
			{
				if (iter_p->set.t == TP_SET_ALG)
				{
					set_ea_str *set_ea = iter_p->set.v.set;
					setl = set_ea->len;
					setm = (set_ea->u.a.step_val < 0 ? set_ea->u.a.start_val : (set_ea->u.a.start_val + set_ea->u.a.step_val * (setl - 1)));
				}
				else
				{
					setm = iter_p->set.v.i;
					setl = (TP_SET_ALG1_1 ? setm : (setm + 1));
				}
				setg = (setm > 0 && setm >= arr_p->a_dir.cap && ((arr_p->a_dir.cap == 0 && setm < MIN_CAP_ARRAY) || ((arr_p->a_dir.cnt+setl) / (double)setm >= MIN_ARRAY_DENSITY)));
			}
			
			/* Wenn nicht in derzeitiges Array passend, aber in erweitertes Array, dann erweitern */
			if ((ind >= arr_p->a_dir.cap && ((arr_p->a_dir.cap == 0 && ind < MIN_CAP_ARRAY) || ((arr_p->a_dir.cnt+1.0) / ind >= MIN_ARRAY_DENSITY))) || setg)
			{
				old_cap = arr_p->a_dir.cap;
				cap_inc = (MIN_CAP_ARRAY >= MIN_CAP_GROW * old_cap ? MIN_CAP_ARRAY : (int)(MIN_CAP_GROW * old_cap));
				if ((setg ? setm : ind) >= old_cap + cap_inc)
					cap_inc = (setg ? setm : ind) + 1 - old_cap;

				PROTO_OUT_3("\n    grow direct array from %d by %d", old_cap, cap_inc);

				vp = ptr_symbol_void(&(arr_p->a_dir.array), arr_p->attr.t, 0, &size);
				if (old_cap == 0)
					vp = (void *)0;

				glvar->incmem(&vp, &(arr_p->a_dir.cap), cap_inc, size);
				set_ptr_sym_u(&(arr_p->a_dir.array), arr_p->attr.t, vp);

				/* Elemente im neuen Bereich als leer kennzeichnen */
				for (n = old_cap; n < arr_p->a_dir.cap; n++)
					set_ind_elem_empty(&(arr_p->a_dir.array), arr_p->attr.t, n);

				/* Pruefen, ob indirekt nummerisch indizierte Elemente nun in direkt indizierte zu uebernehmen sind */
				if (arr_p->a_num.len > 0 && arr_p->a_num.index[0] < arr_p->a_dir.cap && arr_p->a_num.index[arr_p->a_num.len-1] >= old_cap)
				{
					/* Indirekt nummerisch indizierte Elemente durchgehen und wenn passenden in direkt indizierte uebernehmen */
					pos_ins = search_int_sort(arr_p->a_num.index, arr_p->a_num.len, old_cap);
					for (i = pos_ins; i < arr_p->a_num.len; i++)
					{
						ind2 = arr_p->a_num.index[i];
						if (ind2 >= arr_p->a_dir.cap)
							break;

						PROTO_OUT_2("\n    move index %d to direct array", ind2);

						vp = ptr_symbol_void(&(arr_p->a_num.array), arr_p->attr.t, i, &size);
						vp_dst = ptr_symbol_void(&(arr_p->a_dir.array), arr_p->attr.t, ind2, &size);
						memcpy(vp_dst, vp, size);

						arr_p->a_num.cnt--;
						set_ind_elem_empty(&(arr_p->a_num.array), arr_p->attr.t, i);

						arr_p->a_dir.cnt++;
						if (ind2 >= arr_p->a_dir.len)
							arr_p->a_dir.len = ind2 + 1;
					}

					/* Gegebenenfalls Elemente mit groesseren Indizes in den freigewordenen Bereich verschieben */
					if (i > pos_ins)
					{
						if (i < arr_p->a_num.len)
						{
							vp = arr_p->a_num.index + i;
							vp_dst = arr_p->a_num.index + pos_ins;
							memmove(vp_dst, vp, sizeof(int));

							vp = ptr_symbol_void(&(arr_p->a_num.array), arr_p->attr.t, i, &size);
							vp_dst = ptr_symbol_void(&(arr_p->a_num.array), arr_p->attr.t, pos_ins, &size);
							memmove(vp_dst, vp, size * (i - pos_ins));

							arr_p->a_num.len -= (i - pos_ins);
						}
						else
						{
							arr_p->a_num.len = pos_ins;
						}
					}
				}
			}

			/* Wenn nun in Array passend, dann wird hier eingefuegt */
			if (ind < arr_p->a_dir.cap)
			{
				ret = ptr_symbol_elem(&(arr_p->a_dir.array), arr_p->attr.t, ind, &found);
				if (ind >= arr_p->a_dir.len) {
					arr_p->a_dir.len = ind + 1;
					found = false;
				}
				if (!found)
					arr_p->a_dir.cnt++;

				PROTO_OUT_2("\n   insert to direct array (cnt %d)", (int)(arr_p->a_dir.cnt));
				return ret;
			}
		}

		/* In indirekt indiziertes Array einzufuegen */
		arr = (ind_tp == TP_INT ? &(arr_p->a_num) : &(arr_p->a_str));

		/* Wenn Einfuegeposition noch unbekannt dann bestimmen */
		pos_ins = (ins_info ? (ind_tp == TP_INT ? ins_info->ins_ind_num : ins_info->ins_ind_str) : -1);
		if (pos_ins == -1)
			pos_ins = search_int_sort(arr->index, arr->len, (int)ind);
		found = (pos_ins < arr->len && arr->index[pos_ins] == ind);

		PROTO_OUT_3("\n   insert to indirect array (%d: %d)", (int)pos_ins, (int)found);

		/* Wenn Element schon da, zurueck */
		if (found)
		{
			ret = ptr_symbol_elem(&(arr->array), arr_p->attr.t, pos_ins, &found);
			if (pos_ins >= arr->len)
				arr->len = pos_ins + 1;
		}
		else
		{
			/* Wenn Arrays voll, dann Kapazitaet erweitern */
			if (arr->len == arr->cap)
			{
				old_cap = arr->cap;
				cap_inc = (MIN_CAP_ARRAY >= MIN_CAP_GROW * old_cap ? MIN_CAP_ARRAY : (int)(MIN_CAP_GROW * old_cap));

				vp = ptr_symbol_void(&(arr->array), arr_p->attr.t, 0, &size);
				if (old_cap == 0)
					vp = (void *)0;

				glvar->incmem(&vp, &(arr->cap), cap_inc, size);
				set_ptr_sym_u(&(arr->array), arr_p->attr.t, vp);

				cap2 = old_cap;
				vp = (void *)arr->index;
                if (old_cap == 0)
                    vp = (void *)0;

				glvar->incmem(&vp, &cap2, cap_inc, sizeof(int));
				arr->index = (int *)vp;

				/* Elemente im neuen Bereich als leer kennzeichnen */
				for (n = old_cap; n < arr->cap; n++)
					set_ind_elem_empty(&(arr->array), arr_p->attr.t, n);
			}

			/* Wenn nicht anzufuegen, dann Elemente nach der Einfuegeposition verschieben */
			if (pos_ins < arr->len)
			{
				vp = arr->index + pos_ins;
				vp_dst = arr->index + pos_ins + 1;
				memmove(vp_dst, vp, sizeof(int) * (arr->len - pos_ins));

				vp = ptr_symbol_void(&(arr->array), arr_p->attr.t, pos_ins, &size);
				vp_dst = ptr_symbol_void(&(arr->array), arr_p->attr.t, pos_ins+1, &size);
				memmove(vp_dst, vp, size * (arr->len - pos_ins));

				set_ind_elem_empty(&(arr->array), arr_p->attr.t, pos_ins);
			}

			/* Index einfuegen (Element selbst muss nicht eingefuegt werden, sondern wird nachher mit den neuen Werten ueberschrieben) */
			ret = ptr_symbol_elem(&(arr->array), arr_p->attr.t, pos_ins, &found);
			arr->index[pos_ins] = (int)ind;
			arr->len++;
			arr->cnt++;
		}

		return ret;
	}

	/* Sucht einen int-Wert val in einem aufsteigend geordneten Array. Zurueck Index des gefunden Wertes oder der Stelle wo er einzufuegen waere */
	t_addr IntFuncBase::search_int_sort(int *index_arr, int len, int val)
	{
		int ob, ub, m;

		/* Erstmal die Raender testen */
		if (len == 0 || index_arr[0] >= val)
			return 0;
		if (index_arr[len-1] <= val)
			return (index_arr[len-1] == val ? (len-1) : len);

		/* Intervallsuche, von der Mitte beginnend */
		ob = len - 2;
		ub = 1;
		while (ub < ob)
		{
			m = (ob-ub)/2 + ub;
			if (index_arr[m] == val)
				return m;

			if (index_arr[m] < val)
				ub = m + 1;
			else
				ob = m - 1;
		}

		if (index_arr[ub] == val || ub > ob)
			return ub;

		return (index_arr[ub] > val ? ub : (ub+1));
	}

	/* Sucht einen int-Wert val in einem Array aus val_str, deren aufsteigende Ordnung in einem Indexarray gegeben ist. Zurueck Index des gefunden Wertes oder -1 */
	int IntFuncBase::search_int_sort_val_array(val_str *val_arr, int *index_arr, int len, long val, bool sorted)
	{
		int ob, ub, m;

		if (sorted)
		{
			/* Erstmal die Raender testen */
			if (len == 0 || val_arr[index_arr[0]].v.i >= val || val_arr[index_arr[len-1]].v.i <= val)
			{
				if (len > 0 && val_arr[index_arr[0]].v.i == val)
					return index_arr[0];
				else if (len > 0 && val_arr[index_arr[len-1]].v.i == val)
					return index_arr[len-1];
				else
					return -1;
			}

			/* Intervallsuche, von der Mitte beginnend */
			ob = len - 2;
			ub = 1;
			while (ub < ob)
			{
				m = (ob-ub)/2 + ub;
				if (val_arr[index_arr[m]].v.i == val)
					return index_arr[m];

				if (val_arr[index_arr[m]].v.i < val)
					ub = m + 1;
				else
					ob = m - 1;
			}

			if (val_arr[index_arr[ub]].v.i == val)
				return index_arr[ub];
		}
		else
		{
			/* Sortierung in index_arr ist keine aufsteigende int-Sortierung, deshalb muessen alle Werte getestet werden */
			for (m = 0; m < len; m++)
			{
				if (val_arr[index_arr[m]].v.i == val)
					return index_arr[m];
			}
		}

		return -1;
	}

	/* Gibt passenden Pointer zurueck fuer Arrayelement eines bestimmten Pointertyps */
	union ptr_sym_u IntFuncBase::ptr_symbol_elem(union ptr_sym_u *ptr, enum ptrtp tp, long ind, bool *found)
	{
		union ptr_sym_u ret;

		switch (tp) {
			case PTR_TP_CONST:
				ret.c = ptr->c + ind;
				if (found)
					*found = (ret.c->c_val.t != TPP_EMPTY);
				break;

			case PTR_TP_VALUE:
				ret.s = ptr->s + ind;
				if (found)
					*found = (ret.s->t != TPP_EMPTY);
				break;

			case PTR_TP_VAR:
				ret.v = ptr->v + ind;
				if (found)
					*found = (ret.v->v_type != TPP_EMPTY);
				break;

			case PTR_TP_ARRAY:
				ret.a = ptr->a + ind;
				if (found)
					*found = (ret.a->attr.t != PTR_TP_EMPTY);
				break;
		}

		return ret;
	}

	/* Kennzeichnet ein Arrayelement eines bestimmten Symboltyps als unbenutzt */
	void IntFuncBase::set_ind_elem_empty(union ptr_sym_u *ptr, enum ptrtp tp, long ind)
	{
		switch (tp) {
			case PTR_TP_CONST:
				ptr->c[ind].tp = TPP_EMPTY;
				ptr->c[ind].c_val.t = TPP_EMPTY;
				break;

			case PTR_TP_VALUE:
				ptr->s[ind].t = TPP_EMPTY;
				break;

			case PTR_TP_VAR:
				ptr->v[ind].v_type = TPP_EMPTY;
				break;

			case PTR_TP_ARRAY:
				ptr->a[ind].attr.t = PTR_TP_EMPTY;
				break;
		}
	}

	/* Gibt Pointer auf Arrayelement eines bestimmten Symboltyps als void-Pointer zurueck */
	void* IntFuncBase::ptr_symbol_void(union ptr_sym_u *ptr, enum ptrtp tp, long ind, size_t *size_ret)
	{
		void *ret;

		switch (tp) {
			case PTR_TP_CONST:
				ret = (void *) (ptr->c + ind);
				*size_ret = sizeof(const_str);
				break;

			case PTR_TP_VALUE:
				ret = (void *) (ptr->s + ind);
				*size_ret = sizeof(val_str);
				break;

			case PTR_TP_VAR:
				ret = (void *) (ptr->v + ind);
				*size_ret = sizeof(var_str);
				break;

			case PTR_TP_ARRAY:
				ret = (void *) (ptr->a + ind);
				*size_ret = sizeof(array_str);
				break;
		}

		return ret;
	}

	/* Setzt einen union ptr_sym_u fuer einen bestimmten Symboltyp aus einem void-Pointer */
	void IntFuncBase::set_ptr_sym_u(union ptr_sym_u *ptr, enum ptrtp tp, void *vp)
	{
		switch (tp) {
			case PTR_TP_CONST:
				ptr->c = (const_str *) vp;
				break;

			case PTR_TP_VALUE:
				ptr->s = (val_str *) vp;
				break;

			case PTR_TP_VAR:
				ptr->v = (var_str *) vp;
				break;

			case PTR_TP_ARRAY:
				ptr->a = (array_str *) vp;
				break;
		}
	}

	/* Suche nach indiziertem Element (ind_tp ist Typ des Index und kann nur TP_INT oder TP_ISTR sein) */
	union ptr_sym_u IntFuncBase::search_ind_elem(array_str *arr_p, long ind, enum etp ind_tp, bool *found, array_ins_info_str *ret_ins_p)
	{
		struct a_str *arr;
		union ptr_sym_u ret;
		long i;

		*found = false;

		/* Nur wenn Index TP_INT ist, kann er direkt indiziert sein */
		if (ind_tp == TP_INT && ind >= 0)
		{
			if (ind < arr_p->a_dir.len)
			{
				/* Auch wenn der Index im direkt indizierten Bereich liegt, ist das Element vielleicht nicht enthalten */
				ret = ptr_symbol_elem(&(arr_p->a_dir.array), arr_p->attr.t, ind, found);
			}

			/* Noetigenfalls Info fuer Einfuegen fuellen */
			if (ret_ins_p)
				ret_ins_p->ins_ind_num = ret_ins_p->ins_ind_str = -1;

			/* Wenn gefunden, dann keine weitere Suche notwendig */
			if (*found)
				return ret;
		}
		
		/* Zu durchsuchendes Array haengt vom Indextyp ab */
		arr = (ind_tp == TP_INT ? &(arr_p->a_num) : &(arr_p->a_str));

		/* Suche nach dem Index */
		i = search_int_sort(arr->index, arr->len, (int)ind);
		*found = (i < arr->len && arr->index[i] == ind);

		if (*found)
			ret = ptr_symbol_elem(&(arr->array), arr_p->attr.t, i, found);

		/* Noetigenfalls Info fuer Einfuegen fuellen */
		if (ret_ins_p)
		{
			ret_ins_p->ins_ind_num = (ind_tp == TP_INT ? i : -1);
			ret_ins_p->ins_ind_str = (ind_tp != TP_INT ? i : -1);
		}

		return ret;
	}

	/* Legt Elemente einer Teilmatrix auf den Stack (nur fuer Konstanten), oder zaehlt sie nur: wenn die Teilmatrix durch einen Tupelset angegeben ist */
	long IntFuncBase::ffetch_cnt_vals_ts(symbol_str *s_p, int c_i, bool cnt)
	{
		SetIterator *iterp = &(glvar->ind_inf_ptr->i_ind->iter_set);
		long res = 0;

		/* Tupel wird auf den naechsten Platz fuer ein Indexinfo gelegt */
		(glvar->ind_inf_ptr)++;
		if (glvar->ind_inf_ptr >= glvar->ind_inf_table + MAX_IND_INF)
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_REK_SET_IND_MAX);

		ind_inf_str *iip = glvar->ind_inf_ptr;
		ind_inf_one_str *iop;
		tupel_t tup;
		int i;

		union ptr_sym_u up;
		bool found;
		enum ptrtp pt;

		iip->i_anz_ind = c_i;
		iip->i_free_ind = 0;
		for (i=0, iop=iip->i_ind; i<c_i; i++, iop++)
			iop->is_free = false;

		/* Iteration ueber das Tupelset */
		iterp->iter_set_start(true);
		while (!iterp->set_end)
		{
			if (TP_IS_TUPEL(iterp->cur_val.t) && RANK_TP_TUPEL(iterp->cur_val.t) == c_i)
			{
				/* Indexinfo mit Werten aus dem Tupel fuellen */
				if (c_i == 1)
				{
					iop = iip->i_ind;
					iop->cur_val = TupelUtil::tupelvalToIntTp(iterp->cur_val.v.i, &(iop->cur_tp));
				}
				else if (c_i > 1)
				{
					tup = iterp->cur_val.v.tp;
					for (i=0, iop=iip->i_ind; i<c_i; i++, iop++)
						iop->cur_val = TupelUtil::tupelvalToIntTp(tup[i+1], &(iop->cur_tp));
				}

				/* Wert zu dem Tupel holen */
				up = ffetch_p(s_p, (array_str *)0, 0, &found, &pt);
				if (found &&
						((pt == PTR_TP_CONST && up.c && up.c->c_val.t != TPP_EMPTY) ||
						 (pt == PTR_TP_VALUE && up.s && up.s->t != TPP_EMPTY) ||
						 (pt == PTR_TP_VAR && cnt)))
				{
					res++;
					if (!cnt)
					{
						if (pt == PTR_TP_CONST)
							push_val(&(up.c->c_val));
						else
							push_val(up.s);
					}
				}
			}

			iterp->iter_set_next();
		}

		/* Indexinfopointer wieder zurueck */
		(glvar->ind_inf_ptr)--;

		return res;
	}

	/* Konvertiert Wert in vs in Typ tp und gibt den konvertierten Wert in vd zurueck */
	void IntFuncBase::fconv_to_etp(val_str *vs, val_str *vd, enum etp tp, char *format, bool expl_conv)
	{
		bool f;

		if (vs->t == tp && !format)
		{
			*vd = *vs;
			return;
		}
		else if (TYPECLASS(tp) == TPK_SET)
		{
			if (TYPECLASS(vs->t) == TPK_SET)
			{
				*vd = *vs;
				return;
			}
			else if (vs->t == TPP_NULL)
			{
				vd->t = TP_SET_EMPTY;
				return;
			}
			else if (TYPECLASS(vs->t) == TPK_VALUE)
			{
				push_val(vs);
				push_int(1);
				construct_set(TP_SET_ENUM, vd);
				return;
			}
		}
		else
		{
			switch(tp)
			{
				case TP_BIN:
					vd->v.i = (fconv_to_bin(vs, expl_conv) ? 1 : 0);
					vd->t = TP_BIN;
					return;

				case TP_INT:
					vd->v.i = fconv_to_int(vs, expl_conv, false);
					vd->t = TP_INT;
					return;

				case TP_DBL:
					vd->v.f = fconv_to_dbl(vs, expl_conv);
					vd->t = TP_DBL;
					return;

				case TP_STR:
					vd->v.s = fconv_to_str(vs, format, expl_conv, &f);
					vd->t = TP_STR;
					return;

				case TP_ISTR:
					{
						char *str = fconv_to_str(vs, format, expl_conv, &f);
						glvar->string_to_istr(vd, str, true);
					}
					vd->t = TP_ISTR;
					return;

				case TPU_NUM:	/* double beibehalten, sonst implizite Umwandlung in int */
					if (vs->t == TP_INT || vs->t == TP_DBL)
					{
						*vd = *vs;
					}
					else
					{
						vd->v.i = fconv_to_int(vs, false, true);
						vd->t = TP_INT;
					}
					return;
					
				case TPK_PROBLEM_MAIN:
					vd->v.subsyms = fconv_to_problem(vs, expl_conv);
					vd->t = TPK_PROBLEM_MAIN;
					return;

			}
		}

		/* Ungueltige Konvertierung; ersatzweise nicht konvertieren */	
		SPRINT_ERR_3(ERROR_CONV_EXPL, FILEIO->output_typename(vs->t), FILEIO->output_typename(tp));
		FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
		*vd = *vs;
	}

	/* Konvertiert Wert in double */
	double IntFuncBase::fconv_to_dbl(val_str *vs, bool expl_conv)
	{
		if (!expl_conv && (vs->t == TP_STR || vs->t == TP_ISTR))
		{
			/* Ungueltige implizite Konvertierung; ersatzweise 0 zurueck */
			SPRINT_ERR_3(ERROR_CONV_IMPL, FILEIO->output_typename(vs->t), FILEIO->output_typename(TP_DBL));
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
		}
		else
		{
			switch (vs->t)
			{
				case TP_STR:
					return atof(vs->v.s);

				case TP_ISTR:
					return atof(glvar->strings.strings_ind[vs->v.i]);

				case TP_INT:
				case TP_BIN:
					return (double)(vs->v.i);

				case TP_DBL:
					return vs->v.f;

				case TPP_NULL:
					return 0.0;

				default:
					/* Ungueltige Konvertierung; ersatzweise 0 zurueck */
					SPRINT_ERR_3(ERROR_CONV_EXPL, FILEIO->output_typename(vs->t), FILEIO->output_typename(TP_DBL));
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			}
		}

		return 0.;
	}

	/* Konvertiert Wert in int */
	long IntFuncBase::fconv_to_int(val_str *vs, bool expl_conv, bool conv_num)
	{
		if (!expl_conv && (vs->t == TP_STR || vs->t == TP_ISTR || (vs->t == TP_DBL && (double)((int)(vs->v.f)) != vs->v.f)))
		{
			/* Ungueltige implizite Konvertierung; ersatzweise 0 zurueck */
			if (conv_num) {
				SPRINT_ERR_2(ERROR_NOT_A_NUMBER, write_val_to_temp_buf(vs)); 
			}
			else {
				SPRINT_ERR_3(ERROR_CONV_IMPL, FILEIO->output_typename(vs->t), FILEIO->output_typename(TP_INT));
			}
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
		}
		else
		{
			switch (vs->t)
			{
				case TP_STR:
					return atol(vs->v.s);

				case TP_ISTR:
					return atol(glvar->strings.strings_ind[vs->v.i]);

				case TP_INT:
				case TP_BIN:
					return vs->v.i;

				case TP_DBL:
					return RUND(vs->v.f);

				case TPP_NULL:
					return 0;

				default:
					/* Ungueltige Konvertierung; ersatzweise 0 zurueck */
					if (conv_num) {
						SPRINT_ERR_2(ERROR_NOT_A_NUMBER, write_val_to_temp_buf(vs));
					}
					else {
						SPRINT_ERR_3(ERROR_CONV_EXPL, FILEIO->output_typename(vs->t), FILEIO->output_typename(TP_INT));
					}
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			}
		}

		return 0;
	}

	/* Konvertiert Wert in binary */
	bool IntFuncBase::fconv_to_bin(val_str *vs, bool expl_conv)
	{
		if (!expl_conv && (vs->t == TP_STR || vs->t == TP_ISTR || vs->t == TPK_SET ||
					(vs->t == TP_DBL && vs->v.f != 0. && vs->v.f != 1.) ||
					(vs->t == TP_INT && vs->v.i != 0 && vs->v.i != 1)))
		{
			/* Ungueltige implizite Konvertierung; ersatzweise false zurueck */
			SPRINT_ERR_3(ERROR_CONV_IMPL, FILEIO->output_typename(vs->t), FILEIO->output_typename(TP_BIN));
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
		}
		else
		{
			if (TYPECLASS(vs->t) == TPK_SET)
			{
				return (SET_LEN(vs) > 0);
			}
			else
			{
				switch (vs->t)
				{
					case TP_STR:
						return (strlen(vs->v.s) > 0);

					case TP_ISTR:
						return (strlen(glvar->strings.strings_ind[vs->v.i]) > 0);

					case TP_INT:
					case TP_BIN:
						return (vs->v.i != 0);

					case TP_DBL:
						return (vs->v.f != 0.);

					case TPP_NULL:
						return false;

					default:
						/* Ungueltige Konvertierung; ersatzweise false zurueck */
						SPRINT_ERR_3(ERROR_CONV_EXPL, FILEIO->output_typename(vs->t), FILEIO->output_typename(TP_BIN));
						FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
				}
			}
		}

		return false;
	}

	/* Konvertiert Wert in String */
	char* IntFuncBase::fconv_to_str(val_str *vs, char *format, bool expl_conv, bool *alloc)
	{
		char *s;
		const char *cs = glvar->temp_buf;
		*alloc = false;

		if (!expl_conv && (vs->t != TP_STR && vs->t != TP_ISTR && vs->t != TPP_NULL))
		{
			/* Ungueltige implizite Konvertierung; ersatzweise String "0" zurueck */
			SPRINT_ERR_3(ERROR_CONV_IMPL, FILEIO->output_typename(vs->t), FILEIO->output_typename(TP_STR));
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			SPRINT_TEMP_1("0");
		}
		else
		{
			if (TYPECLASS(vs->t) == TPK_SET)
			{
				FILEIO->write_set_to_str(glvar->temp_buf, vs, TEMPBUFLEN);
			}
			else
			{
				switch (vs->t)
				{
					case TP_STR:
						if (!format)
							return vs->v.s;
						else
							SPRINT_TEMP_2(format, vs->v.s);
						break;

					case TP_ISTR:
						if (!format)
							cs = glvar->strings.strings_ind[vs->v.i];
						else
							SPRINT_TEMP_2(format, glvar->strings.strings_ind[vs->v.i]);
						break;

					case TP_INT:
					case TP_BIN:
						SPRINT_TEMP_2((format ? format : "%ld"), vs->v.i);
						break;

					case TP_DBL:
                        SPRINT_TEMP_2((format ? format : "%f"), vs->v.f);
						break;

					case TPP_NULL:
						SPRINT_TEMP_1("");
						break;
					
					case TPK_PROBLEM_MAIN:
						SPRINT_TEMP_2((format ? format : "%s"), ((CMPLProblemMain*)(vs->v.subsyms))->filename);
						break;

					default:
						if (TP_IS_TUPEL(vs->t) || TP_IS_TUPELSET(vs->t))
							FILEIO->write_tupel_to_str(glvar->temp_buf, vs, TEMPBUFLEN);
						else
						{
							/* Ungueltige Konvertierung; ersatzweise String "0" zurueck */
							SPRINT_ERR_3(ERROR_CONV_EXPL, FILEIO->output_typename(vs->t), FILEIO->output_typename(TP_STR));
							FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
							SPRINT_TEMP_1("0");
						}
				}
			}
		}

		/* Speicherplatz hier nur temporaer reservieren */
		s = (char *) calloc(strlen(cs) + 1, sizeof(char));
		strcpy(s, cs);
		*alloc = true;

		return s;
	}

	/* Konvertiert Wert in cmpl-Problem */
	CMPLProblemMain *IntFuncBase::fconv_to_problem(val_str *vs, bool expl_conv)
	{
		// Konvertierung ist nur explizit und nur aus String möglich
		val_str v;
		if (!expl_conv || (vs->t != TP_STR && vs->t != TP_ISTR))
		{
			/* Ungueltige Konvertierung; ersatzweise aus String "??" konvertieren */
			SPRINT_ERR_3((expl_conv ? ERROR_CONV_EXPL : ERROR_CONV_IMPL), FILEIO->output_typename(vs->t), FILEIO->output_typename(TPK_PROBLEM_MAIN));
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);

			bool found;
			v.v.i = glvar->strings.search_insert(glvar, "??", false, &found);
			v.t = TP_ISTR;
			vs = &v;
		}

		if (vs->t == TP_STR)
		{
			glvar->string_to_istr(&v, vs->v.s, true);
			v.t = TP_ISTR;
			vs = &v;
		}

		CMPLProblemMain *p = (CMPLProblemMain *) glvar->c_getmem(sizeof(CMPLProblemMain));
		p->filename = glvar->strings.strings_ind[vs->v.i];
		p->status = PROBLEM_STAT_NEW;

		// Subsymbole hier noch nicht initialisieren, da genauer Typ nicht bekannt
		p->symtab = NULL;
		p->symval = NULL;

		return p;
	}


	/* Einsetzen Werte Laufkonstanten und Zeilennummer in Zeilenname */
	void IntFuncBase::flbeg_lname(char *n_dest, char *n_src, bool p_mod)
	{
		char s[ZC_NAME_LEN+100], sn[ZC_NAME_LEN+10];
		char *nsrc, *snp;
		int n, m;
		int num;
		bool is_num, is_str;

		for(n=0, nsrc=n_src; *nsrc; n++, nsrc++)
		{
			if(*nsrc==NUM_LINE_NAME)
			{
				nsrc++;
				if(*nsrc!=NUM_LINE_NAME)
				{
					for(m=0; *nsrc && *nsrc!=NUM_LINE_NAME; m++, nsrc++)
						sn[m] = *nsrc;

					sn[m] = '\0';
					is_num = true;
					is_str = false;

					if(*sn=='_') switch(sn[1])
					{
						case '1':
							if(p_mod)
							{
								sprintf(s+n, "%c%s%c",
											NUM_LINE_NAME, sn, NUM_LINE_NAME);
								for(snp=s+n; *snp; n++, snp++);
								is_num = false;
							}
							else
								num = glvar->akt_s_line;
							break;
						case '2':
							if(glvar->line_vekt_bl.lv_is_vekt)
							{
								num = glvar->line_vekt_bl.iter_set.cur_val.v.i;
								is_str = (glvar->line_vekt_bl.iter_set.cur_val.t == TP_ISTR);
							}
							else
							{
								if(m==2) num = 1;
								else is_num = false;
							}
							break;
						case '3':
							if(m==2)
								num = glvar->akt_line_side + 1;
							else
							{
								if(glvar->akt_line_side==0) is_num = false;
								else num = glvar->akt_line_side;
							}
							break;
						case '4':
#ifdef VAR_PROD
							if(p_mod)
							{
								s[n++] = NUM_LINE_NAME;
								s[n++] = '_';
								s[n++] = '0';
								if(m==3) s[n++] = '_';
								s[n++] = NUM_LINE_NAME;
								is_num = false;
							}
							else
							{
								if(m==2) num = 1;
								else is_num = false;
							}
#else
							is_num = false;
#endif
							break;
#ifdef VAR_PROD
						case '0':
							if(m==2)
								num = glvar->akt_s_line - glvar->pvarb_s_line + 2;
							else
								num = glvar->akt_s_line - glvar->pvarb_s_line + 1;
							break;
#endif
					}
					else
					{
						num = glvar->tmp_table[atoi(sn)].iter_set.cur_val.v.i;
						is_str = (glvar->tmp_table[atoi(sn)].iter_set.cur_val.t == TP_ISTR);
					}

					if(is_num)
					{
						if (is_str)
						{
							snprintf(sn, ZC_NAME_LEN, "%s", glvar->strings.strings_ind[num]);
							sn[ZC_NAME_LEN] = '\0';
						}
						else
						{
							sprintf(sn, "%d", num);
						}
						for(snp=sn; *snp; n++, snp++)
							s[n] = *snp;
					}
					if(*nsrc)
					{
						n--;
						continue;
					}
					break;
				}
			}
			s[n] = *nsrc;
		}

		s[n] = '\0';
		if(p_mod)
			s[ZC_NAME_LEN-1] = '\0';
		else
		{
			if(n>=DST_NAME_LEN)
			{
				snprintf(glvar->errbuf, ERRBUFLEN, ERROR_LINE_NAME_LEN, s);
				glvar->errbuf[ERRBUFLEN] = '\0';
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);
				s[DST_NAME_LEN-1] = '\0';
			}
		}

		strncpy(n_dest, s, ZC_NAME_LEN-1);
		n_dest[ZC_NAME_LEN-1] = '\0';
	}


	/* Aktuelle Werte in freie Indizes fuer implizite Schleifen einsetzen */
	void IntFuncBase::fset_free_ind(int i)
	{
		symbol_str *s_p = glvar->symbol_table + (i / MAX_ANZ_IND_1);
		int c_i = i % MAX_ANZ_IND_1;
		int n, m, m1;
		int db, ub;
		bool mod_sp, trp2, colexp;
		SetIterator *iter_p;
		ind_inf_one_str *i_info;
		array_str *array;
		val_str arrset;
		union ptr_sym_u up;
		bool found;

		glvar->symbol_akt_p = s_p;
		if((--(glvar->ind_inf_ptr)) < glvar->ind_inf_table) {
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_PRV_SET_IND);
		}

		if(glvar->ind_inf_ptr->i_free_ind > 2) {
			FILEIO->ferror_symname(glvar->temp_buf, true, TEMPBUFLEN);
			SPRINT_ERR_2(ERROR_MATRIX_FREE_IND, glvar->temp_buf);
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, glvar->errbuf);
		}

		trp2 = false;
		if(glvar->ind_inf_ptr->i_free_ind==2 && glvar->ind_inf_ptr->i_is_transp) trp2 = true;
		mod_sp = true;
		if(glvar->ind_inf_ptr->i_free_ind==1 && glvar->ind_inf_ptr->i_is_transp || trp2)
			mod_sp = false;
		colexp = glvar->term_ind_bl.ti_is_colexp;
		m = glvar->term_ind_bl.ti_anz_prod;

		array = s_p->val_ip[c_i-1];
                if (!array)
                {
                    FILEIO->ferror_symname(glvar->temp_buf, true, TEMPBUFLEN);
                    SPRINT_ERR_2("Dimensions of '%s' don't match", glvar->temp_buf);
                    FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, glvar->errbuf);
                }

		for(n=0, i_info = glvar->ind_inf_ptr->i_ind; n<c_i; n++, i_info++)
		{
			if(i_info->is_free)
			{
				if (array->defset.t != TPP_EMPTY)
				{
					arrset = array->defset;
				}
				else
				{
					arrset.t = TP_SET_SYM_ARR;
					arrset.v.array = array;
				}

				if((!trp2 && !mod_sp && glvar->term_ind_bl.ti_is_colexp) ||
							(mod_sp && !colexp && glvar->term_ind_bl.ti_is_gcol)) 
				{
					FILEIO->ferror_symname(glvar->temp_buf, true, TEMPBUFLEN);
					SPRINT_ERR_2(ERROR_MATRIX_PROD, glvar->temp_buf);
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, glvar->errbuf);
				}

				if(mod_sp && !colexp)
				{
					iter_p = &(glvar->line_vekt_bl.iter_set);

					if(glvar->line_vekt_bl.lv_is_floop && glvar->term_ind_bl.ti_is_floop)
					{
#ifdef PROTO
						if(glvar->is_protokoll)
						{
							char buf1[101], buf2[101];
							FILEIO->ferror_symname(glvar->errbuf, true, ERRBUFLEN);
							FILEIO->write_set_to_str(buf1, &(i_info->iter_set.set), 100);
							FILEIO->write_set_to_str(buf2, &arrset, 100);
							PROTO_OUT_4("\n   vector column %s : %s .. %s", glvar->errbuf, buf1, buf2);
						}
#endif

						if(!glvar->line_vekt_bl.lv_is_vekt)
						{
							iter_p->set = i_info->iter_set.set;
							if (SET_USE_POINTER(iter_p->set.t) && !iter_p->set.v.set->is_gl_mem)
							{
								/* Reservierten Speicher fuer den Set jetzt in Speicherverwaltung aufnehmen */
								glvar->ins_mem((void *)(iter_p->set.v.set));
								iter_p->set.v.set->is_gl_mem = true;
							}

							glvar->line_vekt_bl.lv_is_vekt = true;

							if (!iter_p->iter_set2_start(&arrset, true))
							{
								FILEIO->ferror_symname(glvar->temp_buf, true, TEMPBUFLEN);
								SPRINT_ERR_2(ERROR_EMPTY_SET_FREE_IND, glvar->temp_buf);
								FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, glvar->errbuf);
							}
						}
						else
						{
							/* Hier nur Laenge pruefen (wenn Elemente selbst nicht passen, dann bei Zugriff darauf der Fehler) */
							if (iter_p->iter_len() != SET2_LEN(&(i_info->iter_set.set), &arrset))
							{
#ifdef PROTO
								int a1 = iter_p->iter_len();
								int a2 = SET2_LEN(&(i_info->iter_set.set), &arrset);
								PROTO_OUT_3("\n   l1=%d l2=%d  ", a1, a2);
#endif
								FILEIO->ferror_symname(glvar->temp_buf, true, TEMPBUFLEN);
								SPRINT_ERR_2(ERROR_VEKTOR_LEN, glvar->temp_buf);
								FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, glvar->errbuf);
							}
						}
					}

					i_info->cur_val = iter_p->cur_val.v.i;
					i_info->cur_tp = iter_p->cur_val.t;
					glvar->term_ind_bl.ti_is_gcol = true;
				}
				else
				{
					m1 = (trp2 && !mod_sp && colexp) ? m+1 : m;
					iter_p = glvar->term_ind_bl.iter_set + m1;

					if(glvar->term_ind_bl.ti_is_floop)
					{
						if(!mod_sp)
						{
							iter_p->set = i_info->iter_set.set;
							if (SET_USE_POINTER(iter_p->set.t) && !iter_p->set.v.set->is_gl_mem)
							{
								/* Reservierten Speicher fuer den Set jetzt in Speicherverwaltung aufnehmen */
								glvar->ins_mem((void *)(iter_p->set.v.set));
								iter_p->set.v.set->is_gl_mem = true;
							}

							if (!iter_p->iter_set2_start(&arrset, true))
							{
								FILEIO->ferror_symname(glvar->temp_buf, true, TEMPBUFLEN);
								SPRINT_ERR_2(ERROR_EMPTY_SET_FREE_IND, glvar->temp_buf);
								FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, glvar->errbuf);
							}
						}
						else
						{
							/* Hier nur Laenge pruefen (wenn Elemente selbst nicht passen, dann bei Zugriff darauf der Fehler) */
							if (iter_p->iter_len() != SET2_LEN(&(i_info->iter_set.set), &arrset))
							{
								FILEIO->ferror_symname(glvar->temp_buf, true, TEMPBUFLEN);
								SPRINT_ERR_2(ERROR_VEKTOR_LEN, glvar->temp_buf);
								FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, glvar->errbuf);
							}
						}
					}

					i_info->cur_val = iter_p->cur_val.v.i;
					i_info->cur_tp = iter_p->cur_val.t;

					if(mod_sp)
					{
						if((++m) >= MAX_ANZ_VPROD)
						{
							FILEIO->ferror_symname(glvar->temp_buf, true, TEMPBUFLEN);
							SPRINT_ERR_2(ERROR_MATRIX_PROD_MAX, glvar->temp_buf);
							FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, glvar->errbuf);
						}
						glvar->term_ind_bl.ti_anz_prod = m;
						if(!trp2) glvar->term_ind_bl.ti_is_colexp = false;
					}
					else
					{
						glvar->term_ind_bl.ti_is_colexp = true;
					}
				}

				i_info->is_free = false;
				mod_sp = mod_sp ? false : true;
			}

			/* Zum Array des naechsten Index */
			if (array->attr.t == PTR_TP_ARRAY)
			{
				up = search_ind_elem(array, i_info->cur_val, i_info->cur_tp, &found, (array_ins_info_str *)0);
				if (!found)
				{
					FILEIO->ferror_symname(glvar->temp_buf, true, TEMPBUFLEN);
					SPRINT_ERR_2(ERROR_SYMBOL_NODEF, glvar->temp_buf);
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
				}
				else
					array = up.a;
			}
		}

		ffree_index_sets(glvar->ind_inf_ptr);

#ifdef PROTO
		if(glvar->is_protokoll)
		{
			FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
			PROTO_OUT_2("\n   vector element %s", glvar->errbuf);
		}
#endif
	}

	/* Indizes vom Wertestack zum Indexstack */
	void IntFuncBase::set_index_info(bool ind_given, bool set_freemem, Tupelset *tps, bool keep_tps)
	{
		int n, i, ind_t;
		val_str v;
		ind_inf_one_str *inf_p;
		bool tp_to_indices;

		glvar->ind_inf_ptr->i_anz_ind = (tps ? (tps->rank() == 0 ? 0 : 1) : pop_int());
		glvar->ind_inf_ptr->i_free_ind = 0;
		glvar->ind_inf_ptr->i_is_transp = false;
		glvar->ind_inf_ptr->i_is_tupelset = false;

		if (tps)
		{
			v.v.tps = tps;
			v.t = TPK_TUPELSET_N((tps->rank() >= 0 ? tps->rank() : 1));
		}

		for(n=glvar->ind_inf_ptr->i_anz_ind-1; n>=0; n--)
		{
			inf_p = glvar->ind_inf_ptr->i_ind + n;
			if (ind_given || tps)
			{
				if (!tps)
					pop_val(&v);

				// Tupel oder Tupelset zur Disaggregation in Einzelindizes (soweit zerlegbar) zurück auf den Stack
				if (TP_IS_TUPEL(v.t) || TP_IS_TUPELSET(v.t))
				{
					tp_to_indices = true;
					if (TP_IS_TUPELSET(v.t))
					{
						if ((keep_tps && v.v.tps->rank() > 1) || v.v.tps->hasEnum())
							tp_to_indices = false;
					}

					if (tp_to_indices)
					{
						//TODO: bei Iteration ueber Tupelset Speicher nicht freigeben, weil dort freigegeben, sonst aber doch
						//int cnt = push_tupel_indices(&v, true);
						int cnt = push_tupel_indices(&v, false, false);
						if (cnt == 0)
						{
							glvar->ind_inf_ptr->i_anz_ind--;

							for (i = n; i < glvar->ind_inf_ptr->i_anz_ind; i++)
								glvar->ind_inf_ptr->i_ind[i] = glvar->ind_inf_ptr->i_ind[i+1];

							continue;
						}
						else
						{
							if (cnt > 1)
							{
								cnt--;
								glvar->ind_inf_ptr->i_anz_ind += cnt;
								n += cnt;

								for (i = glvar->ind_inf_ptr->i_anz_ind-1; i > n; i--)
									glvar->ind_inf_ptr->i_ind[i] = glvar->ind_inf_ptr->i_ind[i-cnt];

								inf_p = glvar->ind_inf_ptr->i_ind + n;
							}

							pop_val(&v);
						}
					}
				}
			}
			else
			{
				v.t = TP_SET_ALL;
			}
				
			if (TYPECLASS(v.t) == TPK_SET)
			{
				/* Freier Index */
				glvar->ind_inf_ptr->i_free_ind++;
				inf_p->is_free = true;
				inf_p->iter_set.set = v;
			}
			else if (TP_IS_TUPELSET(v.t))
			{
				/* Tupelset: da schon soweit möglich disaggregiert, muss es ein enumeration tupelset sein; erstmal einfach aufnehmen */
				glvar->ind_inf_ptr->i_free_ind++;
				inf_p->is_free = true;
				inf_p->iter_set.set = v;
				glvar->ind_inf_ptr->i_is_tupelset = true;
			}
			else
			{
				/* Nicht-freier Index */
				inf_p->is_free = false;
				inf_p->cur_val = test_val_index(&v, &(inf_p->cur_tp));
			}
		}

		if (glvar->ind_inf_ptr->i_is_tupelset && glvar->ind_inf_ptr->i_anz_ind > 1)
		{
			// enumeration tupelset neben weiteren Indizes enthalten: alles in ein einzelnes Tupelset aggregieren
			Tupelset *tps = tupelsetFromIndexinfo(glvar->ind_inf_ptr);

			glvar->ind_inf_ptr->i_free_ind = glvar->ind_inf_ptr->i_anz_ind;
			glvar->ind_inf_ptr->i_anz_ind = 1;

			inf_p = glvar->ind_inf_ptr->i_ind;
			inf_p->is_free = true;

			inf_p->iter_set.set.v.tps = tps;
			inf_p->iter_set.set.t = TPK_TUPELSET_N((tps->rank() >= 0 ? tps->rank() : 1));
		}

		glvar->ind_inf_ptr->i_is_freemem = set_freemem;

		(glvar->ind_inf_ptr)++;
		PROTO_OUT_2("  index info stackptr : %d", (int)(glvar->ind_inf_ptr-glvar->ind_inf_table));

		if(glvar->ind_inf_ptr >= glvar->ind_inf_table + MAX_IND_INF) {
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_REK_SET_IND_MAX);
		}
	}

	/* generates a tupelset from an index info */
	Tupelset *IntFuncBase::tupelsetFromIndexinfo(ind_inf_str *indInfo)
	{
		int n;
		ind_inf_one_str *inf_p;
		val_str v;

		for (n=0; n < indInfo->i_anz_ind; n++)
		{
			inf_p = indInfo->i_ind + n;
			if (inf_p->is_free)
				push_val(&(inf_p->iter_set.set));
			else
			{
				v.v.i = inf_p->cur_val;
				v.t = inf_p->cur_tp;
				push_val(&v);
			}
		}

		n = indInfo->i_anz_ind;

		long len;
		int d;
		val_str *arr = pop_list_elems(n, false, &len, &d);

		return Tupelset::constructTupel(glvar, n, arr, n, false);
	}

	/* Gibt alle nicht global reservierten Sets aus Indexinfo frei */
	void IntFuncBase::ffree_index_sets(ind_inf_str *ind_inf_p)
	{
		if (ind_inf_p->i_is_freemem && ind_inf_p->i_anz_ind > 0)
		{
			if (ind_inf_p->i_is_tupelset)
			{
				Tupelset *tps = ind_inf_p->i_ind->iter_set.set.v.tps;
				if (!tps->globAlloc())
				{
					Tupelset::freeTupelset(glvar, tps);
					ind_inf_p->i_ind->iter_set.set.t = TP_SET_EMPTY;
				}
			}
			else
			{
				int n;
				ind_inf_one_str *inf_p;
				set_ea_str *eap;

				for (n = 0, inf_p = ind_inf_p->i_ind; n < ind_inf_p->i_anz_ind; n++, inf_p++)
				{
					if (inf_p->is_free && SET_USE_POINTER(inf_p->iter_set.set.t))
					{
						eap = inf_p->iter_set.set.v.set;
						if (!eap->is_gl_mem)
						{
							inf_p->iter_set.set.t = TP_SET_EMPTY;
							free(eap);
						}
					}
				}
			}
		}
	}
}

