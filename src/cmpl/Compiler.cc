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


#include "Compiler.hh"
#include "FileIO.hh"


namespace cmpl
{
	/* Konstruktor */
	Compiler::Compiler(Main *m)
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

		PROTO_OUT_2("  %lf", f);
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


	/* Suche Symbol (Variable, Konstante oder Laufkonstante) */
	int Compiler::search_symbol(char *str, int *ip)
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
			if(strncmp(str, symp->s_name, SYM_NAME_LEN)==0) {
				PROTO_OUT_2(" %d", n * MAX_ANZ_IND_1);
				*ip = n;
				return(n * MAX_ANZ_IND_1);
			}
		}

		PROTO_OUT_1(" not found");
		return(-1);
	}

	/* Einfuegen Symbol */
	int Compiler::insert_symbol(char *name, enum etp tpclass)
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
		strncpy(symp->s_name, name, SYM_NAME_LEN-1);
		symp->s_name[SYM_NAME_LEN-1] = '\0';
		symp->tp_sym = tpclass;

		/* Initial gibt es keinen Wert fuer das Symbol */
		symp->val_ip0 = (nonarray_str *)0;
		for(n=0; n<MAX_ANZ_IND; n++)
			symp->val_ip[n] = (array_str *)0;

		ret = (glvar->symbol_anzahl-1) * MAX_ANZ_IND_1;
		PROTO_OUT_2(" : %d", ret);
		return ret;
	}

	/* Setzt bzw. testet die Typklasse fuer ein Symbol */
	void Compiler::set_test_tpcl_symbol(int symnr, enum etp tpclass)
	{
		symbol_str *symp = glvar->symbol_table + (symnr / MAX_ANZ_IND_1);
		if (symp->tp_sym == tpclass)
			return;

		if (symp->tp_sym == TPP_EMPTY)
		{
			symp->tp_sym = tpclass;
			return;
		}

		/* Symbol ist fuer andere Typklasse definiert */
		SPRINT_ERR_2(ERROR_SYMBOL_TPK_MIX, symp->s_name);
		output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, glvar->errbuf);
	}

	/* Initialisiert einen loop-Kontext beim Compilieren, setzt den Zeilennamenprefix */
	void Compiler::init_loop_comp(unsigned short nr, bool base, enum looptp loop_tp, char *lname)
	{
		/* Test ob noch frei */
		if (nr > MAX_ANZ_TMP - 1)
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_FATAL, ERROR_LOOP_MAX);

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
				char *s = (nr == 0 ? (char *)0 : loopp[-1].line_name_pref);
				loopp->line_name_pref = (char *) glvar->c_getmem(sizeof(char) * ((s ? strlen(s) : 0) + strlen(lname) + strlen(LINE_NANE_CONCAT_1) + 1));
				if (s)
					strcat(strcpy(loopp->line_name_pref, s), LINE_NANE_CONCAT_1);
				else
					loopp->line_name_pref[0] = '\0';
				strcat(loopp->line_name_pref, lname);
			}
		}
		else if (nr > 0)
			loopp->line_name_pref = loopp[-1].line_name_pref;
	}

	/* Setzt neuen aktuellen loop-Kontext beim Compilieren, initialisiert den naechsten */
	loop_ctrl_str *Compiler::set_loop_comp(unsigned short sub, unsigned short mod, char *sname, bool inc)
	{
		loop_ctrl_str *loopp;

		if (inc)
			glvar->tmp_level++;

		loopp = glvar->tmp_table + (glvar->tmp_level - 1);
		loopp->mod = mod;
		loopp->valid = true;

		set_loop_sname(sub, sname);

		/* Naechsten loop-Kontext hier schon initialisieren */
		init_loop_comp(glvar->tmp_level, false, (glvar->tmp_level == 1 ? LOOP_EXEC : loopp[-1].loop_tp), (char *)0);

		return loopp;
	}

	/* Traegt Art und den Namen fuer die Laufkonstante bzw. die lokal definierte Konstante im loop-Kontext nach */
	void Compiler::set_loop_sname(unsigned short sub, char *sname)
	{
		loop_ctrl_str *loopp = loopp = glvar->tmp_table + (glvar->tmp_level - 1);
		loopp->sub = sub;
		loopp->jump_end = (sub != START_LOOP && loopp->mod == 0);
		loopp->s_name = sname;

		/* WHILE-Modifikator ist nicht mit START_LOOP erlaubt, und nicht in Summenschleife */
		if ((loopp->mod & LOOP_MOD_WHILE) && (sub == START_LOOP || loopp->loop_tp != LOOP_EXEC))
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_LOOP_WHILE);

		/* Zeilennamenpostfix setzen */
		if (sname)
		{
			if (glvar->tmp_level == 1 || !loopp[-1].line_name_post)
			{
				loopp->line_name_post = (char *) glvar->c_getmem(sizeof(char) * (strlen(sname) + 3));
				sprintf(loopp->line_name_post, "%c%s%c", NUM_LINE_NAME, sname, NUM_LINE_NAME);
			}
			else
			{
				char *s = (glvar->tmp_level == 1 ? (char *)0 : loopp[-1].line_name_post);
				loopp->line_name_post = (char *) glvar->c_getmem(sizeof(char) * ((s ? strlen(s) : 0) + strlen(sname) + strlen(LINE_NANE_CONCAT_3) + 3));
				if (s)
					strcat(strcpy(loopp->line_name_post, s), LINE_NANE_CONCAT_3);
				else
					loopp->line_name_post[0] = '\0';
				sprintf(loopp->line_name_post + strlen(loopp->line_name_post), "%c%s%c", NUM_LINE_NAME, sname, NUM_LINE_NAME);
			}
		}
		else if (glvar->tmp_level > 1)
			loopp->line_name_post = loopp[-1].line_name_post;
	}

	/* Setzt loop-Kontexte bis einschliesslich naechstem Basiskontext zurueck; compiliert entsprechende Beendigungen */
	void Compiler::unset_loop_comp()
	{
		loop_ctrl_str *loopp = glvar->tmp_table + (glvar->tmp_level - 1);
		t_addr a;

		/* Alle loops bis zur Basis beenden */
		while (1) {
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

			/* Naechsten loop beenden */
			glvar->tmp_level--;
			if (loopp->base || glvar->tmp_level==0) break;
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
					case LOOP_MOD_WHILE: s = "while"; break;
					default: s = "<intern>"; break;
				}
			}

			SPRINT_ERR_3(ERROR_LOOP_CTRL_REF, s, errstr);
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, glvar->errbuf);
			return (loop_ctrl_str *)0;
		}

		/* continue darf nur fuer loop-Kontext mit Iteration verwendet werden; oder mit WHILE-Modifikator */
		if (iscontinue && loopp->sub != START_LOOP && !(loopp->mod & LOOP_MOD_WHILE))
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_LOOP_ONCE_CONT);
			return (loop_ctrl_str *)0;
		}

		return loopp;
	}

	/* Erstellt Zeilennamen; ersetzen Laufkonstantennamen in Zeilennamen durch Laufkonstantennummer */
	void Compiler::linename_repl(char *n_dest, char *n_src)
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

	/* Gibt Warnung bzw. Fehlermeldung wegen veraltetem Zuweisungsoperator */
	void Compiler::out_warn_old_assign(bool err)
	{
		if (!glvar->aufruf_p->assign_old_no_warn || err)
			output_error_parse(ERROR_TYPE_COMP, (err ? ERROR_LVL_NORMAL : ERROR_LVL_WARN), ERROR_ASSIGN_OLD);
	}

	/* Typableitung fuer Vektor */
	enum etp Compiler::parse_type_dev_array(enum etp t1, enum etp t2)
	{
		enum etp tkl = TYPECLASS(t1);
		bool n1, n2, s1, s2;

		if (t2 == TPP_EMPTY || t1 == t2)
			return t1;

		/* Typklasse muss gleich sein */
		if (t1 == TPP_MIX_TPK || t2 == TPP_MIX_TPK || tkl != TYPECLASS(t2))
			return TPP_MIX_TPK;

		if (tkl == TPK_VALUE)
		{
			n1 = (t1 == TP_INT || t1 == TP_DBL || t1 == TP_BIN || t1 == TPU_NUM);
			n2 = (t2 == TP_INT || t2 == TP_DBL || t2 == TP_BIN || t2 == TPU_NUM);
			if (n1 && n2)
				return TPU_NUM;

			s1 = (t1 == TP_STR || t1 == TP_ISTR);
			s2 = (t2 == TP_STR || t2 == TP_ISTR);
			if (s1 && s2)
				return TP_STR;
		}

		return tkl;
	}

	/* Typableitung fuer Auswahl aus zwei Werten */
	enum etp Compiler::parse_type_dev_switch(enum etp t1, enum etp t2)
	{
		return parse_type_dev_array(t1, t2);
	}

	/* Typableitung fuer nummerische Operation */
	enum etp Compiler::parse_type_dev_num_op(int sub, enum etp t1, enum etp t2)
	{
		bool n1, n2, s1, s2, no1, no2;
		
		if (t1 == TPP_NULL)
		{
			if (TYPECLASS(t2) == TPK_VALUE || TYPECLASS(t2) == TPK_SET)
				return t2;

			t1 = t2;
		}

		/* Typklasse muss jedenfalls TPK_VALUE sein */
		if (TYPECLASS(t1) != TPK_VALUE || (sub != NEG && TYPECLASS(t2) != TPK_VALUE))
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
			return TPU_NUM;
		}

		n1 = (t1 == TP_INT || t1 == TP_DBL || t1 == TP_BIN || t1 == TPU_NUM);
		n2 = (t2 == TP_INT || t2 == TP_DBL || t2 == TP_BIN || t2 == TPU_NUM);
		s1 = (t1 == TP_STR || t1 == TP_ISTR);
		s2 = (t2 == TP_STR || t2 == TP_ISTR);

		if (sub == ADD && (!n1 || !n2))
			return (s1 || s2 ? TP_ISTR : TPK_VALUE);

		no1 = (n1 || t1 == TPK_VALUE);
		no2 = (n2 || t2 == TPK_VALUE);

		if (!no1 || (sub != NEG && !no2))
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
			return TPU_NUM;
		}

		if (sub == DIVI || sub == MODI)
			return TP_INT;

		if (sub == NEG)
			return (t1 == TP_BIN ? TP_INT : (n1 ? t1 : TPU_NUM));
		
		if (t1 == TP_DBL || t2 == TP_DBL)
			return TP_DBL;

		return TPU_NUM;
	}

	/* Typableitung fuer Vergleichsoperation (eigentlich keine Typableitung, sondern nur Typpruefung) */
	enum etp Compiler::parse_type_dev_cmp_op(int sub, enum etp t1, enum etp t2)
	{
		if (sub == INSET)
		{
			/* Erstes muss einfacher Wert sein, zweites Set */
			if (TYPECLASS(t1) != TPK_VALUE || TYPECLASS(t2) != TPK_SET)
			{
				/* (Fehlermeldung passt nicht so recht, aber eigentlich sichert Parser schon richtige Typklassen) */
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CMP_TPK_MIX);
			}
		}
		else
		{
			/* Typklasse muss gleich sein */
			if (TYPECLASS(t1) != TYPECLASS(t2))
			{
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CMP_TPK_MIX);
			}
		}

		/* Ergebnis ist jedenfalls binaer */
		return TP_BIN;
	}

	/* Typableitung fuer literale Set-Angabe (eigentlich keine Typableitung, sondern nur Typpruefung */
	enum etp Compiler::parse_type_dev_lit_set(enum etp settp, enum etp t1, enum etp t2, enum etp t3)
	{
		/* Alles muss TPK_VALUE oder nicht verwendet (TPP_NULL) sein */
		if (TYPECLASS(t1) != TPK_VALUE || (t2 != TPP_NULL && TYPECLASS(t2) != TPK_VALUE) || (t3 != TPP_NULL && TYPECLASS(t3) != TPK_VALUE))
		{
			output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CONST_DEF_SET_ELEM);
			return settp;
		}

		/* Ausser bei TP_SET_ENUM sind Strings nicht erlaubt */
		if (settp != TP_SET_ENUM)
		{
			if (t2 == TP_STR || t2 == TP_ISTR || t3 == TP_STR || t3 == TP_ISTR)
			{
				output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
				return settp;
			}
		}

		return settp;
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

