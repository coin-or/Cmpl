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


#include "IntFuncCtrl.hh"
#include "gencode.h"


namespace cmpl
{
	//********************************************************
	// IntFuncLoopCtrl
	//********************************************************

	/* Beginn oder Ende einer Bedingung/Schleife */
	void IntFuncLoopCtrl::exec(val_str *vp)
	{
		int op = vp->v.c.sub;
		int nr = vp->v.c.par;

		/* Unterscheidung ob Beginn oder Ende */
		if (op == LOOP_END)
		{
			/* Ende: hier nichts weiter zu tun */
			PROTO_OUT_2("  %d", op);
		}
		else
		{
			if (vp[1].t != TP_INT)
			{
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
			}
			else
			{
				/* loop-Kontext initialisieren */
				loop_ctrl_str *loopp = glvar->tmp_table + nr;
				loopp->sub = op;
				loopp->mod = vp[1].v.i;
				loopp->exec_num = 0;
				loopp->start = true;
				loopp->is_lokal_assign = false;

				PROTO_OUT_4("  %d: %d %d", op, nr, (int)loopp->mod);

				/* Außer bei leerem Schleifenkopf Wert vom Stack holen */
				if (op != START_EMPTY)
				{
					pop_val(&(loopp->loop_val));
#ifdef PROTO
					if(glvar->is_protokoll)
					{
						bool f;
						char *s = fconv_to_str(&(loopp->loop_val), (char *)0, true, &f);
						PROTO_OUT_2("  loop val = %s", s);
						if (f)
							free(s);
					}
#endif
				}
				else
				{
					loopp->loop_val.t = TPP_EMPTY;
				}
			}
		}
	}


	//********************************************************
	// IntFuncTestIter
	//********************************************************

	/* Schleife iterieren und Endebedingung testen oder Bedingung pruefen */
	void IntFuncTestIter::exec(val_str *vp)
	{
		if (vp[1].t != TPP_ADDR || vp[2].t != TPP_ADDR || vp[3].t != TPP_ADDR)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			int nr = vp->v.c.par;
			loop_ctrl_str *loopp = glvar->tmp_table + nr;
			t_addr jmp = 0;

			PROTO_OUT_4("  %d: %d (%d)\n", loopp->sub, nr, (int)loopp->start);

			/* Pruefung haengt von Art der Bedingung/Schleife ab */
			if (loopp->sub == START_LOOP)
			{
				if (loopp->start)
				{
                                        /* Wert fuer Schleife in Iterationsset eintragen (muss Set oder Tupelset sein, noetigenfalls in Set wandeln) */
                                        if (TP_IS_TUPELSET(loopp->loop_val.t))
                                            loopp->iter_set.set = loopp->loop_val;
                                        else
                                            fconv_to_etp(&(loopp->loop_val), &(loopp->iter_set.set), TPK_SET, (char *)0, true);

					if (! loopp->iter_set.iter_set_start(true))
					{
						/* Schleife wird gar nicht durchlaufen, stattdessen zur naechsten Alternative */
						jmp = vp[3].v.a;
					}
				}
				else
				{
					/* Wenn Wert der Laufkonstante geaendert, dann jetzt Iteration entsprechend setzen */
					if (loopp->is_lokal_assign)
					{
						loopp->is_lokal_assign = false;
						if (! loopp->iter_set.iter_set_val(&(loopp->iter_set.cur_val)))
							jmp = vp[2].v.a;
					}

					/* Iteration der Schleife */
					if (loopp->exec_num == 1 && jmp == 0)
					{
						loopp->iter_set.iter_set_next();
						if (loopp->iter_set.set_end)
							jmp = vp[2].v.a;
						
					}
					else
					{
						/* Alternative wurde ausgefuert, jetzt beenden */
						jmp = vp[2].v.a;
					}
				}
				PROTO_OUT_3(" - iter to %d (%d)", loopp->iter_set.cur_val.v.i, (int)(loopp->iter_set.set_end));

				if (loopp->iter_set.set_end && SET_USE_POINTER(loopp->iter_set.set.t))
				{
					/* Iteration jetzt fertig, Set kann freigegeben werden */
					if (!loopp->iter_set.set.v.set->is_gl_mem)
					{
						free(loopp->iter_set.set.v.set);
						loopp->iter_set.set.v.set = (set_ea_str *)0;
						loopp->iter_set.set.t = TP_SET_EMPTY;
					}
				}
			}

			else
			{
				if (loopp->start)
				{
					/* Wenn Bedingung, dann Wert fuer Schleife als binary auswerten, sonst jedenfalls ausfuehren */
					if (loopp->sub == START_IF)
					{
						/* Wert fuer Schleife implizit in binary wandeln */
						bool is_true = fconv_to_bin(&(loopp->loop_val), false);
						PROTO_OUT_2(" - test %d\n", (int)is_true);

						/* Wenn Bedingung nicht erfuellt ist, zur naechsten Alternative */
						if (!is_true)
							jmp = vp[3].v.a;
					}

					/* Wenn lokale Definition, dann Wert der Schleife uebernehmen */
					else if (loopp->sub == START_ASSIGN)
					{
						/* Wert wird als aktueller Wert in iter_set eingetragen */
						loopp->iter_set.cur_val = loopp->loop_val;
					}
				}
				else
				{
					/* Wenn WHILE und irgendein Schleifenkoerper ausgefuehrt, dann jetzt Schleife neu starten; andernfalls zum Ende */
					if ((loopp->mod & HEAD_MOD_WHILE) && loopp->exec_num != 0)
						jmp = vp[1].v.a;
					else
						jmp = vp[2].v.a;
				}
			}

			loopp->start = false;
			if (jmp == 0)
				loopp->exec_num = 1;
			else
				glvar->int_ptr = jmp;
		}
	}


	//********************************************************
	// IntFuncTestAlter
	//********************************************************

	/* Alternativbedingung pruefen */
	void IntFuncTestAlter::exec(val_str *vp)
	{
		int op = vp->v.c.sub;
		int nr = vp->v.c.par;
		loop_ctrl_str *loopp = glvar->tmp_table + nr;
		t_addr jmp = 0;

		PROTO_OUT_3("  %d: %d", op, nr);

		/* Unterscheidung ob Default */
		if (op == TEST_ALTER_DEF)
		{
			/* Ausfuehren */
			loopp->exec_num = -1;
		}
		else
		{
			if (vp[1].t != TPP_ADDR)
			{
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
			}
			else
			{
				/* Wert vom Stack holen, implizit in binary wandeln */
				val_str v;
				bool is_true;

				pop_val(&v);
				is_true = fconv_to_bin(&v, false);
				PROTO_OUT_2(" - test %d", (int)is_true);

				if (is_true)
					loopp->exec_num = 2;
				else
					glvar->int_ptr = vp[1].v.a;
			}
		}
	}

	
	//********************************************************
	// IntFuncGoto
	//********************************************************

	/* Sprung zu anderer Stelle des Zwischencodes */
	void IntFuncGoto::exec(val_str *vp)
	{
		if (vp[1].t != TPP_ADDR)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			/* Unterkommando bestimmt, ob direkter oder indirekter Sprung */
			int op = vp->v.c.sub;
			t_addr a = vp[1].v.a;
			PROTO_OUT_3("  %d %u", op, a);

			if (op == GOTO_INDIREKT)
			{
				vp = glvar->prog + a;
				if (vp->t != TPP_ADDR)
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
				}
				a = vp->v.a;
			}

			glvar->int_ptr = a;
		}
	}
}

