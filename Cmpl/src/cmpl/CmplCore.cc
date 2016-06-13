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


/*************************************************************************************
 *
 * Start der Ausfuehrung und Hauptfunktionen.
 *
 *************************************************************************************/

#define CMPL_CORE_CC

#include "CmplCore.hh"
#include "CmplData.hh"
#include "FileIO.hh"
#include "Compiler.hh"
#include "Interpreter.hh"

#include "IntFuncBase.hh"
#include "IntFuncCode.hh"
#include "IntFuncEnv.hh"
#include "IntFuncValue.hh"
#include "IntFuncDef.hh"
#include "IntFuncIndex.hh"
#include "IntFuncOper.hh"
#include "IntFuncCtrl.hh"
#include "IntFuncLine.hh"
#include "IntFuncTerm.hh"

#include <string>

namespace cmpl
{
	/* Konstruktor */
        CmplCore::CmplCore(cmplUtil::CmplArgs *arguments)
	{
		// Allgemeine Verweise setzen
		glvar = this;
		fileIO = NULL;
		interpreter = NULL;
		
		// this in anderen Objekten setzen
                setUtil.glvar = this;

                //cmplUtil::CmplArgs *arguments
                args=arguments;

		// seed for rand();
		srand( time(NULL) );

		int i, j;
#ifdef VAR_PROD
		i = MAX_ANZ_TMP+1;
#else
		i = MAX_ANZ_TMP;
#endif
		for (i--; i >= 0; i--)
			tmp_table[i].iter_set.glvar = this;

		for (i = 0; i < MAX_ANZ_VPROD; i++)
			term_ind_bl.iter_set[i].glvar = this;

		//for (i = 0; i < MAX_ANZ_IND; i++)
		//	index_info.i_ind[i].iter_set.glvar = this;

		for (j = 0; j < MAX_IND_INF; j++)
		{
			for (i = 0; i < MAX_ANZ_IND; i++)
				ind_inf_table[j].i_ind[i].iter_set.glvar = this;
		}

		line_vekt_bl.iter_set.glvar = this;

	}

        /* Destruktor */
        CmplCore::~CmplCore()
        {
            /* Funktionen zur Ausfuehrung des Zwischencodes wieder freigeben */
            //delete_code_table();

            /* Allen reservierten Speicher wieder freigeben */
            freemem_all();

        }



	/* Start der Ausfuehrung mit Auswertung der Parameter */
        int CmplCore::makeModell()
	{
		int ret = 1;

		/* Aufrufparameter-Struktur fuellen */
		aufruf_str aufruf_par;

        if (file_setting(&aufruf_par))
        {
			/* Start mit Aufrufparametern */
			ret = start_par(&aufruf_par);
		}

               free_aufruf_par(&aufruf_par);

		return ret;
	}

	/* Ausfuehrung fuer gegebene Aufrufparameter */
	int CmplCore::start_par(aufruf_str *aufruf_par)
	{
		FileIO output_gl(this);
		int ret = 0;
		jmp_buf env;

		/* Globale Werte setzen */
		fileIO = &output_gl;
		set_vals(aufruf_par);
		jmp_env = &env;

		/* Tabelle fuer Funktionen zur Ausfuehrung des Zwischencodes initialisieren */
        t_code_str code[CODE_TABLE_COUNT] = INIT_CODE_TABLE;
        code_table = code;

        if (setjmp(env) == 0)
		{
                    /* Dateien oeffnen entsprechend der Aufrufparameter */
                    fileIO->io_start_files();

                    /* open cmpl data files */
                    //TODO: if no data file, then don't init cdata and set cmplData to NULL
                    CmplData cdata(args);
                    if (!cdata.isOpenFiles())
                        fileIO->output_error((io_type_str *)0, ERROR_TYPE_IO, ERROR_LVL_FATAL, (char *)0, cdata.errorMsg().c_str());
                    cmplData = &cdata;

                    /* either the input file does not exists or the output file is not generated */
                    if (glvar->anz_error>0) {
                        ret=1;
                    } else {

                        /* Compiler (Erstellung Zwischencode) */
                        comp_main();

                        if (glvar->args->checkOnlySyntax()) {
                            cout << "CMPL syntax check has finished with " << anz_comp_error << " error(s)." << endl;
                        } else {
                            if (anz_comp_error > 0)
                                ret += 2;
                            else
                            {
                                /* Interpreter (Generieren Matrix) */
                                int_main();
                                if (anz_int_error > 0)
                                    ret += 4;
                            }
                        }
                    }
		}

		/* either the input file does not exists or the output file is not generated */
		if (glvar->anz_error>0)
			ret=1;

		/* Funktionen zur Ausfuehrung des Zwischencodes wieder freigeben */
		delete_code_table();

		/* Allen reservierten Speicher wieder freigeben */
		//freemem_all();


		/* Noch notwendige Ausgaben auf Dateien; Dateien schliessen */
        fileIO->io_out_string_files();

        fileIO = NULL;
		return ret;
	}



	/* Funktionen zur Ausfuehrung des Zwischencodes wieder freigeben */
	void CmplCore::delete_code_table()
	{
		int i;
		for (i = 0; i < CODE_TABLE_COUNT; i++)
		{
			if (code_table[i].exec_class)
			{
				delete code_table[i].exec_class;
				code_table[i].exec_class = NULL;
			}
        }
	}

	/* Speicher bereitstellen */
	void *CmplCore::c_getmem(size_t len)
	{
		void *c;

		PROTO_OUT_3("\nmemory allocation %u : %u", mem_alloc_akt, len);

        
        c = calloc(len, 1);
		ins_mem(c);
		return c;
	}

	/* Reservierten Speicher in Speicherverwaltung aufnehmen */
	void CmplCore::ins_mem(void *c)
	{
		PROTO_OUT_2("\nadd to memory allocation %u", mem_alloc_akt);

		/* Noetigenfalls erst noch Array fuer Speicherverwaltung bereitstellen */
		if (!mem_alloc || mem_alloc_max == 0)
		{
			mem_alloc = (void **) calloc(MAX_MEM, sizeof(void *));
			mem_alloc_max = MAX_MEM;
			mem_alloc_akt = 0;
		}
		else if (mem_alloc_akt >= mem_alloc_max)
		{
			/* Noetigenfalls neues groesseres Array fuer Speicherverwaltung bereitstellen */
			mem_alloc = (void **) realloc(mem_alloc, (mem_alloc_max + MAX_MEM) * sizeof(void *));
			mem_alloc_max += MAX_MEM;
		}

		if(!c) {
			fileIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL,
					(char *)0, ERROR_NO_MEMORY);
		}
		mem_alloc[mem_alloc_akt++] = c;
	}

	/* Vergroessert einen reservierten Speicherbereich */
	void CmplCore::incmem(void **mem_base, t_addr *mem_max, t_addr mem_inc, size_t size)
	{
		void *c = *mem_base;
		t_addr i, mem_cnt;
		bool found = false;

		/* Wenn bisher kein Speicherbereich, dann neu reservieren */
		if (!c)
		{
			*mem_base = c_getmem((*mem_max + mem_inc) * size);
			*mem_max += mem_inc;
			return;
		}

		/* In der Speicherverwaltung suchen */
		for (i = 0; i < mem_alloc_akt; i++)
		{
			if (mem_alloc[i] == c)
			{
				mem_cnt = i;
				found = true;
				break;
			}
		}

		PROTO_OUT_3("\nmemory reallocation %u : %u", mem_cnt, (*mem_max + mem_inc));

		/* Neuen Speicher reservieren und alten freigeben */
		*mem_base = realloc(*mem_base, (*mem_max + mem_inc) * size);
		*mem_max += mem_inc;

		/* Neuen Speicher eintragen */
		if (found)
			mem_alloc[mem_cnt] = *mem_base;
	}

	/* Speicher freigeben */
	void CmplCore::freemem(t_addr mem_alloc_cnt)
	{
		if (mem_alloc && mem_alloc_cnt < mem_alloc_akt)
		{
			void *c = mem_alloc[mem_alloc_cnt];
			free(c);
			mem_alloc[mem_alloc_cnt] = NULL;
			
			if (mem_alloc_cnt = mem_alloc_akt - 1)
				mem_alloc_akt--;
		}
	}

	/* Allen Speicher freigeben */
	void CmplCore::freemem_all()
	{
                //PROTO_OUT_2("\nfree all memory %d", mem_alloc_akt);

		if (mem_alloc)
		{
			t_addr i;
			for (i = 0; i < mem_alloc_akt; i++)
			{
				if (mem_alloc[i])
					free(mem_alloc[i]);
			}
			mem_alloc_akt = 0;

			/* Auch Array fuer Speicherverwaltung selbst freigeben */
			free(mem_alloc);
			mem_alloc = NULL;
			mem_alloc_max = 0;
		}
	}

	/* Runden double auf int, mit oder ohne Bereichspruefung */
	long CmplCore::rund(double x, bool fl)
	{
		if(fl && (x > LONG_MAX || x < LONG_MIN))
		{
			fileIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INT_MAX_MIN);
			return (x > LONG_MAX ? LONG_MAX : LONG_MIN);
		}
		return((long)(x + (x>0. ? 0.5 : -0.5)));
	}


	/* Sucht String in gespeicherten Strings und fuegt noetigenfalls dort ein; fuellt valp; zurueck ob str selbst verwendet */
	bool CmplCore::string_to_istr(val_str *valp, const char *str, bool copy)
	{
		bool found;

		valp->t = TP_ISTR;
		valp->v.i = strings.search_insert(this, str, copy, &found);

		return (!found && !copy);
	}


	/* Initialisiert allgemeine globalen Werte */
	void CmplCore::set_vals(aufruf_str *aufruf_par)
	{
		int i, j, n;

		/* Alles auf 0 setzen */
		init();

		aufruf_p = aufruf_par;

		/* Allgemeine Speicherreservierungen */
		prog = (val_str *) c_getmem(MAX_PROG * sizeof(val_str));
		prog_max = MAX_PROG;

		value_stack = (val_str *) c_getmem(MAX_LEN_STACK * sizeof(val_str));
		stack_max = MAX_LEN_STACK;

		//symbol_table = (symbol_str *) c_getmem(MAX_ANZ_SYMBOL * sizeof(symbol_str));
		//symbol_max = MAX_ANZ_SYMBOL;
	}

	/* Setzt alle Instanzvariablen auf 0 */
	void CmplCore::init()
	{
		aufruf_p = NULL;
		is_protokoll = false;

		jmp_env = NULL;

		prog = NULL;
		comp_ptr = 0;
		int_ptr = 0;
		prog_max = 0;
		
		mem_alloc = NULL;
		mem_alloc_akt = 0;
		mem_alloc_max = 0;

		incl_stack_ptr = 0;
#ifdef CHDIR
		dir_sep_char = '\0';
#endif

		akt_src_file = NULL;
		akt_src_line = 0;

		anz_comp_error = 0;
		anz_int_error = 0;
		*errbuf = '\0';
		*temp_buf = '\0';

		value_stack = NULL;
		t_addr stack_ptr = 0;
		t_addr stack_max = 0;
		
		var_dn_b.b_is_b = false;
		var_up_b.b_is_b = false;

		akt_defnum = 0;

		symbol_table = NULL;
		t_addr symbol_anzahl = 0;
		t_addr symbol_max = 0;
		symbol_akt_p = NULL;

		//strings.cap = strings.len = 0;

		tmp_level = 0;

		neg_fak_term = false;

		dest_grd = NULL;
		dest_akt = NULL;
		old_dest_akt = NULL;

		dest_names = NULL;

		akt_s_line = 0;
		mode_s_line = 0;
		*name_s_line = '\0';
		*org_name_s_line = '\0';
		is_left_side = false;
		akt_line_side = 0;
		beg_akt_line = 0;
		akt_line_obj_to = 0;

		*last_str = '\0';
		exp_transp = 0;
		//last_bef = 0;
		sect = 0;
		is_left = false;
		is_scann_const_vekt = false;

		gl_mline_vp = NULL;
		gl_mline_fp = NULL;
		gl_mline_fp_any = NULL;
		gl_mline_num = NULL;
		gl_mline_cap = 0;
		gl_mline_cnt_var = 0;
		gl_mline_cnt_not0 = 0;

		gl_mline_defnum = 0;
		gl_mline_varptr = NULL;

#ifdef VAR_PROD
		comp_end = 0;
		vp_sn_num = 0;

		var_split_str *var_split_grd = NULL;
		var_prod_str *var_prod_grd = NULL;

		pvarb_s_line = 0;
		*name_pvarb_line = '\0';
#endif

	}

	/* Hauptfunktion Compiler: Parst die Input-Datei und erzeugt Zwischencode */
	void CmplCore::comp_main()
	{
		PROTO_OUT_1("\ncompiler start");

		Compiler comp(this);
		comp.parse();

		PROTO_OUT_2("\ncompiler end (%d errors)", anz_comp_error);
	}


	/* Hauptfunktion Interpreter: Fuehrt den Zwischencode aus */
	void CmplCore::int_main()
	{
		PROTO_OUT_1("\ninterpreter start");

		Interpreter intpr(this);
		interpreter = &intpr;
		
		// Vorbereitung Symboltabellen fuer Interpreter
		symbol_main_table.init_symbol_int(this);

		// Interpreter ausfuehren
		intpr.int_prog();

		interpreter = NULL;

		PROTO_OUT_2("\ninterpreter end (%d errors)", anz_int_error);
	}




        bool CmplCore::file_setting(aufruf_str *aufruf_par)
        {
                io_type_str *ios;
                init_aufruf_str(aufruf_par);

                //input file handling
                ios = &(aufruf_par->input);
                ios->io_type = IO_TYPE_FILE;
                ios->filename = const_cast<char*>(args->cmplFile().c_str());


                //error file handling
                if (args->errorOut()) {
                    ios = aufruf_par->output + OUTPUT_ERROR;
                    ios->io_type =  IO_TYPE_FILE;
                    if (args->errorFile().empty())
                        ios->file_fp = stdout;
                    else {
                        ios->io_type = IO_TYPE_STR_FILE;
                        ios->filename = const_cast<char*>(args->errorFile().c_str());
                    }
                }

                //protocoll file handling
                if (args->protocolOut()) {
                    ios = aufruf_par->output + OUTPUT_PROTOKOLL;
                    ios->io_type =  IO_TYPE_FILE;
                    if (args->protocolFile().empty())
                        ios->file_fp = stdout;
                    else
                        ios->filename = const_cast<char*>( args->protocolFile().c_str());
                }


                /* Pruefen und fehlende Standardwerte setzen */
                return fill_aufruf_str(aufruf_par);
        }


	/* Initialisiert die Aufrufparameterstruktur mit Standardwerten */
	void CmplCore::init_aufruf_str(aufruf_str *aufruf_par)
	{
		/* Ein-/Ausgaben */
		int n;
		io_type_str *ios;
                for(n = -1; n <= OUTPUT_PROTOKOLL; n++)  /* Stegger 1.2.3 */
		{
			ios = (n == -1 ? &(aufruf_par->input) : (aufruf_par->output + n));
			ios->io_type = IO_TYPE_NO;
			ios->filename = (char *)0;
			ios->file_fp = (FILE *)0;
			ios->io_string = (char *)0;
			ios->out_str = (void *)0;
			ios->string_size = 0;
			ios->string_mem = 0;
			ios->mem_filename = false;
			ios->file_open = false;
		}
	}

	/* Aufrufparameter mit Standardwerten vervollstaendigen */
	bool CmplCore::fill_aufruf_str(aufruf_str *aufruf_par)
	{
                io_type_str *ios;

                /* Wenn fuer Fehlerausgabe nichts angegeben, auf stderr ausgeben */
                ios = aufruf_par->output + OUTPUT_ERROR;
                if (ios->io_type == IO_TYPE_NO)
                {
                    ios->io_type = IO_TYPE_FILE;
                    ios->file_fp = stderr;
                }

		return true;
	}

	/* Gibt Speicher frei, der bei Fuellen der Aufrufparameterstruktur reserviert wurde */
	void CmplCore::free_aufruf_par(aufruf_str *aufruf_par)
	{
		int n;
		io_type_str *ios;
                for(n = -1; n <= OUTPUT_PROTOKOLL; n++) /* stegger 1.2.3 */
		{
			ios = (n == -1 ? &(aufruf_par->input) : (aufruf_par->output + n));
			if (ios->mem_filename)
			{
                //free(ios->filename);
				ios->filename = (char *)0;
				ios->mem_filename = false;
			}
			if ((ios->io_string || ios->out_str) && ios->string_mem > 0)
			{
				if (ios->io_string)
					free(ios->io_string);
				else
					free(ios->out_str);

				ios->io_string = (char *)0;
				ios->out_str = (void *)0;
				ios->string_size = 0;
				ios->string_mem = 0;
			}
		}
	}



        /* Sucht das Element mit der angegebenen Symbolnummer; zurück Indexnummer oder -1 falls nicht gefunden */
        int SymbolTab::search(int symno)
        {
                //TODO: zusätzliches int-Array mit sortierten Symbolnummern für schnellere Suche
                int i;
                symbol_comp_str *symp;

                for (i=0, symp=symbols; i < len; i++, symp++)
                {
                        if (symp->symno == symno)
                                return i;
                }

                return -1;
        }

        /* Fügt neuen leeren Symboleintrag in die Symboltabelle ein; am Index i oder wenn i == -1 beim nächsten freien Index; zurück verwendeten Index */
        int SymbolTab::insert(CmplCore *glvar, const char *name, int symno, int i)
        {
                if (i < 0)
                        i = len;

                while (i >= cap)
                {
                        // Array vergroessern
                        t_addr old_cap = cap;
                        t_addr cap_inc = (MIN_CAP_ARRAY >= MIN_CAP_GROW * old_cap ? MIN_CAP_ARRAY : (int)(MIN_CAP_GROW * old_cap));

                        PROTO_OUT_3("\n    grow symbol table array from %d by %d", cap, cap_inc);

                        glvar->incmem((void **) &symbols, &old_cap, cap_inc, sizeof(symbol_comp_str));
                        cap = old_cap;
                }

                if (i >= len)
                        len = i + 1;

                symbol_comp_str *symp = symbols + i;
                symp->init(name);
                symp->symno = symno;

                return i;
        }

        /* Vorbereitung Symboltabellen fuer Interpreter */
        void SymbolTab::init_symbol_int(CmplCore *glvar)
        {
                int i, n;
                symbol_str *symi;
                symbol_comp_str *symc;

                PROTO_OUT_2("\nsymbol count: %d", len);

                // Symboltabelle fuer Interpreter aus der fuer Compiler initialisieren
                glvar->symbol_anzahl = glvar->symbol_max = len;
                glvar->symbol_table = (symbol_str *) (glvar->c_getmem(sizeof(symbol_str) * glvar->symbol_anzahl));

                for (i=0, symi=glvar->symbol_table, symc=symbols; i < len; i++, symi++, symc++)
                {
                        if (symc->symtp != PTR_TP_EMPTY && symc->valid && !(symc->local))
                        {
                                symi->s_name = symc->s_name;
                                symi->tp_sym = symc->tpk;
                                symi->dim = symc->dim;

                                if (TP_WITH_SUBSYM(symc->tpk))
                                        symi->sub_symtab = glvar->symbol_sub_tables.elem(symc->tp_sub_tab);

                        symi->val_ip0 = (nonarray_str *)0;
                        for(n=0; n<MAX_ANZ_IND; n++)
                            symi->val_ip[n] = (array_str *)0;
                        }
                }
        }




	/* Fügt neue leere Subsymboltabelle ein; zurück verwendeten Index */
        int SubSymbolTabs::insert(CmplCore *glvar)
	{
		int i = len;

		while (i >= cap)
		{
			// Array vergroessern
			t_addr old_cap = cap;
			t_addr cap_inc = (MIN_CAP_ARRAY >= MIN_CAP_GROW * old_cap ? MIN_CAP_ARRAY : (int)(MIN_CAP_GROW * old_cap));

			PROTO_OUT_3("\n    grow subsymbol tables array from %d by %d", cap, cap_inc);

			glvar->incmem((void **) &symbolTabs, &old_cap, cap_inc, sizeof(SymbolTab));
			cap = old_cap;
		}

		if (i >= len)
			len = i + 1;

		symbolTabs[i].init();

		return i;
	}


	/* Sucht String in String-Tabelle und fuegt noetigenfalls dort ein; zurueck Nummer des Strings */
        int StringTab::search_insert(CmplCore *glvar, const char *str, bool copy, bool *found)
	{
		int i, iu, io, comp;
		const char *s;

		/* Suche nach dem String */
		iu = i = 0;
		io = len - 1;

		while (iu <= io)
		{
			i = (io + iu) / 2;
			comp = strcmp(str, strings_sort[i]);
			if (comp == 0)
			{
				*found = true;
				return index[i];
			}

			if (comp < 0)
				io = i - 1;
			else
				iu = i + 1;
		}

		/* String nicht gefunden, muss bei iu eingefuegt werden */
		if (len == cap)
		{
			/* Arrays vergroessern */
			t_addr old_cap = cap;
			t_addr cap_inc = (MIN_CAP_ARRAY >= MIN_CAP_GROW * old_cap ? MIN_CAP_ARRAY : (int)(MIN_CAP_GROW * old_cap));

			PROTO_OUT_3("\n    grow strings array from %d by %d", old_cap, cap_inc);

			glvar->incmem((void **) &(strings_ind), &(old_cap), cap_inc, sizeof(char *));
			old_cap = cap;
			glvar->incmem((void **) &(strings_sort), &(old_cap), cap_inc, sizeof(char *));
			old_cap = cap;
			glvar->incmem((void **) &(index), &(old_cap), cap_inc, sizeof(int));
			cap = old_cap;
		}

		/* Noetigenfalls verschieben */
		if (iu < len)
		{
			memmove(strings_sort + iu + 1, strings_sort + iu, sizeof(char *) * (len - iu));
			memmove(index + iu + 1, index + iu, sizeof(int) * (len - iu));
		}

		/* String wird noetigenfalls kopiert */
		if (copy)
		{
			char *s1 = (char *) (glvar->c_getmem(sizeof(char) * (strlen(str) + 1)));
			strcpy(s1, str);
			s = s1;
		}
		else
		{
			s = str;
		}

		PROTO_OUT_3("\n    insert string '%s' as #%d", s, len);

		/* Einfuegen */
		strings_ind[len] = s;
		strings_sort[iu] = s;
		index[iu] = len++;

		*found = false;
		return (len-1);
	}


	/* Tiefe Kopie von Wert mit Subsymbolen machen */
        ValWithSubSyms* ValWithSubSyms::copy(CmplCore *glvar)
	{
		ValWithSubSyms *p = (ValWithSubSyms *) glvar->c_getmem(sizeof(ValWithSubSyms));
		p->symtab = symtab;
		p->symvalCopy(glvar, symval);
		return p;
	}

	/* Symbolwerttabelle initialisieren */
        void ValWithSubSyms::symvalInit(CmplCore *glvar)
	{
		if (!symtab)
		{
			symval = NULL;
			return;
		}

		int n, d, cnt = symtab->count();
		symbol_str *s_p = (symbol_str *) glvar->c_getmem(cnt * sizeof(symbol_str));
		symbol_comp_str *symp;

		symval = s_p;
		for (n = 0; n < cnt; n++, s_p++)
		{
			symp = symtab->elem(n);
			s_p->s_name = symp->s_name;
			s_p->tp_sym = symp->tpk;
			s_p->dim = symp->dim;

			if (TP_WITH_SUBSYM(symp->tpk))
				s_p->sub_symtab = glvar->symbol_sub_tables.elem(symp->tp_sub_tab);

			s_p->val_ip0 = (nonarray_str *)0;
			for(d=0; d<MAX_ANZ_IND; d++)
				s_p->val_ip[d] = (array_str *)0;
		}
	}

	/* Tiefe Kopie von Symbolwerttabelle machen */
        void ValWithSubSyms::symvalCopy(CmplCore *glvar, symbol_str *src)
	{
		if (!src)
		{
			symval = NULL;
			return;
		}

		symvalInit(glvar);			

		//TODO: Hilfsweise wird hier erstmal flache Kopie gemacht; sollte aber tiefe sein
		int n, d, cnt = symtab->count();
		symbol_str *dst = symval;

		for (n = 0; n < cnt; n++, src++, dst++)
		{
			dst->val_ip0 = src->val_ip0;
			for(d=0; d<MAX_ANZ_IND; d++)
				dst->val_ip[d] = src->val_ip[d];
		}
	}

	/* Tiefe Kopie von cmpl-Problem machen */
        ValWithSubSyms* CMPLProblemMain::copy(CmplCore *glvar)
	{
		CMPLProblemMain *p = (CMPLProblemMain *) glvar->c_getmem(sizeof(CMPLProblemMain));

		p->symtab = symtab;
		p->symvalCopy(glvar, symval);
		p->filename = filename;
		p->status = PROBLEM_STAT_NEW;	// Optimierungsergebnis wird nicht kopiert; neues Problem gilt immer als noch ungelöst

		return p;
	}


}


