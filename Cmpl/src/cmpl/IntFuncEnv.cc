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


#include "IntFuncEnv.hh"
#include "gencode.h"


namespace cmpl
{
	//********************************************************
	// IntFuncReadCSV
	//********************************************************

	/* stegger 1.3.1.b import values into a vector or matrix from a csv file */
	void IntFuncReadCSV::exec(val_str *vp)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			int dim = vp[1].v.i;

			val_str val;
			char *filename;
			bool alloc;

			pop_val(&val);
			filename = fconv_to_str(&val, (char *)0, false, &alloc);

			FILEIO->io_read_csv(dim, filename, this);

			if (val.t == TP_STR || alloc)
				free(filename);
		}
	}


	//********************************************************
	// IntFuncReadStdin
	//********************************************************

	/* stegger 1.3.1.b read a value from stdin */
	void IntFuncReadStdin::exec(val_str *vp)
	{
		val_str val, r;
		char *prompt;
		bool alloc;

		pop_val(&val);
		prompt = fconv_to_str(&val, (char *)0, false, &alloc);

		FILEIO->io_read_stdin(prompt, &r);
		push_val(&r);

		if (val.t == TP_STR || alloc)
			free(prompt);
	}


	//********************************************************
	// IntFuncOutputMsg
	//********************************************************

	/* Ausgabe nutzerdefinierte Meldung oder Fehlermeldung und Abbruch */
	void IntFuncOutputMsg::exec(val_str *vp)
	{
		int otp = (int) vp->v.c.sub;
		val_str val;
		char *s;
		bool alloc;

		if (otp != ONLY_NL)
		{
			pop_val(&val);
			s = fconv_to_str(&val, (char *)0, true, &alloc);
		}

		if (otp >= USR_MSG)
		{
			PROTO_OUT_2("  user message: %s", (otp != ONLY_NL ? s : "<nl>"));
			printf("%s%s", (otp != ONLY_NL ? s : ""), (otp == USR_MSG ? "" : "\n"));
		}
		else
		{
			PROTO_OUT_2("  user error: %s", s);
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_USER, otp, (char *)0, s);
		}

		if (otp != ONLY_NL && (val.t == TP_STR || alloc))
			free(s);
	}
		

	//********************************************************
	// IntFuncCmplSolve
	//********************************************************

	/* Funktionen im Zusammenhang mit dem Lösen von CMPL-Problemen */
	void IntFuncCmplSolve::exec(val_str *vp)
	{
	}

}

