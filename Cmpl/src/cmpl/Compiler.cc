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


#include "Compiler.hh"
#include "FileIO.hh"
#include "CmplData.hh"


namespace cmpl
{
	/* Konstruktor */
	Compiler::Compiler(CmplCore *m)
	{
		glvar = m;
		fileIO = m->fileIO;

		trace_scanning = false;
		trace_parsing = false;
	}


	/* Aufruf des Parsers */
	void Compiler::parse()
	{
		glvar->comp_ptr = 0;
		glvar->akt_src_line = 1;

		comp(SRC_LINE, 1);
		comp_int(glvar->akt_src_line);

		/* Symbole aus cmplData definieren */
		if (glvar->cmplData)
		{
			if (glvar->cmplData->cmplDataCompiler(this))
				comp(INIT_CMPLDATA, 0);
			else
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, glvar->cmplData->errorMsg().c_str());
		}

		scan_begin();

		/* Parsen */
		CMPL_Parser parser(*this);
		parser.parse();

		scan_end();
	}

	/* Compilieren Zwischencodebefehl */
	t_addr Compiler::comp(unsigned short st, unsigned short paranz)
	{
		t_addr old_ptr = glvar->comp_ptr;
		val_str *p = glvar->prog + old_ptr;

		p->t = TPP_CMD;
		p->v.c.cmd = st;
		p->v.c.sub = 0;
		p->v.c.par = 0;
		p->v.c.len = paranz;

		PROTO_OUT_3("\nCompile %5u : %s", glvar->comp_ptr, glvar->code_table[st].code_text);

		glvar->comp_ptr++;
		if(glvar->comp_ptr + paranz >= glvar->prog_max)
			glvar->incmem((void **) &(glvar->prog), &(glvar->prog_max), MAX_PROG, sizeof(val_str));
		return(old_ptr);
	}

	/* Compilieren Zwischencodebefehl mit Unterkommando und eingebettetem Parameter, und keinen weiteren Parametern */
	t_addr Compiler::comp_sub_par(unsigned short st, unsigned short sub, short par)
	{
		return comp_sub_par_cnt(st, sub, par, 0);
	}

	/* Compilieren Zwischencodebefehl mit Unterkommando und eingebettetem Parameter, und weiteren Parametern */
	t_addr Compiler::comp_sub_par_cnt(unsigned short st, unsigned short sub, short par, unsigned short paranz)
	{
		t_addr old_ptr = glvar->comp_ptr;
		val_str *p = glvar->prog + old_ptr;

		p->t = TPP_CMD;
		p->v.c.cmd = st;
		p->v.c.sub = sub;
		p->v.c.par = par;
		p->v.c.len = paranz;

		PROTO_OUT_3("\nCompile %5u : %s", glvar->comp_ptr, glvar->code_table[st].code_text);
		PROTO_OUT_3(" %d %d", (int)sub, (int)par);

		glvar->comp_ptr++;
		if(glvar->comp_ptr + paranz >= glvar->prog_max)
			glvar->incmem((void **) &(glvar->prog), &(glvar->prog_max), MAX_PROG, sizeof(val_str));
		return(old_ptr);
	}

	/* Compilieren Zwischencodebefehl, mit einem int-Parameter */
	t_addr Compiler::comp_par_int(unsigned short st, long par)
	{
		t_addr old_ptr = glvar->comp_ptr;
		val_str *p = glvar->prog + old_ptr;

		p->t = TPP_CMD;
		p->v.c.cmd = st;
		p->v.c.sub = 0;
		p->v.c.par = 0;
		p->v.c.len = 1;

		PROTO_OUT_3("\nCompile %5u : %s", glvar->comp_ptr, glvar->code_table[st].code_text);

		glvar->comp_ptr += 2;
		if(glvar->comp_ptr + 1 >= glvar->prog_max)
			glvar->incmem((void **) &(glvar->prog), &(glvar->prog_max), MAX_PROG, sizeof(val_str));

		p = glvar->prog + old_ptr + 1;
		p->t = TP_INT;
		p->v.i = par;

		PROTO_OUT_2("  %ld", par);

		return(old_ptr);
	}

	/* Compilieren Wert */
	t_addr Compiler::comp_val(val_str *vp)
	{
		t_addr old_ptr = glvar->comp_ptr;
		val_str *p = glvar->prog + old_ptr;

		*p = *vp;

		glvar->comp_ptr++;
		if(glvar->comp_ptr >= glvar->prog_max)
			glvar->incmem((void **) &(glvar->prog), &(glvar->prog_max), MAX_PROG, sizeof(val_str));

		PROTO_OUT_2("  (val) %ld", vp->v.i);
		return(old_ptr);
	}

	/* Compilieren Zwischencodeadresse */
	t_addr Compiler::comp_addr(t_addr a)
	{
		val_str v;
		v.t = TPP_ADDR;
		v.v.a = a;

		PROTO_OUT_2("  %u", a);
		return comp_val(&v);
	}

	/* Compilieren int-Wert */
	t_addr Compiler::comp_int(long i)
	{
		val_str v;
		v.t = TP_INT;
		v.v.i = i;

		PROTO_OUT_2("  %ld", i);
		return comp_val(&v);
	}

	/* Compilieren double-Wert */
	t_addr Compiler::comp_double(double f)
	{
		val_str v;
		v.t = TP_DBL;
		v.v.f = f;

        PROTO_OUT_2("  %f", f);
		return comp_val(&v);
	}

	/* Compilieren String (nur fuer Zeilennamen) */
	t_addr Compiler::comp_str(const char *s)
	{
		val_str v;
		int n = strlen(s);
		char *sn = (char *) glvar->c_getmem((n + 1) * sizeof(char));

		strcpy(sn, s);
		v.t = TP_STR;
                v.v.s = sn;

		PROTO_OUT_2("  %s", sn);
		return comp_val(&v);
	}

	/* Compilieren gespeicherter String */
	t_addr Compiler::comp_istr(long i)
	{
		val_str v;
		v.t = TP_ISTR;
		v.v.i = i;

		PROTO_OUT_3("  (%ld) %s", i, glvar->strings.strings_ind[i]);
		return comp_val(&v);
	}

	/* Compilieren Pointer (auf import-Parameterblock) */
	t_addr Compiler::comp_voidp(void *ptr)
	{
		val_str v;
		v.t = TPP_PTR;
		v.v.p = ptr;

		PROTO_OUT_2("  (void *)%p", ptr);
		return comp_val(&v);
	}

	/* Compilieren int-Wert an eine andere angebene Adresse dst */
	void Compiler::comp_int_at(t_addr dst, long i)
	{
		val_str *p = glvar->prog + dst;

		p->t = TP_INT;
		p->v.i = i;

		PROTO_OUT_3("\nCompile   %ld at %u", i, dst);
	}

	/* Compilieren Zwischencodeadresse a an eine andere angebene Adresse dst */
	void Compiler::comp_addr_at(t_addr dst, t_addr a)
	{
		val_str *p = glvar->prog + dst;

		p->t = TPP_ADDR;
		p->v.a = a;

		PROTO_OUT_3("\nCompile   %u at %u", a, dst);
	}

	/* Aktuelle Compilationsadresse zurueckgeben */
	t_addr Compiler::addr_comp()
	{
		return(glvar->comp_ptr);
	}

	/* Setzt aktuelle Compilationsadresse auf einen anderen Wert */
	void Compiler::set_addr_comp(t_addr dst)
	{
		while(dst >= glvar->prog_max)
			glvar->incmem((void **) &(glvar->prog), &(glvar->prog_max), MAX_PROG, sizeof(val_str));

		glvar->comp_ptr = dst;
	}

	/* Suche Symbol (Variable, Konstante oder Laufkonstante) */
	//TODO: entfaellt
	int Compiler::search_symbol(const char *str, int *ip)
	{
		register int n;
		register symbol_str *symp;
		register loop_ctrl_str *loopp;

		PROTO_OUT_2("\n   search for symbol '%s' :", str);

		for(n=0, loopp=glvar->tmp_table; n<glvar->tmp_level; n++, loopp++) {
			if(loopp->s_name && strcmp(str, loopp->s_name)==0) {
				PROTO_OUT_2(" %d", -(n+2));
				return(-(n+2));
			}
		}

		for(n=0, symp=glvar->symbol_table; n<glvar->symbol_anzahl; n++, symp++) {
			if(symp->s_name && strncmp(str, symp->s_name, SYM_NAME_LEN)==0) {
				PROTO_OUT_2(" %d", n * MAX_ANZ_IND_1);
				*ip = n;
				return n;
			}
		}

		PROTO_OUT_1(" not found");
		return -1;
	}

	/* Einfuegen Symbol */
	//TODO: entfaellt
	int Compiler::insert_symbol(const char *name, enum etp tpclass)
	{
		int d;
		int n = search_symbol(name, &d);
		int ret;
		register symbol_str *symp;

		PROTO_OUT_2("\n   insert symbol %s : ", name);

		if(n != -1) {
			snprintf(glvar->errbuf, ERRBUFLEN, ERROR_SYMBOL_DEF, name);
			glvar->errbuf[ERRBUFLEN] = '\0';
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, glvar->errbuf);
			ret = 0;
		}

		/* Platz in der Symboltabelle bereitstellen */
		if (glvar->symbol_anzahl + 1 >= glvar->symbol_max)
			glvar->incmem((void **) &(glvar->symbol_table), &(glvar->symbol_max), MAX_ANZ_SYMBOL, sizeof(symbol_str));

		symp = glvar->symbol_table + glvar->symbol_anzahl;
		(glvar->symbol_anzahl)++;

		/* Symbol einfuegen */
		//strncpy(symp->s_name, name, SYM_NAME_LEN-1);
		//symp->s_name[SYM_NAME_LEN-1] = '\0';
		symp->s_name = name;
		symp->tp_sym = tpclass;

		/* Initial gibt es keinen Wert fuer das Symbol */
		symp->val_ip0 = (nonarray_str *)0;
		for(n=0; n<MAX_ANZ_IND; n++)
			symp->val_ip[n] = (array_str *)0;

		ret = (glvar->symbol_anzahl-1);
		PROTO_OUT_2(" : %d", ret);
		return ret;
	}

	/* inserts and initializes a new symbol from a cmplData file */
	int Compiler::insertCmplDataSymbol(const char *name, enum etp tpclass, int dim)
	{
		// Symbol suchen und noetigenfalls Name einfuegen
		bool found;
		int sym = glvar->symbol_main_names.search_insert(glvar, name, true, &found);

		// Symbolinfo einfuegen, erstmal ohne Typ und ungueltig
		glvar->symbol_main_table.insert(glvar, glvar->symbol_main_names.elem(sym), sym, sym);

		// Symbol initialisieren
		EXPSTR exp;
		exp.set(tpclass, dim);
		exp.set_sym(&(glvar->symbol_main_table), sym);

		init_symbol(&exp, PTR_TP_CONST, tpclass, dim, false);

		return exp.symp()->symno * MAX_ANZ_IND_1 + dim;
	}

	/* Initialisiert Symboleintrag; exp muss einen Expression kennzeichnen, der ein Symbol darstellt */
	void Compiler::init_symbol(EXPSTR *exp, enum ptrtp symtp, enum etp tpk, int sym_dim, bool local)
	{
		symbol_comp_str *symp = exp->symp();
		symp->symtp = symtp;
		symp->tpk = tpk;
		symp->tp_sub_tab = exp->tp_sub_tab;
		symp->dim = sym_dim;		//exp->dim_in;
		symp->valid = true;
		symp->local = local;

		if (TP_WITH_SUBSYM(tpk) && symp->tp_sub_tab < 0)
		{
			symp->tp_sub_tab = glvar->symbol_sub_tables.insert(glvar);
			insert_standard_subsyms(symp);
		}

		PROTO_OUT_4("\n  init_symbol #%d (%s): dim:%d,", symp->symno, symp->s_name, symp->dim);
		PROTO_OUT_4(" tpk:%d (%d), local:%d  ", (int)(symp->tpk), symp->tp_sub_tab, (int)(symp->local));
	}

	/* Fügt fuer ein Symbol mit Subsymbolen die standardmäßigen Subsymbole ein */
	void Compiler::insert_standard_subsyms(symbol_comp_str *symp)
	{
		switch (TYPECLASS(symp->tpk))
		{
			case TPK_PROBLEM_MAIN:
				insert_subsymbol(symp->tp_sub_tab, SUB_AUTO_SOLUTION, TPK_PROBLEM_SOL, 1);
				insert_subsymbol(symp->tp_sub_tab, SUB_AUTO_OBJECTIVE, TPK_VALUE, 0);
				break;

			case TPK_PROBLEM_SOL:
				insert_subsymbol(symp->tp_sub_tab, SUB_AUTO_VARIABLES, TPK_PROBLEM_VC, 0);
				insert_subsymbol(symp->tp_sub_tab, SUB_AUTO_CONSTRAINTS, TPK_PROBLEM_VC, 0);
				break;

			case TPK_PROBLEM_VC:
				insert_subsymbol(symp->tp_sub_tab, SUB_AUTO_VC_ACTIVITY, TPK_PROBLEM_RES, 0);
				insert_subsymbol(symp->tp_sub_tab, SUB_AUTO_VC_MARGINAL, TPK_PROBLEM_RES, 0);
				insert_subsymbol(symp->tp_sub_tab, SUB_AUTO_VC_NAME, TPK_PROBLEM_RES, 0);
				insert_subsymbol(symp->tp_sub_tab, SUB_AUTO_VC_NUMBER, TPK_PROBLEM_RES, 0);
				break;

			case TPK_PROBLEM_RES:
				insert_subsymbol(symp->tp_sub_tab, SUB_AUTO_VCRES_FLAT, TPK_VALUE, 1);
				break;
		}
	}

	/* Fuegt ein Subsymbol in eine Subsymboltabelle ein und initialisiert es */
	void Compiler::insert_subsymbol(int tp_sub_tab, const char *sname, enum etp tpk, int dim)
	{
		// Subsymbol suchen und noetigenfalls Name einfuegen
        bool found;
        int subno = glvar->symbol_sub_names.search_insert(glvar, sname, true, &found);
        PROTO_OUT_4("\n    searched for standard subsymbol %s, returns #%d (found:%d)", sname, subno, (int)found);

		// In der Subsymboltabelle das Subsymbol mit der subno suchen; nötigenfalls noch einfügen
		SymbolTab *symtab = glvar->symbol_sub_tables.elem(tp_sub_tab);
		int symno = symtab->search(subno);
		if (symno < 0)
		{
			// In Subsymboltabelle einfügen
			symno = symtab->insert(glvar, glvar->symbol_sub_names.elem(subno), subno, -1);

			// Subsymbol initialisieren
			EXPSTR exp;
			exp.set_sym(symtab, symno);
			exp.set_dim(dim, dim, 0);
			init_symbol(&exp, PTR_TP_CONST, tpk, dim, false);
		}
	}

	/* Testet die Typklasse fuer ein Symbol */
	void Compiler::test_tpk_symbol(symbol_comp_str *symp, enum etp tpk, bool test_invalid_local)
	{
		if (symp->symtp == PTR_TP_EMPTY || !(symp->valid))
		{
			// Neues Symbol; nur gegebenenfalls auf bisherige Verwendung als lokales Symbol prüfen
			if (test_invalid_local && symp->local)
			{
				/*TODO: Warnung, dass sonst lokales Symbol ab hier als global definiert*/
			}
		}
		else {
			// Schon definiertes Symbol; Typklasse muss passen
			if (symp->tpk != tpk)
			{
				/* Symbol ist fuer andere Typklasse definiert */
				PROTO_OUT_3("\n  typeclass mismatch: %d - %d", (int)(symp->tpk), (int)tpk);	/*TODO*/
				SPRINT_ERR_2(ERROR_SYMBOL_TPK_MIX, symp->s_name);
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, glvar->errbuf);
			}
		}
	}

	/* Initialisiert einen loop-Kontext beim Compilieren, setzt den Zeilennamenprefix */
	void Compiler::init_loop_comp(unsigned short nr, bool base, enum looptp loop_tp, const char *lname)
	{
		/* Test ob noch frei */
		if (nr > MAX_ANZ_TMP - 1)
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_FATAL, ERROR_LOOP_MAX);

		PROTO_OUT_3("\n  init_loop_comp #%d (%s):", (int)nr, (lname ?: ""));

		/* Initialisieren */
		loop_ctrl_str *loopp = glvar->tmp_table + nr;
		loopp->init();

		/* Kennzeichen fuer Basis und Zeilennamenprefix setzen */
		loopp->base = base;
		loopp->loop_tp = loop_tp;
		loopp->l_name = lname;

		if (lname)
		{
			if (nr == 0 || !loopp[-1].line_name_pref)
				loopp->line_name_pref = lname;
			else
			{
				const char *s = (nr == 0 ? (char *)0 : loopp[-1].line_name_pref);
				char *sn = (char *) glvar->c_getmem(sizeof(char) * ((s ? strlen(s) : 0) + strlen(lname) + strlen(LINE_NANE_CONCAT_1) + 1));
				if (s)
					strcat(strcpy(sn, s), LINE_NANE_CONCAT_1);
				else
					sn[0] = '\0';
				strcat(sn, lname);
				loopp->line_name_pref = sn;
				/*
				loopp->line_name_pref = (char *) glvar->c_getmem(sizeof(char) * ((s ? strlen(s) : 0) + strlen(lname) + strlen(LINE_NANE_CONCAT_1) + 1));
				if (s)
					strcat(strcpy(loopp->line_name_pref, s), LINE_NANE_CONCAT_1);
				else
					loopp->line_name_pref[0] = '\0';
				strcat(loopp->line_name_pref, lname);
				*/
			}
		}
		else if (nr > 0)
			loopp->line_name_pref = loopp[-1].line_name_pref;
	}

	/* Setzt neuen aktuellen loop-Kontext beim Compilieren, initialisiert den naechsten */
	loop_ctrl_str *Compiler::set_loop_comp(unsigned short sub, unsigned short mod)
	{
		loop_ctrl_str *loopp;

		glvar->tmp_level++;

		loopp = glvar->tmp_table + (glvar->tmp_level - 1);
		loopp->mod = mod;
		loopp->valid = true;

		set_loop_sname(sub, (EXPSTR *)NULL, (int *)NULL);

		/* Naechsten loop-Kontext hier schon initialisieren */
		init_loop_comp(glvar->tmp_level, false, (glvar->tmp_level == 1 ? LOOP_EXEC : loopp[-1].loop_tp), (char *)0);

		return loopp;
	}

	/* Traegt Art und den Namen fuer die Laufkonstante bzw. die lokal definierte Konstante im loop-Kontext nach */
	void Compiler::set_loop_sname(unsigned short sub, EXPSTR *exp, int *symblock)
	{
		loop_ctrl_str *loopp = glvar->tmp_table + (glvar->tmp_level - 1);
		loopp->sub = sub;
		loopp->jump_end = (sub != START_LOOP && loopp->mod == 0);

		// Falls Expression, dann muss Symbol darin enthalten und undefiniert sein (durch die Grammatik gesichert)
		// (bzw. erstes Symbol aus symblock)
		int symind = (symblock ? glvar->symbol_main_table.search(symblock[0]) : (exp ? exp->sym_ind : -1));
		symbol_comp_str *symp = (symblock ? glvar->symbol_main_table.elem(symind) : (exp ? exp->symp() : (symbol_comp_str *)NULL));
		const char *sname = (symp ? symp->s_name : (const char *)NULL);

		loopp->s_name = sname;
		loopp->sym_ind = symind;
		PROTO_OUT_4("\nset_loop_comp: %d %d %s", (glvar->tmp_level - 1), loopp->sym_ind, loopp->s_name);

		// Symbol fertig initialisieren
		if (symblock)
		{
			// linke Seite ist Tupel aus undefinierten Laufkonstanten, alle Symbole daraus definieren
			EXPSTR expSym;
			int i;

			for (i=0; i<exp->dim_t_rank1; i++)
			{
				symind = glvar->symbol_main_table.search(symblock[i]);
				symp = glvar->symbol_main_table.elem(symind);
				symp->symno = glvar->tmp_level - 1 + 1024*(i+1);

				expSym.set_sym(&(glvar->symbol_main_table), symind);
				init_symbol(&expSym, PTR_TP_CONST, TPK_VALUE, 0, true);
			}
		}
		else if (symp)
		{
			symp->symno = glvar->tmp_level - 1;
			init_symbol(exp, PTR_TP_CONST, exp->tp, 0, true);
		}


		/* WHILE-Modifikator ist nicht mit START_LOOP erlaubt, und nicht in Summenschleife */
		if ((loopp->mod & HEAD_MOD_WHILE) && (sub == START_LOOP || loopp->loop_tp != LOOP_EXEC))
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_LOOP_WHILE);

		/* REF-Modifikator ist nur fuer lokale Definition erlaubt */
		if ((loopp->mod & HEAD_MOD_REF) && sub != START_ASSIGN)
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "ref nicht erlaubt");	//TODO

		/* Zeilennamenpostfix setzen */
		char *sn;
		if (sname)
		{
			if (glvar->tmp_level == 1 || !loopp[-1].line_name_post)
			{
				sn = (char *) glvar->c_getmem(sizeof(char) * (strlen(sname) + 3));
				sprintf(sn, "%c%s%c", NUM_LINE_NAME, sname, NUM_LINE_NAME);
				loopp->line_name_post = sn;
			}
			else
			{
				const char *s = (glvar->tmp_level == 1 ? (char *)0 : loopp[-1].line_name_post);
				sn = (char *) glvar->c_getmem(sizeof(char) * ((s ? strlen(s) : 0) + strlen(sname) + strlen(LINE_NANE_CONCAT_3) + 3));
				if (s)
					strcat(strcpy(sn, s), LINE_NANE_CONCAT_3);
				else
					sn[0] = '\0';
				sprintf(sn + strlen(sn), "%c%s%c", NUM_LINE_NAME, sname, NUM_LINE_NAME);
				loopp->line_name_post = sn;
			}
		}
		else if (glvar->tmp_level > 1)
			loopp->line_name_post = loopp[-1].line_name_post;

		/* Gegebenenfalls symblock freigeben */
		if (symblock)
			free(symblock);
	}

	/* Setzt loop-Kontexte bis einschliesslich naechstem Basiskontext zurueck; compiliert entsprechende Beendigungen */
	void Compiler::unset_loop_comp()
	{
		loop_ctrl_str *loopp = glvar->tmp_table + (glvar->tmp_level - 1);
		t_addr a;

		/* Alle loops bis zur Basis beenden */
		while (1)
		{
			PROTO_OUT_4("\nunset_loop_comp: %d %d %s", (glvar->tmp_level - 1), loopp->sym_ind, loopp->s_name);

			/* Ruecksprung zur Iteration fuer den letzten Block */
			if (!loopp->jump_end)
			{
				comp_sub_par_cnt(GOTO, GOTO_DIREKT, 0, 1);
				comp_addr(loopp->iter_start);
			}

			/* Ende des loops */
			a = comp_sub_par(LOOP_CTRL, LOOP_END, glvar->tmp_level-1);
			comp_addr_at(loopp->end_jump, a);
			if (loopp->last_jump != 0)
				comp_addr_at(loopp->last_jump, a);

			// iterator symbol goes out of scope
			symbol_comp_str *symp = loopp->symp(&(glvar->symbol_main_table));
			if (symp)
			{
				PROTO_OUT_3("\n    symbol #%d (%s) goes out of scope (A)", symp->symno, symp->s_name);
				symp->valid = false;

                // symno auf symbol index zuruecksetzen
                int symno = symp->symno;
                symp->symno = symp - glvar->symbol_main_table.elem(0);

                if (symno >= 1024)
                {
                    // Loop mit Tupeldisaggregation: auch übrige Symbole darin werden ungültig
                    int i;
                    symno = symno + 1024;
                    while (1)
                    {
                        i = glvar->symbol_main_table.search(symno);
                        if (i >= 0)
                        {
                            symp = glvar->symbol_main_table.elem(i);
                            PROTO_OUT_3("\n    symbol #%d (%s) goes out of scope (B)", symp->symno, symp->s_name);
                            symp->valid = false;
                            symp->symno = symp - glvar->symbol_main_table.elem(0);
                            symno += 1024;
                        }
                        else
                            break;
                    }
                }
            }

			/* Naechsten loop beenden */
			glvar->tmp_level--;
			if (loopp->base || glvar->tmp_level==0)
				break;

			loopp--;
		}
	}

	/* Sucht naechsten loop-Kontext mit passendem Laufkonstanten- oder Zeilennamen bzw. (wenn !s und mod != 0) passendem Modifikator */
	loop_ctrl_str *Compiler::search_loop_name_mod(const char *s, unsigned short mod, const char *errstr, bool iscontinue)
	{
		int i = glvar->tmp_level - 1;
		loop_ctrl_str *loopp = glvar->tmp_table + i;

		for (; i >= 0; i--, loopp--)
		{
			if (loopp->valid)
			{
				if (s)
				{
					if ((loopp->s_name && strcmp(loopp->s_name, s) == 0) || (loopp->l_name && strcmp(loopp->l_name, s) == 0))
						break;
				}
				else if (mod != 0)
				{
					if (loopp->mod & mod)
						break;
				}
				else
					break;
			}
		}

		if (i < 0)
		{
			/* Nicht gefunden */
			if (!s)
			{
				switch (mod)
				{
					case HEAD_MOD_REF: s = "ref"; break;
					case HEAD_MOD_WHILE: s = "while"; break;
					default: s = "<intern>"; break;
				}
			}

			SPRINT_ERR_3(ERROR_LOOP_CTRL_REF, s, errstr);
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, glvar->errbuf);
			return (loop_ctrl_str *)0;
		}

		/* continue darf nur fuer loop-Kontext mit Iteration verwendet werden; oder mit WHILE-Modifikator */
		if (iscontinue && loopp->sub != START_LOOP && !(loopp->mod & HEAD_MOD_WHILE))
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_LOOP_ONCE_CONT);
			return (loop_ctrl_str *)0;
		}

		return loopp;
	}

	/* Erstellt Zeilennamen; ersetzen Laufkonstantennamen in Zeilennamen durch Laufkonstantennummer */
	void Compiler::linename_repl(char *n_dest, const char *n_src)
	{
		char ln[ZC_NAME_LEN+1], s[ZC_NAME_LEN+50], sn[20], *snp;
		register char *nsrc;
		register int n, m;
		int si, di, num;

		/* Prefix aus loop-Kontext einfuegen */
		loop_ctrl_str *loopp = (glvar->tmp_level > 0 ? glvar->tmp_table + glvar->tmp_level - 1 : (loop_ctrl_str *)0);
		if (loopp && loopp->line_name_pref)
		{
			if (n_src)
				snprintf(ln, ZC_NAME_LEN, "%s%s%s", loopp->line_name_pref, LINE_NANE_CONCAT_1, n_src);
			else
				strncpy(ln, loopp->line_name_pref, ZC_NAME_LEN);
		}
		else
		{
			/* Wenn gar kein Name, dann Standard */
			strncpy(ln, (n_src ? n_src : LINE_NAME_DEF), ZC_NAME_LEN);
		}
		ln[ZC_NAME_LEN] = '\0';

		/* Wenn bisher keine Ersetzungen im Namen, dann auch Postfix aus loop-Kontext einfuegen */
		if (loopp && loopp->line_name_post)
		{
			for (nsrc=ln; *nsrc; nsrc++) {
				if (*nsrc == NUM_LINE_NAME)
					break;
			}
			if (*nsrc == '\0')
			{
				n = strlen(ln);
				if (n < ZC_NAME_LEN)
					snprintf(ln + n, ZC_NAME_LEN-n, "%s%s", LINE_NANE_CONCAT_2, loopp->line_name_post);
			}
		}

		/* Ersetzung der Laufkonstantennamen ausfuehren */
		for(n=0, nsrc=ln; *nsrc; n++, nsrc++) {
			s[n] = *nsrc;
			if(*nsrc==NUM_LINE_NAME) {
				nsrc++;
				n++;
				for(m=0; *nsrc && *nsrc!=NUM_LINE_NAME; m++, nsrc++) sn[m] = *nsrc;
				sn[m] = '\0';
                if(*sn>='1' && *sn<='4' && (m==1 || (sn[1]=='_' && m==2))) {
                    s[n++] = '_';
                    s[n++] = *sn;
					if(m==2) s[n++] = sn[1];
				}
				else if(m>0) {
					si = search_symbol(sn, &di);
					if(si > -2) {
						snprintf(glvar->errbuf, ERRBUFLEN, ERROR_LINE_SYMBOL, sn, ln);
						glvar->errbuf[ERRBUFLEN] = '\0';
						output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, glvar->errbuf);
						num = 0;
					}
					else num = -(si+2);
					sprintf(sn, "%d", num);
					for(snp=sn; *snp; n++, snp++) s[n] = *snp;
				}
				s[n] = NUM_LINE_NAME;
				if(! *nsrc) break;
			}
		}
		s[n] = '\0';
		if(n >= ZC_NAME_LEN) s[ZC_NAME_LEN-1] = '\0';
		strcpy(n_dest, s);
	}

	/* Testen Anzahl Variablen in einem Term */
	void Compiler::var_test_prod(int i)
	{
#ifdef VAR_PROD
		if(i > MAX_ANZ_VAR_PROD)
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_VARPROD_MAX);
#else
		if(i > 1)
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_VARPROD_1);
#endif
	}

	/* removes a tupel construction from the intermediary code and returns the symbols in it */
	int* Compiler::tupel_assign_symbols(int *cnt, int localrank)
	{
		int rank = localrank;
		val_str *p;

		if (rank == 0)
		{
			// Als letztes im Zwischencode muss CONSTRUCT_TUPEL stehen, davor literaler Listenzähler
			if (glvar->comp_ptr <= 2)
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "internal parse error");

			p = glvar->prog + (--(glvar->comp_ptr));
			if (p->t != TPP_CMD || p->v.c.cmd != CONSTRUCT_TUPEL)
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "internal parse error");

			p = glvar->prog + (--(glvar->comp_ptr));
			if (p->t != TPP_LIST_VAL)
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "internal parse error");

			rank = p->v.i;

			p = glvar->prog + (--(glvar->comp_ptr));
			if (p->t != TPP_CMD || p->v.c.cmd != LIT_VAL)
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "internal parse error");
		}
		
		// Block fuer Symbolnummern reservieren
		int *symblock = (int *) calloc(rank, sizeof(int));

		// Davor stehen FETCH, Anzahl entsprechend Rang (immer FETCH_CONST, auch für local, denn dann dürfen es nur undefinierte Symbole sein)
		// 	(Es sind generell nur einfache Elementsymbole erlaubt, keine indizierten Symbole und keine Subsymbole !)
		int symno;
		for (int i = rank - 1; i >= 0; i--)
		{
			p = glvar->prog + (--(glvar->comp_ptr));
			if (p->t != TP_INT)
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "lvalue of disaggregating tupel assignment must consist of simple symbols (A)");

			symno = p->v.i / MAX_ANZ_IND_1;

			p = glvar->prog + (--(glvar->comp_ptr));
			if (p->t != TPP_CMD || p->v.c.cmd != FETCH_CONST)
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "lvalue of disaggregating tupel assignment must consist of simple symbols (B)");

			// Wenn Symbol schon definiert ist, steht davor noch die Indexanzahl
			symbol_comp_str *symp = glvar->symbol_main_table.elem(glvar->symbol_main_table.search(symno));
			if (symp->isvalid())
			{
				p = glvar->prog + (--(glvar->comp_ptr));
				if (p->t != TP_INT || p->v.i != 0)
					output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "lvalue of disaggregating tupel assignment must consist of simple symbols (C)");

				p = glvar->prog + (--(glvar->comp_ptr));
				if (p->t != TPP_CMD || p->v.c.cmd != LIT_VAL)
					output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "lvalue of disaggregating tupel assignment must consist of simple symbols (D)");
			}

			symblock[i] = symno;
		}

		*cnt = rank;
		return symblock;
		
		// Vorgehen:
		// 	- Startposition vor diesen Einträgen im Zwischencode suchen
		// 	- dabei alle Symbolnummern und ob TMP merken
		// Weitere Funktion (nach Parsen rechter Seite)
		//  - Tupelzuweisung compilieren, verwendet alle gemerkten Symbole
		
		// Wo werden die Symbole definiert bzw. für bereits definierte die Typklasse geprüft?
		// 	(möglicherweise dafür noch zusätzlich Block mit den Symbolen zurückzugeben?)

		// Suche nach Symbolpointer: glvar->symbol_main_names.search(symno);
	}

	/* inserts intermediary code commands for storing values to symbols */
	t_addr Compiler::comp_symbols_assign(int *symblock, int cnt, bool freeblock)
	{
		t_addr old_ptr = glvar->comp_ptr;

		for (int i = 0; i < cnt; i++)
		{
			//(bisher nur fuer globale Symbole passend)
			int symind = glvar->symbol_main_table.search(symblock[i]);
			symbol_comp_str *symp = glvar->symbol_main_table.elem(symind);
			
			// Unterscheidung ob Symbol bereits definiert
			if (!(symp->isvalid()))
			{
				EXPSTR exp;
				exp.set(TPK_VALUE, 0);
				exp.set_sym(&(glvar->symbol_main_table), symind);
				init_symbol(&exp, PTR_TP_CONST, TPK_VALUE, 0, false);
			}
			else
			{
				test_tpk_symbol(symp, TPK_VALUE, false);
				if (symp->dim != 0)
					output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");
			}

			// Zuweisung fuer das Symbol compilieren
			comp_sub_par(LIT_VAL_SHORT, 0, 0);
			comp_sub_par(SET_INDEXES, IND_FREE, 0);
			comp_sub_par_cnt(DEF_CONST, SYM_MAIN, TPP_EMPTY, 1);
			comp_int(symp->symno * MAX_ANZ_IND_1 + symp->dim);
		}

		if (freeblock)
			free(symblock);

		return(old_ptr);
	}

	/* inserts a block of intermediary code commands to the intermediary code */
	/*
	t_addr Compiler::comp_insert_block(val_str *codeblock, int len, bool freeblock)
	{
		t_addr old_ptr = glvar->comp_ptr;
		val_str *p = glvar->prog + old_ptr;

		PROTO_OUT_3("\nCompile codeblock %p (len %d)", codeblock, len);

		glvar->comp_ptr += len;
		if(glvar->comp_ptr + 1 >= glvar->prog_max)
			glvar->incmem((void **) &(glvar->prog), &(glvar->prog_max), MAX_PROG, sizeof(val_str));

		memcpy(p, codeblock, sizeof(val_str)*len);

		if (freeblock)
			free(codeblock);

		return(old_ptr);
	}
	*/

	/* Gibt Warnung bzw. Fehlermeldung wegen veraltetem Zuweisungsoperator */
	void Compiler::out_warn_old_assign(bool err)
	{
  			//if (glvar->args->assignOldWarn() || err)
                  //      output_error_parse(ERROR_TYPE_COMP, (err ? ERROR_LVL_NORMAL : ERROR_LVL_WARN), ERROR_ASSIGN_OLD);
            if (glvar->args->assignOldWarn() || err)
                output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_ASSIGN_OLD);
	}

	/* Typableitung fuer Vektor */
	EXPSTR Compiler::parse_type_dev_array(EXPSTR *t1, EXPSTR *t2)
	{
		enum etp t1tp = t1->tp;
		enum etp t2tp = t2->tp;
		enum etp tkl = TYPECLASS(t1->tp);
		bool n1, n2, s1, s2;

		EXPSTR ret;
		ret.set(t1);

		if (t2tp == TPP_EMPTY || t1tp == t2tp)
			return ret;
			
		if (t1tp == TPP_EMPTY)
		{
			ret.set(t2);
			return ret;
		}



		/* Tupel/Tupelset gleichen Ranges gelten hier als miteinander kompatibel (strenge Typisierung dafür unvollständig) */
		if ((TP_IS_TUPEL(t1tp) || TP_IS_TUPELSET(t1tp)) && (TP_IS_TUPEL(t2tp) || TP_IS_TUPELSET(t2tp)) && RANK_TP_TUPEL(t1tp) == RANK_TP_TUPEL(t2tp))
		{
			if (TP_IS_TUPELSET(t2tp) && !TP_IS_TUPELSET(t1tp))
				ret.set(t2);
			return ret;
		}

		/* Typklasse muss gleich sein */
		if (t1tp == TPP_MIX_TPK || t2tp == TPP_MIX_TPK || tkl != TYPECLASS(t2tp))
		{
			ret.tp = TPP_MIX_TPK;
			return ret;
		}

		if (tkl == TPK_VALUE)
		{
			n1 = (t1tp == TP_INT || t1tp == TP_DBL || t1tp == TP_BIN || t1tp == TPU_NUM);
			n2 = (t2tp == TP_INT || t2tp == TP_DBL || t2tp == TP_BIN || t2tp == TPU_NUM);
			if (n1 && n2)
			{
				ret.tp = TPU_NUM;
				return ret;
			}

			s1 = (t1tp == TP_STR || t1tp == TP_ISTR);
			s2 = (t2tp == TP_STR || t2tp == TP_ISTR);
			if (s1 && s2)
			{
				ret.tp = TP_STR;
				return ret;
			}
		}

		ret.tp = tkl;
		return ret;
	}

	/* Typableitung fuer Auswahl aus zwei Werten */
	EXPSTR Compiler::parse_type_dev_switch(EXPSTR *t1, EXPSTR *t2)
	{
		return parse_type_dev_array(t1, t2);
	}

	/* Typableitung fuer nummerische Operation */
	EXPSTR Compiler::parse_type_dev_num_op(int sub, EXPSTR *t1, EXPSTR *t2)
	{
		enum etp t1tp = t1->tp;
		enum etp t2tp = t2->tp;
		bool n1, n2, s1, s2, no1, no2;

		EXPSTR ret;
		ret.set(t2);
		
		if ((sub == MULT || sub == INTERSECT_RR) && (TP_IS_TUPELSET(t1tp) || TP_IS_TUPEL(t1tp)))
		{
			// Tupelsetoperation, auch zweiter Operand muss Tupel oder Tupelset sein
			if (!TP_IS_TUPEL(t2tp) && !TP_IS_TUPELSET(t2tp))
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "tupelset expected");   //TODO

			// Rang des Ergebnistupelsets hier nicht bekannt, ersatzweise 1 verwenden
			ret.set(TPK_TUPELSET_N(1), 0);
			return ret;
		}

	    if (TP_IS_TUPEL(t1tp) || TP_IS_TUPEL(t2tp) || TP_IS_TUPELSET(t1tp) || TP_IS_TUPELSET(t2tp))
		{
			if ( sub == ADD && ( t1tp == TP_ISTR || t2tp == TP_ISTR || t1tp == TP_STR || t2tp == TP_STR) ) {
				ret.set( TP_ISTR , 0);
				return ret;
			}
			else  {
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "Command is not implemented for n-tupels with n>1 yet.");   //TODO
				return ret;
			}
		}
		
		
		if (t1->dim_t_undef > 0 || t2->dim_t_undef > 0)
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol in operation");   //TODO
			return ret;
		}

		if (t1tp == TPP_NULL)
		{
			if (TYPECLASS(t2tp) == TPK_VALUE || TYPECLASS(t2tp) == TPK_SET)
				return ret;

			t1tp = t2tp;
		}

		/* Operation ADD und MULT werden auch fuer sets verwendet */
		if (TYPECLASS(t1tp) == TPK_SET && TYPECLASS(t2tp) == TPK_SET && (sub == ADD || sub == MULT))
			return ret;

		/* Typklasse muss jedenfalls TPK_VALUE sein */
		if (TYPECLASS(t1tp) != TPK_VALUE || (sub != NEG && TYPECLASS(t2tp) != TPK_VALUE))
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
			ret.set(TPU_NUM, 0);
			return ret;
		}

		n1 = (t1tp == TP_INT || t1tp == TP_DBL || t1tp == TP_BIN || t1tp == TPU_NUM);
		n2 = (t2tp == TP_INT || t2tp == TP_DBL || t2tp == TP_BIN || t2tp == TPU_NUM);
		s1 = (t1tp == TP_STR || t1tp == TP_ISTR);
		s2 = (t2tp == TP_STR || t2tp == TP_ISTR);

		if (sub == ADD && (!n1 || !n2))
		{
			ret.set((s1 || s2 ? TP_ISTR : TPK_VALUE), 0);
			return ret;
		}

		no1 = (n1 || t1tp == TPK_VALUE);
		no2 = (n2 || t2tp == TPK_VALUE);

		if (!no1 || (sub != NEG && !no2))
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
			ret.set(TPU_NUM, 0);
			return ret;
		}

		if (sub == DIVI || sub == MODI)
			ret.set(TP_INT, 0);
		else if (sub == NEG)
			ret.set((t1tp == TP_BIN ? TP_INT : (n1 ? t1tp : TPU_NUM)), 0);
		else if (t1tp == TP_DBL || t2tp == TP_DBL)
			ret.set(TP_DBL, 0);
		else
			ret.set(TPU_NUM, 0);

		return ret;
	}

	/* Typableitung fuer Vergleichsoperation (eigentlich keine Typableitung, sondern nur Typpruefung) */
	EXPSTR Compiler::parse_type_dev_cmp_op(int sub, EXPSTR *t1, EXPSTR *t2)
	{
		enum etp t1tp = t1->tp;
		enum etp t2tp = t2->tp;

		EXPSTR ret;
		ret.set(TP_BIN, 0);

		if (t1->dim_t_undef > 0 || t2->dim_t_undef > 0)
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol in operation");   //TODO
			return ret;
		}

		if (sub == INSET)
		{
			/* Erstes muss einfacher Wert sein, zweites Set; oder erster Wert Tupel und zweiter gleichrangiger Tupelset */
			if ((TYPECLASS(t1tp) != TPK_VALUE || TYPECLASS(t2tp) != TPK_SET) &&
					(!TP_IS_TUPEL(t1tp) || !TP_IS_TUPELSET(t2tp) || RANK_TP_TUPEL(t1tp) != RANK_TP_TUPEL(t2tp)))
			{
				/* (Fehlermeldung passt nicht so recht, aber eigentlich sichert Parser schon richtige Typklassen) */
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CMP_TPK_MIX);
			}
		}
		else if ((TP_IS_TUPEL(t1tp) || TP_IS_TUPELSET(t1tp)) && (TP_IS_TUPEL(t2tp) || TP_IS_TUPELSET(t2tp)))
		{
			if (sub != EQ && sub != EQ2)
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "compare operation not implemented for tupels");
		}
		else
		{
			/* Typklasse muss gleich sein */
			if (TYPECLASS(t1tp) != TYPECLASS(t2tp))
			{
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CMP_TPK_MIX);
			}
		}

		/* Ergebnis ist jedenfalls binaer */
		return ret;
	}

	/* Typableitung fuer literale Set-Angabe (eigentlich keine Typableitung, sondern nur Typpruefung */
	EXPSTR Compiler::parse_type_dev_lit_set(enum etp settp, EXPSTR *t1, EXPSTR *t2, EXPSTR *t3)
	{
		enum etp t1tp = t1->tp;
		enum etp t2tp = t2->tp;
		enum etp t3tp = t3->tp;

		EXPSTR ret;
		ret.set(settp, 0);

		if (t1->dim_t_undef > 0 || t2->dim_t_undef > 0 || t3->dim_t_undef > 0)
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol in operation");   //TODO
			return ret;
		}

		/* Alles muss TPK_VALUE oder nicht verwendet (TPP_NULL) sein */
		if (TYPECLASS(t1tp) != TPK_VALUE || (t2tp != TPP_NULL && TYPECLASS(t2tp) != TPK_VALUE) || (t3tp != TPP_NULL && TYPECLASS(t3tp) != TPK_VALUE))
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CONST_DEF_SET_ELEM);
		}

		/* Ausser bei TP_SET_ENUM sind Strings nicht erlaubt */
		else if (settp != TP_SET_ENUM)
		{
			if (t2tp == TP_STR || t2tp == TP_ISTR || t3tp == TP_STR || t3tp == TP_ISTR)
			{
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
			}
		}

		return ret;
	}

	/* Typableitung fuer Subsymbol-Operator (holt eigentlich passendes Symbolelement oder fuegt leeres dafuer ein) */
	EXPSTR Compiler::parse_type_dev_subsym_op(EXPSTR *t, int subno)
	{
		EXPSTR ret;

		// t muss zu Typklasse mit Subsymbolen gehören
		if (t->symtab && !(t->symp()->isvalid()))
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "expression is not valid");	//TODO
		if (!(TP_WITH_SUBSYM(t->tp)) || t->tp_sub_tab < 0)
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "expression cannot have subsymbols");	//TODO

		// t muss ein skalarer Ausdruck sein
		if (t->dim_out != 0)
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "only a scalar expression can use subsymbols");   //TODO

		// t muss definiert sein
		if (t->dim_t_undef > 0)
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol cannot have subsymbols");   //TODO

		// In der Subsymboltabelle das Subsymbol mit der subno suchen; nötigenfalls noch einfügen
		SymbolTab *symtab = glvar->symbol_sub_tables.elem(t->tp_sub_tab);
		int i = symtab->search(subno);
		if (i < 0)
			i = symtab->insert(glvar, glvar->symbol_sub_names.elem(subno), subno, -1);

		ret.set_sym(symtab, i, i);
		if (TP_READONLY_SUB(t->tp))
			ret.readonly = true;

		return ret;
	}

	void Compiler::test_debug(EXPSTR *t, int i)
	{
		int a = 1;
	}

	/* Fehlerausgabe waehrend des Parsens */
	void Compiler::output_error_parse(int typ, int level, const char *text)
	{
		/* TODO: locp in glvar, fuer genaue Position */
		//TODO
		//struct yyguts_t * yyg = (struct yyguts_t*)(glvar->scanner);

		//output_error((io_type_str *)0, typ, level, (yyg ? yyg->yytext_r : (char *)0), text);
		fileIO->output_error((io_type_str *)0, typ, level, (char *)0, text);
	}


	/* 1.1.3 TS: Initialisierung fuer include */
	void Compiler::include_init()
	{
#ifdef CHDIR
		/* Aktuelles Verzeichnis merken, damit nach dem Parsen wieder dorthin gewechselt werden kann */
		glvar->dir_start_name = get_current_wd();

		/* Pfadseparator bestimmen: Als Pfadseparator kommen nur '/' und '\' in Frage, davor steht vielleicht noch ein Laufwerksbuchstabe */
		glvar->dir_sep_char = '/';
		if (!glvar->dir_start_name.empty())
		{
			int i;
			for (i = 0; i < 10 && glvar->dir_start_name[i] != '\0'; i++)
			{
				if (glvar->dir_start_name[i] == '/' || glvar->dir_start_name[i] == '\\')
				{
					glvar->dir_sep_char = glvar->dir_start_name[i];
					break;
				}
			}
		}

		/* Protokollausgaben */
		PROTO_OUT_2("\ncurrent start path: %s", (glvar->dir_start_name.empty() ? "(n.a.)" : glvar->dir_start_name.c_str()));
		PROTO_OUT_2("\npath separator char: %c", glvar->dir_sep_char);
#endif

		glvar->incl_stack_ptr = 0;
	}

#ifdef CHDIR
	/* 1.1.3 TS: Gibt das aktuelle Directory */
	const std::string Compiler::get_current_wd()
	{
		char buf[FILE_PATH_BUF_LEN+1];
		char *cp = (char *)0;

		if (getcwd(buf, FILE_PATH_BUF_LEN))
		{
			buf[FILE_PATH_BUF_LEN] = '\0';
			const std::string s(buf);
			return s;
		}
		else
		{
			output_error_parse(ERROR_TYPE_SONST, ERROR_LVL_FATAL, ERROR_PATH_TOO_LONG);
			const std::string s;
			return s;
		}
	}
#endif

	/* 1.1.3 TS: Setzt den Dateinamen als aktuelle Datei, fuers Parsen und im Zwischencode */
	void Compiler::include_set_filename(const char *filename)
	{
		/* Wenn kein Dateiname, dann ersatzweise als '()' darstellen: Sollte bei in der derzeitigen Nutzung nie vorkommen */
		if (!filename)
			filename = "()";

		glvar->akt_src_file = filename;
		comp(SRC_FILE, 1);
		comp_str(filename);
	}

	/* 1.1.3 TS: Datei auf den Stack fuer include */
	void Compiler::include_push(const char *full_filename)
	{
		//TODO: Location
		const char *path, *name;
		FILE *dir_fp;

		/* Test ob Stack ausreicht */
		if (glvar->incl_stack_ptr >= MAX_INCL_STACK)
			output_error_parse(ERROR_TYPE_SONST, ERROR_LVL_FATAL, ERROR_INCLUDE_MAX);

		/* Vollen Dateinamen in Pfad und Name ohne Pfad aufteilen (reserviert Speicher dafür; setzt im Pfad den Separator anstelle von '/') */
		name = split_path_name(full_filename, &path);

#ifdef CHDIR
		/* Wenn Pfad enthalten, dann dorthin wechseln, und merken */
		if (path != (char *)0)
		{
			/* Zu dem Pfad wechseln */
			if (chdir(path) != 0)
			{
				SPRINT_ERR_2(ERROR_FILE_INCL_DIR_NF, path);
				output_error_parse(ERROR_TYPE_IO, ERROR_LVL_FATAL, glvar->errbuf);
			}
			PROTO_OUT_3("\nStart parsing: '%s' (changed to dir '%s')", name, path);

			/* Nun aktuellen Pfad merken, damit spaeter dorthin zurueck gewechselt werden kann */
			glvar->incl_dir_name[glvar->incl_stack_ptr] = get_current_wd();
		}
		else
		{
			/* Andernfalls gilt das bisherige Directory weiter */
			PROTO_OUT_2("\nStart parsing: '%s' (current dir)", name);
			glvar->incl_dir_name[glvar->incl_stack_ptr] = (glvar->incl_stack_ptr == 0 ? glvar->dir_start_name : glvar->incl_dir_name[glvar->incl_stack_ptr - 1]);
		}
#else
		/* Pfad wird nicht weiter beachtet */
		PROTO_OUT_2("\nStart parsing: '%s'", name);
#endif

		/* Name merken */
		glvar->incl_file_name[glvar->incl_stack_ptr] = name;
		include_set_filename(name);

		glvar->incl_stack_ptr++;
	}

	/* 1.1.3 TS: Datei von Stack fuer include */
	void Compiler::include_pop()
	{
		//TODO: Location
		const std::string *dir_name;
		const char *nm;

		PROTO_OUT_2("\nEnd parsing: '%s'", glvar->akt_src_file);

		glvar->incl_stack_ptr--;
		if (glvar->incl_stack_ptr < 0)
			output_error_parse(ERROR_TYPE_SONST, ERROR_LVL_FATAL, ERROR_INTERN_INCL_STACK);

#ifdef CHDIR
		/* Noetigenfalls Directory zuruecksetzen */
		dir_name = (glvar->incl_stack_ptr == 0 ? &(glvar->dir_start_name) : &(glvar->incl_dir_name[glvar->incl_stack_ptr - 1]));
		if (glvar->incl_dir_name[glvar->incl_stack_ptr] != *dir_name)
		{
			if (chdir(dir_name->c_str()) != 0)
				output_error_parse(ERROR_TYPE_SONST, ERROR_LVL_FATAL, ERROR_INTERN_INCL_CHDIR);

			PROTO_OUT_2(" (back to dir '%s')", dir_name->c_str());
		}
#endif

		/* Wieder passenden Dateiname setzen */
		nm = (glvar->incl_stack_ptr == 0 ? (char *)0 : glvar->incl_file_name[glvar->incl_stack_ptr - 1].c_str());
		if (glvar->incl_stack_ptr > 0)
		{
			include_set_filename(nm);
			PROTO_OUT_2("\nContinue parsing: '%s'", glvar->akt_src_file);
		}
	}

	/* 1.1.3 TS: Vollen Dateinamen in Pfad und Name ohne Pfad aufteilen (reserviert Speicher dafuer) */
	char* Compiler::split_path_name(const char *full_filename, const char **path)
	{
		int i;
		const char *c;
		char *p, *nm;

		/* Hinterstes Vorkommen des Separators suchen */
#ifdef CHDIR
		c = strrchr(full_filename, (int) glvar->dir_sep_char);
#else
		c = strrchr(full_filename, (int) '/');
#endif
		if (c == (char *)0)
		{
			/* Keinen Pfad, nur Name im aktuellen Directory */
			*path = (char *)0;
			c = full_filename;
		}
		else
		{
			/* Pfad kopieren */
			i = (c - full_filename) + 1;
			p = (char *) (glvar->c_getmem(sizeof(char) * (i+1)));
			strncpy(p, full_filename, i);
			p[i] = '\0';

			*path = p;

			/* Dateiname ohne Pfad beginnt nach der Trennung */
			c++;
		}

		/* Dateiname ohne Pfad kopieren */
		nm = (char *) (glvar->c_getmem(sizeof(char) * (strlen(c)+1)));
		strcpy(nm, c);

		return nm;
	}
}

