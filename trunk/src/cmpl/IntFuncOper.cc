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


#include "IntFuncOper.hh"
#include "gencode.h"


namespace cmpl
{
	//********************************************************
	// IntFuncNumOp
	//********************************************************

	/* Numerische Operation ausfuehren */
	void IntFuncNumOp::exec(val_str *vp)
	{
		int op = vp->v.c.sub;
		int cnt = (op == NEG ? 1 : 2);
		bool f1, f2, fn, fo;
		val_str v1, v2;
		int n;
		long e;
		double d;

		pop_val(&v1);
		if (cnt == 2)
			pop_val(&v2);

		if (v1.t == TPP_NULL || cnt == 2 && v2.t == TPP_NULL)
		{
			/* Mindestens ein Operand ist Null: Wenn alle Operanden Null, dann Ergebnis immer Null, sonst Null in anderen Typ wandeln */
			if (cnt == 1 || (v1.t == TPP_NULL && v2.t == TPP_NULL))
			{
				push_val(&v1);
				return;
			}

			if (v1.t == TPP_NULL)
				fconv_to_etp(&v1, &v1, v2.t, (char *)0, false);
			else
				fconv_to_etp(&v2, &v2, v1.t, (char *)0, false);
		}

		/* Wenn beide Operanden Set, dann stattdessen Set-Operation ausfuehren */
		if (TYPECLASS(v1.t) == TPK_SET && TYPECLASS(v2.t) == TPK_SET)
		{
			fset_op(op, v2, v1);
			return;
		}

		/* Wenn ADD und ein Operand String, dann Verkettung */
		if (op == ADD && (v1.t == TP_STR || v1.t == TP_ISTR || v2.t == TP_STR || v2.t == TP_ISTR))
		{
			fstrcat(&v2, &v1);
			return;
		}

		/* binary wird in nummerischen Ausdruck als int behandelt */
		if (v1.t == TP_BIN)
			v1.t = TP_INT;
		if (cnt == 2 && v2.t == TP_BIN)
			v2.t = TP_INT;

		/* Beide Operanden muessen nummerisch sein */
		if ((v1.t != TP_INT && v1.t != TP_DBL) || (cnt == 2 && v2.t != TP_INT && v2.t != TP_DBL))
		{
			SPRINT_ERR_2(ERROR_NOT_A_NUMBER, write_val_to_temp_buf((v1.t != TP_INT && v1.t != TP_DBL ? &v1 : &v2)));
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			push_int(1);
			return;
		}

		f1 = (v1.t == TP_INT);
		f2 = (v2.t == TP_INT);
		fo = (f1 && (cnt == 1 || f2) && glvar->aufruf_p->num_op_mode != NUM_OP_ALWAYS_DBL);
		fn = (fo && glvar->aufruf_p->num_op_mode == NUM_OP_NOCHECK);

		/* Division durch 0 ist Fehler: Fehler ausgeben, ersatzweises Ergebnis ist 1 */
		if ((op == DIVD || op == DIVI || op == MODI) && ((f1 && v1.v.i == 0) || (!f1 && IS_EQ(v1.v.f, 0.)))) { 
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_DIV_0);
			push_int(1);
			return;
		}

		/* Bei ganzzahliger Division muessen beide Operanden als long-Werte darstellbar sein, ersatzweises Ergebnis ist 1 */
		if ((op == DIVI || op == MODI) && ((!f1 && (v1.v.f >= LONG_MAX || v1.v.f <= LONG_MIN)) || ((!f2 && (v2.v.f >= LONG_MAX || v2.v.f <= LONG_MIN)))))
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INT_MAX_MIN);
			push_int(1);
			return;
		}

		switch(op)
		{
			case NEG:
				PROTO_OUT_1(" NEG");
				if (fn)
					push_int(-v1.v.i);
				else
					push_rund(-(f1 ? (double)v1.v.i : v1.v.f), fo);
				break;

			case ADD:
				PROTO_OUT_1(" ADD");
				if (fn)
					push_int(v2.v.i + v1.v.i);
				else
					push_rund((f2 ? (double)v2.v.i : v2.v.f) + (f1 ? (double)v1.v.i : v1.v.f), fo);
				break;

			case SUB:
				PROTO_OUT_1(" SUB");
				if (fn)
					push_int(v2.v.i - v1.v.i);
				else
					push_rund((f2 ? (double)v2.v.i : v2.v.f) - (f1 ? (double)v1.v.i : v1.v.f), fo);
				break;

			case MULT:
				PROTO_OUT_1(" MULT");
				if (fn)
					push_int(v2.v.i * v1.v.i);
				else
					push_rund((f2 ? (double)v2.v.i : v2.v.f) * (f1 ? (double)v1.v.i : v1.v.f), fo);
				break;

			case DIVD:
				PROTO_OUT_1(" DIVD");
				/* DIVD hat immer double-Ergebnis, auch bei 2 int-Operanden */
				push_dbl((f2 ? (double)v2.v.i : v2.v.f) / (f1 ? (double)v1.v.i : v1.v.f));
				break;

			case DIVI:
				PROTO_OUT_1(" DIVI");
				push_int((f2 ? v2.v.i : RUND(v2.v.f)) / (f1 ? v1.v.i : RUND(v1.v.f)));
				break;

			case MODI:
				PROTO_OUT_1(" MODI");
				push_int((f2 ? v2.v.i : RUND(v2.v.f)) % (f1 ? v1.v.i : RUND(v1.v.f)));
				break;

			case POW:
				PROTO_OUT_1(" POW");
				/* Bei kleinen ganzen Zahlen jedenfalls testweise ganzzahlig rechnen */
				if (fo && v1.v.i <= 10) {
					for (n=0, e=1; n < v1.v.i; n++, e *= v2.v.i) ;
					if (fn)
						push_int(e);
				}
				if (!fo || !fn)
				{
					d = pow((f2 ? (double)v2.v.i : v2.v.f), (f1 ? (double)v1.v.i : v1.v.f));
					if (fo && e == RUND(d))
						push_int(e);
					else
						push_dbl(d);
				}
				break;
		}
	}

	/* Stringverkettung ausfuehren */
	void IntFuncNumOp::fstrcat(val_str *v1, val_str *v2)
	{
		int i;
		val_str *vp;
		val_str vd;
		const char *s1, *s2;
		char *sd;
		bool f;

		/* Beide Operanden müssen String sein, noetigenfalls konvertieren */
		for (i = 0; i < 2; i++)
		{
			vp = (i == 0 ? v1 : v2);
			if (vp->t != TP_STR && vp->t != TP_ISTR)
			{
				sd = fconv_to_str(vp, (char *)0, true, &f);
				vp->t = TP_STR;
				vp->v.s = sd;
			}
		}

		/* Strings als char* holen */
		s1 = (v1->t == TP_STR ? v1->v.s : glvar->strings.strings_ind[v1->v.i]);
		s2 = (v2->t == TP_STR ? v2->v.s : glvar->strings.strings_ind[v2->v.i]);

		/* Verkettung ausfuehren (Speicher dafuer direkt reservieren statt ueber c_getmem, da nur temporaer verwendet) */
		sd = (char *) calloc(strlen(s1) + strlen(s2) + 1, sizeof(char));
		strcpy(sd, s1);
		strcat(sd, s2);

		vd.t = TP_STR;
		vd.v.s = sd;
		push_val(&vd);
		
		/* Wenn Operanden TP_STR, dann kann der Speicher dafuer jetzt freigegeben werden */
		if (v1->t == TP_STR)
			free(v1->v.s);
		if (v2->t == TP_STR)
			free(v2->v.s);
	}

	/* Mengenoperation: Erster Operand (v1) bestimmt die Reihenfolge, dann kommen gegebenenfalls noch Elemente aus zweitem Operand in ihrer Reihenfolge (v2) hinzu */
	void IntFuncNumOp::fset_op(int op, val_str v1, val_str v2)
	{
		val_str v1t, v2t, vr, vro, *v1p, *v2p, *vrp;
		int cnt;
		bool iter = false, order = false, error = false;

		/* Beide Operanden muessen Sets sein, noetigenfalls in Sets wandeln */
		if (TYPECLASS(v1.t) == TPK_SET)
			v1p = &v1;
		else
		{
			v1p = &v1t;
			fconv_to_etp(&v1, v1p, TPK_SET, (char *)0, false);
		}

		if (TYPECLASS(v2.t) == TPK_SET)
			v2p = &v2;
		else
		{
			v2p = &v2t;
			fconv_to_etp(&v2, v2p, TPK_SET, (char *)0, false);
		}

		if (SET_USE_NO_PAR(v1p->t) || SET_USE_NO_PAR(v2p->t))
		{
			/* Mindestens ein Set ist bereits durch seinen Typ voll bestimmt (unendlich oder leer) */
			if (v1p->t == v2p->t)
			{
				vrp = v1p;
			}
			else if (v1p->t == TP_SET_EMPTY)
			{
				vrp = (op == ADD ? v2p : v1p);
			}
			else if (v2p->t == TP_SET_EMPTY)
			{
				vrp = (op == ADD ? v1p : v2p);
			}
			else if (v1p->t == TP_SET_ALL || (v1p->t == TP_SET_ALL_NUM && !SET_CAN_STRINGS(v2p->t)))
			{
				vrp = (op == ADD ? v1p : v2p);
				order = (op != ADD);
			}
			else if (v2p->t == TP_SET_ALL || (v2p->t == TP_SET_ALL_NUM && !SET_CAN_STRINGS(v1p->t)))
			{
				vrp = (op == ADD ? v2p : v1p);
			}
			else if ((v1p->t == TP_SET_ALL_NUM && v2p->t == TP_SET_ALL_STR) || (v1p->t == TP_SET_ALL_STR && v2p->t == TP_SET_ALL_NUM))
			{
				vr.t = (op == ADD ? TP_SET_ALL : TP_SET_EMPTY);
				vrp = &vr;
			}
			else if ((v1p->t == TP_SET_ALL_STR && !SET_CAN_STRINGS(v2p->t)) || (v2p->t == TP_SET_ALL_STR && !SET_CAN_STRINGS(v1p->t)))
			{
				if (op == ADD)
				{
					vrp = (v1p->t == TP_SET_ALL_STR ? v1p : v2p);
					cnt = SET_LEN((v1p->t == TP_SET_ALL_STR ? v2p : v1p));
					error = (cnt != 0);
				}
				else
				{
					vr.t = TP_SET_EMPTY;
					vrp = &vr;
				}
			}
			else
			{
				/* Hier ist jedenfalls ein Set TP_SET_ALL_NUM/TP_SET_ALL_STR, der andere TP_SET_ENUM/TP_SET_SYM_ARR */
				val_str *vup = (SET_USE_NO_PAR(v1p->t) ? v1p : v2p);
				val_str *vep = (SET_USE_NO_PAR(v1p->t) ? v2p : v1p);

				if (op == ADD)
				{
					vrp = vup;
					if ((vup->t == TP_SET_ALL_NUM && SET_LEN_TP(vep, TP_ISTR) != 0) || (vup->t == TP_SET_ALL_STR && SET_LEN_TP(vep, TP_INT) != 0))
						error = true;
				}
				else
				{
					if ((vup->t == TP_SET_ALL_NUM && SET_LEN_TP(vep, TP_INT) == 0) || (vup->t == TP_SET_ALL_STR && SET_LEN_TP(vep, TP_ISTR) == 0))
					{
						vr.t = TP_SET_EMPTY;
						vrp = &vr;
					}
					else
						iter = true; 
				}
			}
		}
		else if (SET_IS_INFINITE(v1p->t) || SET_IS_INFINITE(v2p->t))
		{
			/* Mindestens ein Set ist halbseitig beschraenkter unendlicher Set (alle anderen unendlichen sind schon oben behandelt) */
			if (SET_IS_INFINITE(v1p->t) && SET_IS_INFINITE(v2p->t))
			{
				if (op == ADD)
				{
					vrp = ((v1p->t == TP_SET_HALF_LB && v1p->v.i <= v2p->v.i) || (v1p->t == TP_SET_HALF_UB && v1p->v.i >= v2p->v.i) ? v1p : v2p);
					if (v1p->t != v2p->t)
					{
						if ((v1p->t == TP_SET_HALF_LB ? v1p->v.i : v2p->v.i) <= (v1p->t == TP_SET_HALF_UB ? v1p->v.i : v2p->v.i))
						{
							vr.t = TP_SET_ALL_NUM;
							vrp = &vr;
						}
						else
							error = true;
					}
				}
				else
				{
					if (v1p->t == v2p->t)
					{
						vrp = ((v1p->t == TP_SET_HALF_LB && v1p->v.i >= v2p->v.i) || (v1p->t == TP_SET_HALF_UB && v1p->v.i <= v2p->v.i) ? v1p : v2p);
					}
					else
					{
						/* Algorithmischer Set von unterer zu oberer Schranke */
						push_int((v1p->t == TP_SET_HALF_LB ? v1p->v.i : v2p->v.i));
						push_int((v1p->t == TP_SET_HALF_UB ? v1p->v.i : v2p->v.i));
						vrp = &vr;
						construct_set(TP_SET_ALG_S1, vrp);
					}
				}
			}
			else
			{
				val_str *vup = (SET_IS_INFINITE(v1p->t) ? v1p : v2p);
				val_str *vep = (SET_IS_INFINITE(v1p->t) ? v2p : v1p);

				if (op == ADD)
				{
					int diff = SET_LEN(vep) - SET2_LEN(vep, vup);
					vrp = vup;

					if (diff > 0)
					{
						/* Es kann sich hier um den Fall handeln, dass durch den endlichen Set gerade so Werte hinzukommen,
						 * dass die Vereinigung weiterhin ein halbseitig beschraenkter Set aber nun mit anderer Schranke ist.
						 * Dann muessen direkt angrenzend an vup hier diff Elemente in vep enthalten sein */
						int z;
						int s = (vup->t == TP_SET_HALF_LB ? -1 : 1);
						val_str it;

						it.t = TP_INT;
						for (z = diff, it.v.i = vup->v.i + s; z > 0; it.v.i += s, z--)
						{
							if (!SET_TEST_ELEM(vep, &it))
							{
								error = true;
								break;
							}
						}

						if (!error)
						{
							vr.t = vup->t;
							vr.v.i = vup->v.i + (s * diff);
							vrp = &vr;
						}
					}
				}
				else
				{
					if (SET_LEN_TP(vep, TP_INT) == 0)
					{
						vr.t = TP_SET_EMPTY;
						vrp = &vr;
					}
					else
						iter = true; 
				}
			}
		}
		else
		{
			/* Beide Sets sind nicht unendlich und nicht leer */
			iter = true;
		}

		/* Noetigenfalls muss das Ergebnis durch Iteration ueber die Sets bestimmt werden */
		if (iter)
		{
			/* Durch Obiges sind hier bei op == ADD jedenfalls beide Sets endlich, sonst kann einer auch unendlich sein */
			int len = 0;
			SetIterator iter_set(glvar);

			if (op == ADD)
			{
				iter_set.set = *v1p;
				iter_set.iter_set_start(true);
				while (!iter_set.set_end)
				{
					push_val(&(iter_set.cur_val));
					len++;
					iter_set.iter_set_next();
				}

				iter_set.set = *v2p;
				iter_set.iter_set_start(true);
				while (!iter_set.set_end)
				{
					if (!iter_set.set_test_elem(v1p, &(iter_set.cur_val)))
					{
						push_val(&(iter_set.cur_val));
						len++;
					}
					iter_set.iter_set_next();
				}
			}
			else
			{
				val_str *vup = (SET_IS_INFINITE(v1p->t) ? v1p : v2p);
				iter_set.set = (SET_IS_INFINITE(v1p->t) ? *v2p : *v1p);

				iter_set.iter_set2_start(vup, true);
				while (!iter_set.set_end)
				{
					push_val(&(iter_set.cur_val));
					len++;
					iter_set.iter_set_next();
				}

				order = SET_IS_INFINITE(v1p->t);
			}

			push_int(len);
			vrp = &vr;
			construct_set(TP_SET_ENUM, vrp);
		}

		/* Falls das Ergebnis nicht darstellbar ist, dann Fehler */
		if (error)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_SET_NOT_SUPPORTED);
		}

		/* Noetigenfalls muss das Ergebnis nun noch in die Standardreihenfolge gebracht werden */
		if (order)
		{
			order_set(vrp, &vro);
			vrp = &vro;
		}

		/* Ergebnis auf den Stack */
		push_val(vrp);
	}

	/* Gibt zu einem Eingabeset vi den in die Standardreihenfolge gebrachten Set vo mit denselben Elementen zurueck */
	void IntFuncNumOp::order_set(val_str *vi, val_str *vo)
	{
		set_ea_str *set_ea_p;

		/* Wenn Definitionsset direkt gesetzt, dann verwenden */
		if (SET_USE_ARRAY(vi->t) && vi->v.array->defset.t != TPP_EMPTY)
			vi = &(vi->v.array->defset);

		/* Nur algorithmische und Aufzaehlungssets koennen ueberhaupt abweichende Reihenfolge haben */
		if (!SET_USE_POINTER(vi->t))
		{
			*vo = *vi;
			return;
		}

		set_ea_p = vi->v.set;
		if (set_ea_p->len == 0)
		{
			*vo = *vi;
			return;
		}

		if (set_ea_p->is_enum)
		{
			/* Aufzaehlungsset: Erstmal pruefen, ob schon in Standardreihenfolge */
			int i;
			for (i = 0; i < set_ea_p->len; i++)
			{
				if (i != set_ea_p->u.e.index[i])
					break;
			}

			if (i >= set_ea_p->len)
			{
				*vo = *vi;
			}
			else
			{
				for (i = 0; i < set_ea_p->len; i++)
				{
					push_val(set_ea_p->u.e.array + set_ea_p->u.e.index[i]);
				}
				push_int(set_ea_p->len);
				construct_set(TP_SET_ENUM, vo);
			}
		}
		else
		{
			/* Algorithmischer Set: Wenn Schrittweite negativ, dann umkehren */
			if (set_ea_p->u.a.step_val >= 0)
			{
				*vo = *vi;
			}
			else
			{
				push_int(set_ea_p->u.a.start_val + (set_ea_p->u.a.step_val * (set_ea_p->len - 1)));
				push_int(-set_ea_p->u.a.step_val);
				push_int(set_ea_p->u.a.start_val);
				construct_set(TP_SET_ALG, vo);
			}
		}
	}


	//********************************************************
	// IntFuncSetOp
	//********************************************************

	/* Mengenoperation ausfuehren */
	void IntFuncSetOp::exec(val_str *vp)
	{
		int op = vp->v.c.sub;
		val_str v1, v2;

		/* Erster Operand (v1) bestimmt die Reihenfolge, dann kommen gegebenenfalls noch Elemente aus zweitem Operand in ihrer Reihenfolge (v2) hinzu */
		pop_val(&v2);
		pop_val(&v1);

		fset_op(op, v1, v2);
	}


	//********************************************************
	// IntFuncLogOp
	//********************************************************

	/* Logische Operation ausfuehren */
	void IntFuncLogOp::exec(val_str *vp)
	{
		int op = vp->v.c.sub;
		int cnt = (op == NOT ? 1 : 2);
		bool f1, f2;
		val_str v1, v2;
		bool res;

		pop_val(&v1);
		if (cnt == 2)
			pop_val(&v2);

		/* Beide Operanden noetigenfalls in TP_BIN wandeln */
		f1 = fconv_to_bin(&v1, false);
		if (cnt == 2)
			f2 = fconv_to_bin(&v2, false);

		switch (op)
		{
			case NOT:
				PROTO_OUT_1(" NOT");
				res = !f1;
				break;

			case AND:
				PROTO_OUT_1(" AND");
				res = f1 && f2;
				break;

			case OR:
				PROTO_OUT_1(" OR");
				res = f1 || f2;
				break;
		}

		v1.t = TP_BIN;
		v1.v.i = (res ? 1 : 0);
		push_val(&v1);
	}


	//********************************************************
	// IntFuncCmpOp
	//********************************************************

	/* Vergleichs-Operation ausfuehren */
	void IntFuncCmpOp::exec(val_str *vp)
	{
		int op = vp->v.c.sub;
		val_str v1, v2, v1t, v2t, *v1p, *v2p;
		enum etp tp1, tp2;
		bool res;

		pop_val(&v1);
		pop_val(&v2);

		if (v1.t == TPP_NULL || v2.t == TPP_NULL)
		{
			/* Mindestens ein Operand ist Null: Wenn beide Operanden Null, dann gleich, sonst Null in anderen Typ wandeln (INSET ist immer false, wenn ein Operand Null ist) */
			if ((v1.t == TPP_NULL && v2.t == TPP_NULL) || op == INSET)
			{
				v1.t = TP_BIN;
				v1.v.i = (op != INSET ? 1 : 0);
				push_val(&v1);
				return;
			}

			if (v1.t == TPP_NULL)
				fconv_to_etp(&v1, &v1, v2.t, (char *)0, false);
			else
				fconv_to_etp(&v2, &v2, v1.t, (char *)0, false);
		}

		/* Test auf Datentyp der auszufuehrenden Operation */
		if (op == INSET)
		{
			tp1 = TPK_SET;
			tp2 = (v2.t == TP_STR || v2.t == TP_ISTR ? TP_ISTR : TP_INT);
		}
		else
		{
			if (TYPECLASS(v1.t) == TPK_SET || TYPECLASS(v2.t) == TPK_SET)
				tp1 = TPK_SET;
			else if (v1.t == TP_DBL || v2.t == TP_DBL)
				tp1 = TP_DBL;
			else if (v1.t == TP_INT || v2.t == TP_INT)
				tp1 = TP_INT;
			else if (v1.t == TP_BIN || v2.t == TP_BIN)
				tp1 = TP_BIN;
			else
				tp1 = TP_STR;

			tp2 = tp1;
		}

		/* Operanden in den Datentyp wandeln */
		if (v1.t == tp1)
			v1p = &v1;
		else
		{
			v1p = &v1t;
			fconv_to_etp(&v1, v1p, tp1, (char *)0, false);
		}

		if (v2.t == tp2)
			v2p = &v2;
		else
		{
			v2p = &v2t;
			fconv_to_etp(&v2, v2p, tp2, (char *)0, false);
		}

		if (op == INSET)
		{
			PROTO_OUT_1(" INSET");
			res = SET_TEST_ELEM(v1p, v2p);
		}
		else
		{
			/* Operation je nach Datentyp ausfuehren */
			switch (tp1)
			{
				case TPK_SET:
					res = fcmp_set(op, v2p, v1p);
					break;

				case TP_DBL:
					res = fcmp_dbl(op, v2p->v.f, v1p->v.f);
					break;

				case TP_INT:
				case TP_BIN:
					res = fcmp_int(op, v2p->v.i, v1p->v.i);
					break;

				case TP_STR:
					res = fcmp_str(op, v2p->v.s, v1p->v.s);
					break;
			}
		}

		v1.t = TP_BIN;
		v1.v.i = (res ? 1 : 0);
		push_val(&v1);
	}

	/* Vergleich von int-Werten */
	bool IntFuncCmpOp::fcmp_int(int op, int v1, int v2)
	{
		switch (op)
		{
			case EQ:
			case EQ2:
				PROTO_OUT_1(" EQ (int)");
				return (v1 == v2);
			case GE:
				PROTO_OUT_1(" GE (int)");
				return (v1 >= v2);
			case LE:
				PROTO_OUT_1(" GE (int)");
				return (v1 <= v2);
		}
	}

	/* Vergleich von double-Werten */
	bool IntFuncCmpOp::fcmp_dbl(int op, double v1, double v2)
	{
		switch (op)
		{
			case EQ:
			case EQ2:
				PROTO_OUT_1(" EQ (dbl)");
				return (IS_EQ(v1, v2));
			case GE:
				PROTO_OUT_1(" GE (dbl)");
				return (v1 >= v2 || IS_EQ(v1, v2));
			case LE:
				PROTO_OUT_1(" LE (dbl)");
				return (v1 <= v2 || IS_EQ(v1, v2));
		}
	}

	/* Vergleich von string-Werten */
	bool IntFuncCmpOp::fcmp_str(int op, char *v1, char *v2)
	{
		int i = strcmp(v1, v2);

		switch (op)
		{
			case EQ:
			case EQ2:
				PROTO_OUT_1(" EQ (int)");
				return (i == 0);
			case GE:
				PROTO_OUT_1(" GE (int)");
				return (i >= 0);
			case LE:
				PROTO_OUT_1(" LE (int)");
				return (i <= 0);
		}
	}

	/* Vergleich von Sets */
	bool IntFuncCmpOp::fcmp_set(int op, val_str *set1, val_str *set2)
	{
		int len1, len2, len3;

		/* Wenn LT oder GT testen, dann stattdessen LE bzw GE und Ungleichheit */
		if (op == LT || op == GT)
		{
			op = (op == LT ? LE : GE);
			if (fcmp_set(EQ, set1, set2))
				return false;
		}

		/* Wenn beide Sets den gleichen Typ haben, und keinen oder den gleichen int-Parameter, dann sind sie jedenfalls gleich */
		if (set1->t == set2->t && (SET_USE_NO_PAR(set1->t) || (SET_USE_VALINT(set1->t) && set1->v.i == set2->v.i)))
			return true;

		/* Sonstige triviale Faelle */
		if (set1->t == TP_SET_ALL || set2->t == TP_SET_ALL)
			return (set1->t == TP_SET_ALL ? (op == GE) : (op == LE));
		else if (set1->t == TP_SET_EMPTY || set2->t == TP_SET_EMPTY)
			return (set1->t == TP_SET_EMPTY ? (op == LE) : (op == GE));
		else if ((set1->t == TP_SET_ALL_NUM && !SET_CAN_STRINGS(set2->t)) || (set2->t == TP_SET_ALL_NUM && !SET_CAN_STRINGS(set1->t)))
			return (set1->t == TP_SET_ALL_NUM ? (op == GE) : (op == LE));
		else if ((set1->t == TP_SET_ALL_STR && !SET_CAN_STRINGS(set2->t)) || (set2->t == TP_SET_ALL_STR && !SET_CAN_STRINGS(set1->t)))
			return false;

		/* Sonstige Faelle mit zwei unendlichen Sets */
		if (SET_IS_INFINITE(set1->t) && SET_IS_INFINITE(set2->t))
		{
			/* Mit den trivialen Faellen sind schon alle solche Moeglichkeiten fuer TP_SET_ALL, TP_SET_ALL_NUM und TP_SET_ALL_STR abgedeckt,
			 * hier muessen also beide TP_SET_HALF_LB oder TP_SET_HALF_UB sein */
			if (set1->t == set2->t)
			{
				if (set1->t == TP_SET_HALF_LB)
					return (set1->v.i <= set2->v.i ? (op == GE) : (op == LE));
				else
					return (set1->v.i >= set2->v.i ? (op == GE) : (op == LE));
			}
			else
				return false;
		}
		else if ((op == EQ || op == EQ2) && (SET_IS_INFINITE(set1->t) || SET_IS_INFINITE(set2->t)))
		{
			/* Ein Set ist unendlich, der andere nicht, deshalb koennen sie nicht gleich sein */
			return false;
		}

		/* Anzahl der Elemente in beiden Sets feststellen */
		len1 = SET_LEN(set1);
		len2 = SET_LEN(set2);

		/* Triviale Faelle bezueglich der Laenge */
		if ((op == EQ || op == EQ2) && len1 != len2)
			return false;
		else if (len1 == 0 && len2 == 0)
			return true;
		else if (len1 == 0 || len2 == 0)
			return (len1 == 0 ? (op == LE) : (op == GE));

		if (op != EQ2)
		{
			/* Vergleich ohne Beruecksichtigung Reihenfolge: Elemente im Durchschnitt der beiden Sets zaehlen */
			len3 = SET2_LEN(set1, set2);
			if (len1 == len2)
				return (len3 == len1);
			else if (len3 == len1)
				return (op == LE);
			else if (len3 == len2)
				return (op == GE);
			else
				return false;
		}
		else
		{
			/* Vergleich auf Gleichheit mit Beruecksichtigung Reihenfolge: Beide Sets parallel iterieren */
			SetIterator iter1(glvar, set1);
			SetIterator iter2(glvar, set2);

			iter1.iter_set_start(true);
			iter2.iter_set_start(true);

			while (!iter1.set_end)
			{
				if (iter1.cur_val.t != iter2.cur_val.t || iter1.cur_val.v.i != iter2.cur_val.v.i)
					return false;

				iter1.iter_set_next();
				iter2.iter_set_next();
			}

			return true;
		}
	}


	//********************************************************
	// IntFuncCountOp
	//********************************************************

	/* Zaehl-Operation ausfuehren */
	void IntFuncCountOp::exec(val_str *vp)
	{
		if (vp->v.c.sub == CNT_INC)
		{
			/* Zu einer Anzahl Werte auf dem Stack, denen die Anzahl folgt, wurde ein weiterer Wert gestellt.
			 * Die Anzahl-Angabe muss wieder auf die oberste Stackposition gebracht und inkrementiert werden. */
			val_str v1;
			int anz;

			pop_val(&v1);
			anz = pop_int();

			push_val(&v1);
			push_int(anz + 1);
		}
		else
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
	}


	//********************************************************
	// IntFuncConvOp
	//********************************************************

	/* Konvertierungs-Operation ausfuehren */
	void IntFuncConvOp::exec(val_str *vp)
	{
		enum etp tp = (enum etp) vp->v.c.par;
		val_str vs, vd;
		char *format = (char *)0;
		bool free_format = false;
		bool f;

		pop_val(&vs);

		if (vp->v.c.sub == WITH_FORMAT)
		{
			val_str vf;
			pop_val(&vf);
			format = fconv_to_str(&vf, (char *)0, false, &f);
			free_format = (vf.t == TP_STR);
		}

		/* Bei Wandlung in String wird immer in TP_STR gewandelt */
		if (tp == TP_ISTR)
			tp = TP_STR;

		/* Wenn Typ bereits stimmt, dann keine Konvertierung notwendig */
		if (!format && (vs.t == tp || (tp == TP_STR && vs.t == TP_ISTR)))
		{
			push_val(&vs);
			return;
		}

		/* Konvertieren */
		fconv_to_etp(&vs, &vd, tp, format, true);

		/* Wenn aus TP_STR in etwas anderes gewandelt, dann kann String jetzt freigegeben werden; ebenfalls gegebenenfalls Format */
		if (vs.t == TP_STR)
			free(vs.v.s);
		if (free_format)
			free(format);

		push_val(&vd);
	}


	//********************************************************
	// IntFuncTypeStr
	//********************************************************

	/* Typ eines konstanten Werts oder einer Variable als String */
	void IntFuncTypeStr::exec(val_str *vp)
	{
		val_str v, vt;
		const char *type;

		if (vp->v.c.sub == TYPE_EXPR)
		{
			/* Typ fuer Ausdruck auf dem Stack */
			pop_val(&v);
		}
		else
		{
			/* Typ fuer Variable */
			if (vp[1].t != TP_INT)
			{
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
				v.t = TPP_EMPTY;
			}
			else
			{
				int i = vp[1].v.i;
				symbol_str *s_p = glvar->symbol_table + (i / MAX_ANZ_IND_1);
				int n;
				int c_i = i % MAX_ANZ_IND_1;
				ind_inf_one_str *inf_p;
				union ptr_sym_u up;
				bool found;
				enum ptrtp pt;

				for(n=c_i-1, inf_p=glvar->ind_inf_ptr->i_ind+n; n>=0; n--, inf_p--) {
					inf_p->cur_val = pop_index(&(inf_p->cur_tp));
				}

				glvar->symbol_akt_p = s_p;
				glvar->ind_inf_ptr->i_anz_ind = c_i;

				up = ffetch_p(s_p, (array_str *)0, 0, &found, &pt);

				v.t = TPP_EMPTY;
				if (found && pt == PTR_TP_VAR && up.v != (var_str *)0)
				{
					switch (up.v->v_type)
					{
						case TP_VAR_DBL: 	v.t = TP_DBL; break;
						case TP_VAR_INT:	v.t = TP_INT; break;
						case TP_VAR_BIN:	v.t = TP_BIN; break;
					}
				}
			}
		}

		/* Typstring immer als TP_ISTR */
		type = FILEIO->output_typename(v.t);
		glvar->string_to_istr(&vt, type, false);
		push_val(&vt); 
	}


	//********************************************************
	// IntFuncMathFunc
	//********************************************************

	/* stegger 1.3.1.b several math functions */
	void IntFuncMathFunc::exec(val_str *vp)
	{
		val_str val;
		double d, r;
		short fct;
                //int sd;

		fct = vp->v.c.sub;

		pop_val(&val);
		d = fconv_to_dbl(&val, false);

		switch (fct)
		{
			case F_SQRT:
				r= sqrt(d);
                                if(isnan(r))
					f_math_func_error("sqrt");
				break;

			case F_EXP:
				r= exp(d);
				if(isnan(r) || r==HUGE_VAL || r==-HUGE_VAL)
					f_math_func_error("exp");
				break;

			case F_LOG:
				r= log(d);
				if(isnan(r) || r==HUGE_VAL || r==-HUGE_VAL)
					f_math_func_error("ln");
				break;

			case F_LOG10:
				r= log10(d);
				if(isnan(r) || r==HUGE_VAL || r==-HUGE_VAL) 
					f_math_func_error("lg");
				break;

			case F_LOG2:
				r= log(d)/log(2);
				if(isnan(r) || r==HUGE_VAL || r==-HUGE_VAL) 
					f_math_func_error("ld");
				break;

			case F_RAND:
                                d = rand()%(int)d;
				r = (double) d;
				if(isnan(r) ) 
					f_math_func_error("rand");
				break;

			case F_SIN:
				r= sin(d);
				if(isnan(r)) 
					f_math_func_error("sin");
				break;

			case F_COS:
				r = cos(d);
				if(isnan(r)) 
					f_math_func_error("cos");
				break;

			case F_TAN:
				r = tan(d);
				if(isnan(r)) 
					f_math_func_error("tan");
				break;

			case F_ACOS:
				if(d>=-1 && d<=1) {
					r = acos(d);
					if(isnan(r)) 
						f_math_func_error("acos");
				}
				else
					f_math_func_error("acos / range error");
				break;

			case F_ASIN:
				if(d>=-1 && d<=1) {
					r = asin(d);
					if(isnan(r)) 
						f_math_func_error("asin");
				}
				else
					f_math_func_error("asin / range error");
				break;

			case F_ATAN:
				r = atan(d);
				if(isnan(r)) 
					f_math_func_error("atan");
				break;

			case F_SINH:
				r = sinh(d);
				if(isnan(r)) 
					f_math_func_error("sinh");
				break;

			case F_COSH:
				r = cosh(d);
				if(isnan(r)) 
					f_math_func_error("cosh");
				break;

			case F_TANH:
				r = tanh(d);
				if(isnan(r)) 
					f_math_func_error("tanh");
				break;

			case F_ABS:
				r = abs(d);
                                if(isnan(r))
					f_math_func_error("abs");
				break;

			case F_CEIL:
				r = ceil(d);
				if(isnan(r)) 
					f_math_func_error("ceil");
				break;

			case F_FLOOR:
				r = floor(d);
				if(isnan(r)) 
					f_math_func_error("floor");
				break;

			case F_ROUND:
				r = round(d);
				if(isnan(r)) 
					f_math_func_error("round");
				break;

                        case F_SEED:
                                r = round(d);
                                srand( r );
                                if(isnan(r) )
                                        f_math_func_error("seed");
                                break;
		}
                push_dbl(r);
	}

	/* Fehlermeldung fuer mathematische Funktion */
	void IntFuncMathFunc::f_math_func_error(const char *func)
	{
		SPRINT_ERR_2(ERROR_MATH_FUNC, func);
		FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
	}


	//********************************************************
	// IntFuncAggrOp
	//********************************************************

	/* Aggregat-Operation (Maximum oder Minimum) ausfuehren */
	void IntFuncAggrOp::exec(val_str *vp)
	{
		int op = vp->v.c.sub;
		val_str v;
		bool mf;
		double mv, v1;
		int anz, n;

		anz = pop_int();
		if(anz == 0)
		{
			if (op == MAX_VAL_NULL || op == MIN_VAL_NULL)
			{
				val_str val0;
				val0.t = TPP_NULL;
				push_val(&val0);
			}
			else
			{
				/* Anzahl 0 ist nicht erlaubt */
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, (op==MAX_VAL ? ERROR_MAX_0 : ERROR_MIN_0));
				push_int(0);
			}
		}
		else
		{
			if (op == MAX_VAL_NULL || op == MIN_VAL_NULL)
				op = (op == MAX_VAL_NULL ? MAX_VAL : MIN_VAL);

			for (n = 0; n < anz; n++) {
				pop_val(&v);
				/* Null-Werte werden ignoriert */
				if (v.t != TPP_NULL)
				{
					/* Werte muessen nummerisch sein (Aggregat-Operation fuer Strings oder Sets gibt es derzeit nicht) */
					if (v.t != TP_INT && v.t != TP_DBL && v.t != TP_BIN) {
						SPRINT_ERR_2(ERROR_NOT_A_NUMBER, write_val_to_temp_buf(&v));
						FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
						v1 = 0;
					}
					else {
						v1 = (v.t != TP_DBL ? (double)v.v.i : v.v.f);
					}

					if (n==0 || (op==MAX_VAL ? (v1 > mv) : (v1 < mv))) {
						mv = v1;
						mf = (v.t != TP_DBL);
					}
				}
			}

			if(mf)
				push_int(RUND(mv));
			else
				push_dbl(mv);
		}
	}


	//********************************************************
	// IntFuncUtilOp
	//********************************************************

	/* Sonstige Operation (zur Zeit nur Laenge fuer Sets und Strings) */
	void IntFuncUtilOp::exec(val_str *vp)
	{
		int op = vp->v.c.sub;
		val_str v;
		int len;

		if (op == LEN_VAL)
		{
			pop_val(&v);

			/* Unterscheidung Set oder einfacher Wert */
			if (TYPECLASS(v.t) == TPK_SET)
			{
				len = SET_LEN(&v);
			}
			else
			{
				/* Wert in string wandeln */
				bool f;
				char *s = fconv_to_str(&v, (char *)0, true, &f);
				len = strlen(s);
				if (v.t == TP_STR)
					free(s);
			}

			push_int(len);
		}
	}


	//********************************************************
	// IntFuncDefFunc
	//********************************************************

	/* Feststellen, wieviel Elemente einer Matrix definiert sind */
	void IntFuncDefFunc::exec(val_str *vp)
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
			int n;
			long res = 0;
			PROTO_OUT_2("  %d", i);

			glvar->symbol_akt_p = s_p;
			if((--(glvar->ind_inf_ptr)) < glvar->ind_inf_table)
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_PRV_SET_IND);

			if(c_i == 0)
			{
				if (s_p->val_ip0)
				{
					bool found;
					ptr_symbol_elem(&(s_p->val_ip0->elem), s_p->val_ip0->attr.t, 0, &found);
					res = (found ? 1 : 0);
				}
			}
			else
			{
				res = ff_def_count(s_p->val_ip[c_i-1], c_i, 0);
			}

			push_int(res);

			ffree_index_sets(glvar->ind_inf_ptr);
		}
	}

	/* Zaehlt wieviele Elemente einer Matrix definiert sind */
	long IntFuncDefFunc::ff_def_count(array_str *array, int mi, int ii)
	{
		long res;
		union ptr_sym_u up;
		bool found;
		ind_inf_one_str *inf_p = glvar->ind_inf_ptr->i_ind + ii;

		if (!array)
			return 0;

		if (inf_p->is_free)
		{
			/* Freier Index: Definitionsset fuer das Array wird gebraucht */
			val_str arrset;
			arrset.t = TP_SET_SYM_ARR;
			arrset.v.array = array;

			if (ii < mi - 1)
			{
				/* Nichtletzter Index: Iteration ueber Durchschnitt des Sets des freien Index und des Definitionssets, darin dann rekursiven Aufruf fuer den naechsten Index */
				res = 0;
				inf_p->iter_set.iter_set2_start(&arrset, true);
				while (!inf_p->iter_set.set_end)
				{
					inf_p->cur_tp = inf_p->iter_set.cur_val.t;
					inf_p->cur_val = inf_p->iter_set.cur_val.v.i;

					up = search_ind_elem(array, inf_p->cur_val, inf_p->cur_tp, &found, (array_ins_info_str *)0);
					if (found)
						res += ff_def_count(up.a, mi, ii+1);

					inf_p->iter_set.iter_set_next();
				}
			}
			else
			{
				/* Letzter Index: Ergebnis ist Laenge des Durchschnitts des Sets des freien Index und des Definitionssets zurueck */
				res = SET2_LEN(&(inf_p->iter_set.set), &arrset);
			}
		}
		else
		{
			/* Index ist nicht frei: Wenn Element existiert, dann rekursiv zu naechstem Index bzw. bei letztem Index 1 zurueck */
			up = search_ind_elem(array, inf_p->cur_val, inf_p->cur_tp, &found, (array_ins_info_str *)0);
			if (found)
				res = (ii < mi - 1 ? ff_def_count(up.a, mi, ii+1) : 1); 
		}

		return res;
	}

	
	//********************************************************
	// IntFuncDefsetFunc
	//********************************************************

	/* Bestimmt Definitionsset des ersten freien Index einer Matrix */
	void IntFuncDefsetFunc::exec(val_str *vp)
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
			int n;
			val_str set;
			PROTO_OUT_2("  %d", i);

			glvar->symbol_akt_p = s_p;
			if((--(glvar->ind_inf_ptr)) < glvar->ind_inf_table)
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_PRV_SET_IND);

			/* Default fuer den Fall, dass gar kein freier Index oder Element nicht definiert */
			set.t = TP_SET_EMPTY;

			if (c_i > 0)
			{
				union ptr_sym_u up;
				bool found;
				array_str *array = s_p->val_ip[c_i-1];
				ind_inf_one_str *inf_p = glvar->ind_inf_ptr->i_ind;


				for (n = 0; n < c_i; n++, inf_p++)
				{
					if (inf_p->is_free)
					{
						/* Erster freier Index: Definitionsset bestimmen, als Durchschnitt zum fuer den Index angegebenen Set */
						set = defset_array(false, array, &(inf_p->iter_set));
						break;
					}

					/* Nicht-freier Index: Zum untergeordneten Array */
					if (array->attr.t != PTR_TP_ARRAY)
						break;
					
					up = search_ind_elem(array, inf_p->cur_val, inf_p->cur_tp, &found, (array_ins_info_str *)0);
					if (!found)
						break;

					array = up.a;
				}
			}

			push_val(&set);

			ffree_index_sets(glvar->ind_inf_ptr);
		}
	}


	//********************************************************
	// IntFuncDefsetExtnum
	//********************************************************

	/* Bestimmt hoechsten oder niedrigsten nummerischen Wert im Definitionsset des ersten freien Index einer Matrix */
	void IntFuncDefsetExtnum::exec(val_str *vp)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			int op = vp->v.c.sub;
			int i = vp[1].v.i;
			symbol_str *s_p = glvar->symbol_table + (i / MAX_ANZ_IND_1);
			int c_i = i % MAX_ANZ_IND_1;
			int n;
			long res;
			bool is_res;
			PROTO_OUT_2("  %d", i);

			glvar->symbol_akt_p = s_p;
			if((--(glvar->ind_inf_ptr)) < glvar->ind_inf_table)
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_INTERN_PRV_SET_IND);

			is_res = false;
			if (c_i > 0)
			{
				union ptr_sym_u up;
				bool found;
				array_str *array = s_p->val_ip[c_i-1];
				ind_inf_one_str *inf_p = glvar->ind_inf_ptr->i_ind;


				for (n = 0; n < c_i; n++, inf_p++)
				{
					if (inf_p->is_free)
					{
						/* Erster freier Index: Ergebnis aus Definitionsset und fuer den Index angegebenem Set bestimmen */
						res = defset_get_extnum((op==MAX_VAL || op==MAX_VAL_NULL ? MAX_VAL : MIN_VAL), array, &(inf_p->iter_set.set), &is_res);
						break;
					}

					/* Nicht-freier Index: Zum untergeordneten Array */
					if (array->attr.t != PTR_TP_ARRAY)
						break;
					
					up = search_ind_elem(array, inf_p->cur_val, inf_p->cur_tp, &found, (array_ins_info_str *)0);
					if (!found)
						break;

					array = up.a;
				}
			}

			if (is_res)
			{
				push_int(res);
			}
			else
			{
				if (op == MAX_VAL_NULL || op == MIN_VAL_NULL)
				{
					val_str val0;
					val0.t = TPP_NULL;
					push_val(&val0);
				}
				else
				{
					/* Kein Ergebnis ist nicht erlaubt */
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, (op==MAX_VAL ? ERROR_MAX_0 : ERROR_MIN_0));
					push_int(0);
				}
			}

			ffree_index_sets(glvar->ind_inf_ptr);
		}
	}

	/* Hoechsten oder niedrigsten nummerischen Wert in der Vereinigung eines Definitionssets und eines zweiten Sets bestimmen */
	long IntFuncDefsetExtnum::defset_get_extnum(int op, array_str *array, val_str *set, bool *is_res)
	{
		long res = 0;
		bool found = false;
		long i;
		*is_res = false;

		/* Wenn set keine nummerischen Werte enthalten kann, dann kann nichts gesucht werden */
		if (!SET_CAN_NUMBERS(set->t))
			return 0;

		/* Hoechsten oder niedrigsten Wert aus array holen */
		if (op == MAX_VAL)
		{
			if (array->a_num.cnt > 0)
			{
				for (i = array->a_num.len - 1; i >= 0; i--)
				{
					ptr_symbol_elem(&(array->a_num.array), array->attr.t, i, &found);
					if (found)
					{
						res = array->a_num.index[i];
						*is_res = true;
						break;
					}
				}
			}
			if (array->a_dir.cnt > 0 && (!*is_res || array->a_dir.len > res))
			{
				for (i = array->a_dir.len - 1; i >= 0 && (!*is_res || i > res); i--)
				{
					ptr_symbol_elem(&(array->a_dir.array), array->attr.t, i, &found);
					if (found)
					{
						res = i;
						*is_res = true;
						break;
					}
				}
			}
		}
		else
		{
			if (array->a_num.cnt > 0)
			{
				for (i = 0; i < array->a_num.len; i++)
				{
					ptr_symbol_elem(&(array->a_num.array), array->attr.t, i, &found);
					if (found)
					{
						res = array->a_num.index[i];
						*is_res = true;
						break;
					}
				}
			}
			if (array->a_dir.cnt > 0 && (!*is_res || res > 0))
			{
				for (i = 0; i < array->a_dir.len && (!*is_res || i < res); i++)
				{
					ptr_symbol_elem(&(array->a_dir.array), array->attr.t, i, &found);
					if (found)
					{
						res = i;
						*is_res = true;
						break;
					}
				}
			}
		}

		/* Ergebnis nur moeglich, wenn erstmal ein Wert gefunden wurde: Dann zweiten Set pruefen */
		if (*is_res)
		{
			val_str ind;
			ind.t = TP_INT;
			ind.v.i = res;
			if (SET_TEST_ELEM(set, &ind))
				return res;

			/* Bei halbseitig unendlichen Sets erstmal pruefen, ob ueberhaupt Wert moeglich ist */
			if ((op == MAX_VAL && set->t == TP_SET_HALF_LB && res < set->v.i) || (op != MAX_VAL && set->t == TP_SET_HALF_UB && res > set->v.i))
			{
				*is_res = false;
				return 0;
			}
			else
			{
				/* Ergebnis muss durch Iteration ueber beide Sets bestimmt werden */
				SetIterator iter(glvar);
				iter.set.t = TP_SET_SYM_ARR;
				iter.set.v.array = array;

				iter.iter_set2_start(set, (op != MAX_VAL));
				while (!iter.set_end)
				{
					if (iter.cur_val.t == TP_INT)
						return iter.cur_val.v.i;

					iter.iter_set_next();
				}
			}
		}

		/* Kein Ergebnis, ersatzweise 0 zurueck */
		*is_res = false;
		return 0;
	}

}

