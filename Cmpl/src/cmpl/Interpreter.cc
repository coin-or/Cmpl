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


#include "Interpreter.hh"
#include "IntFuncBase.hh"
#include "FileIO.hh"
#include "Compiler.hh"


namespace cmpl
{
	/* Konstruktor */
	Interpreter::Interpreter(CmplCore *m)
	{
		glvar = m;
		fileIO = glvar->fileIO;
	}


	/* Interpreterhauptfunktion */
	void Interpreter::int_prog()
	{
		Compiler compile(glvar);
		val_str v, *vp;

		compiler = &compile;

		glvar->anz_int_error = 0;				/* Anzahl Fehler */
		glvar->int_ptr = 0;						/* Position im Zwischencode */
		glvar->stack_ptr = 0;					/* naechster freier Stackplatz */
		glvar->akt_s_line = 1;					/* Zeile in Matrix */
		glvar->akt_defnum = 0;					/* naechste Variablendefinitionsnummer */
		glvar->dest_grd = (dest_str *)0;		/* Zeiger auf erste Matrixzeile */
		glvar->ind_inf_ptr = glvar->ind_inf_table;	/* Stackpointer fuer Indexstack */

		glvar->gl_nulltupel = NULL;
		glvar->gl_nulltupel = TupelUtil::construct(glvar, 0, &v, 0, true);

		v.t = TPK_TUPEL_N(0);
		glvar->gl_nulltupelset = NULL;
		glvar->gl_nulltupelset = Tupelset::constructEnum(glvar, &v, 1, -1, true);

		v.t = TP_SET_EMPTY;
		glvar->gl_emptytupelset = NULL;
		glvar->gl_emptytupelset = Tupelset::constructTupel(glvar, 1, &v, 1, true);

#ifdef VAR_PROD
		glvar->var_split_grd = (struct var_split_str *)0;
		glvar->var_prod_grd = (struct var_prod_str *)0;
		glvar->comp_end = glvar->comp_ptr;	/* Beginn Zwischencode fuer Zerlegung */
		glvar->vp_sn_num = 1;			/* fuer Bildung kurzer Namen */
#endif

		/* Interpreterschleife */
		while(1)
		{
			/* aktueller Befehl */
			vp = glvar->prog + glvar->int_ptr;

			if (test_command(vp))
			{
				glvar->int_ptr += vp->v.c.len + 1;
				if (!int_command(vp))
					break;
			}
			else
			{
				// Bei ungueltigem Zwischencodebefehl beenden (kann nur durch internen Fehler ausgeloest sein)
				break;
			}
		}
	}

	/* Testet ob der Zwischencodebefehl gueltig ist */
	bool Interpreter::test_command(val_str *vp)
	{
		unsigned short st;
		t_code_str *akt_code;

		if (vp->t != TPP_CMD)
		{
			/* Inkonsistenter Zwischencode */
			fileIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
			return false;
		}

		st = vp->v.c.cmd;
		akt_code = glvar->code_table + st;

		PROTO_OUT_4("\n%5u : (%d) %s\n", glvar->int_ptr, (int)st, akt_code->code_text);

		if (!akt_code->exec_class)
		{
			/* Kein ausfuehrbarer Zwischencodenbefehl */
			fileIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
			return false;
		}

		return true;
	}

	/* Fuehrt einzelnen Zwischencodebefehl aus; wenn Ende, dann false zurueck */	
	/* (ausser bei PROG_END, LINE_END oder Control-Befehlen bleibt glvar->int_ptr unveraendert) */
	bool Interpreter::int_command(val_str *vp)
	{
		unsigned short st;
		t_code_str *akt_code;
		t_addr l_int_ptr;
		int n;

		st = vp->v.c.cmd;
		akt_code = glvar->code_table + st;

#ifdef VAR_PROD
		if(st == PROG_END)
		{
                        if(glvar->args->isVarProdFollow())
			{
				if(glvar->int_ptr > glvar->comp_end)
				{
					glvar->int_ptr = l_int_ptr;
					glvar->comp_ptr = glvar->comp_end;
					return true;
				}
			}
			else if(glvar->int_ptr == glvar->comp_end)
			{
				compiler->comp(PROG_END, 0);
				return true;
			}
		}
#endif

		/* Ausfuehrende Funktion aufrufen */
		akt_code->exec_class->exec(vp);

#ifdef VAR_PROD
                if(st==LINE_END && glvar->args->isVarProdFollow() &&
					glvar->int_ptr < glvar->comp_end && glvar->comp_ptr > glvar->comp_end)
		{
			compiler->comp(PROG_END, 0);
			l_int_ptr = glvar->int_ptr;
			glvar->int_ptr = glvar->comp_end;
			return true;
		}
#endif /* VAR_PROD */

		if(st == PROG_END)
		{
			/* Interpreterende */
			return false;
		}

		return true;
	}
}

