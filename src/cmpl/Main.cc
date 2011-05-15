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


/*************************************************************************************
 *
 * Start der Ausfuehrung und Hauptfunktionen.
 *
 *************************************************************************************/

#define CMPL_MAIN_CC

#include "Main.hh"
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

#ifdef SOLVERSERVICE
#include "ecmpl.hh"
#endif

namespace cmpl
{
	/* Konstruktor */
	Main::Main()
	{
		// Allgemeine Verweise setzen
		glvar = this;
		fileIO = NULL;
		interpreter = NULL;
		
		// this in anderen Objekten setzen
		setUtil.glvar = this;
		
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

		for (i = 0; i < MAX_ANZ_IND; i++)
			index_info.i_ind[i].iter_set.glvar = this;

		for (j = 0; j < MAX_IND_INF; j++)
		{
			for (i = 0; i < MAX_ANZ_IND; i++)
				ind_inf_table[j].i_ind[i].iter_set.glvar = this;
		}

		line_vekt_bl.iter_set.glvar = this;

	}

//	/* Destruktor */
//	Main::~Main()
//	{
//	}

	/* Start der Ausfuehrung mit Auswertung der Parameter */
	int Main::start_args(int argc, char *argv[])
	{
		int ret = 1;

		/* Aufrufparameter-Struktur fuellen */
		aufruf_str aufruf_par;

		if (parse_args(&aufruf_par, argc, argv))
                {
#ifdef SOLVERSERVICE
                    if (aufruf_par.solve && aufruf_par.outStdIO)
                        cout << MSG_CMPL_STARTED << endl;
#endif
			/* Start mit Aufrufparametern */
			ret = start_par(&aufruf_par);
		}

#ifdef SOLVERSERVICE

                if (ret!=0  && aufruf_par.solve && aufruf_par.outStdIO)
                    cout << MSG_CMPL_FAILED <<endl;
                if (ret==0  && aufruf_par.solve && aufruf_par.outStdIO)
                    cout << MSG_CMPL_FINISHED <<endl;

                if (ret==0 && aufruf_par.solve) {
                    if (aufruf_p->outStdIO)
                        cout << MSG_OSS_STARTED << endl;

                    eCmpl eCh;
                    eCh.setProblem( aufruf_p->input.filename );

                    if (aufruf_p->outStdIO)
                        eCh.setSolverOutputStd(true);

                    if (aufruf_p->outCsv)
                        eCh.setSolverOutputCsv(true);

                    if (aufruf_p->solver.length()!=0)
                        eCh.setSolver(aufruf_p->solver);

                    if (aufruf_p->solverService.length()!=0)
                        eCh.setServiceLocation(aufruf_p->solverService);

                    if (aufruf_p->osolFile.length()!=0)
                        eCh.setOsol(aufruf_p->osolFile);

                    eCh.setSilent(true);

                    ret = eCh.runOSSolverServices();

                    if(ret!=1) {
                        if (aufruf_p->outStdIO)
                            cout << MSG_OSS_FAILED <<endl;
                        ret=6;
                    }
                    if (ret==1) {
                        if (aufruf_par.outStdIO)
                            cout << MSG_OSS_FINISHED <<endl;
                        ret=0;
                    }


                }
#endif


		free_aufruf_par(&aufruf_par);

		return ret;
	}

	/* Ausfuehrung fuer gegebene Aufrufparameter */
	int Main::start_par(aufruf_str *aufruf_par)
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

                        /* either the input file does not exists or the output file is not generated */
                        if (glvar->anz_error>0) {
                            ret=1;
                        } else {

                            /* Compiler (Erstellung Zwischencode) */
                            comp_main();
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


                /* either the input file does not exists or the output file is not generated */
                if (glvar->anz_error>0)
                    ret=1;

                /* Funktionen zur Ausfuehrung des Zwischencodes wieder freigeben */
		delete_code_table();
		
		/* Allen reservierten Speicher wieder freigeben */
		freemem_all();

		/* Noch notwendige Ausgaben auf Dateien; Dateien schliessen */
		fileIO->io_out_string_files();

		fileIO = NULL;
		return ret;
	}

	/* Funktionen zur Ausfuehrung des Zwischencodes wieder freigeben */
	void Main::delete_code_table()
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
	void *Main::c_getmem(size_t len)
	{
		void *c;

		PROTO_OUT_3("\nmemory allocation %u : %u", mem_alloc_akt, len);

                //c = calloc(len+1, 1);
                c = calloc(len, 1);
		ins_mem(c);
		return c;
	}

	/* Reservierten Speicher in Speicherverwaltung aufnehmen */
	void Main::ins_mem(void *c)
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
	void Main::incmem(void **mem_base, t_addr *mem_max, t_addr mem_inc, size_t size)
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
	void Main::freemem(t_addr mem_alloc_cnt)
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
	void Main::freemem_all()
	{
		PROTO_OUT_2("\nfree all memory %d", mem_alloc_akt);

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
	long Main::rund(double x, bool fl)
	{
		if(fl && (x > LONG_MAX || x < LONG_MIN))
		{
			fileIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INT_MAX_MIN);
			return (x > LONG_MAX ? LONG_MAX : LONG_MIN);
		}
		return((long)(x + (x>0. ? 0.5 : -0.5)));
	}


	/* Sucht String in gespeicherten Strings und fuegt noetigenfalls dort ein; fuellt valp; zurueck ob str selbst verwendet */
	bool Main::string_to_istr(val_str *valp, const char *str, bool copy)
	{
		int i, iu, io, comp;
		const char *s;

		valp->t = TP_ISTR;

		/* Suche nach dem String */
		iu = i = 0;
		io = strings.len - 1;

		while (iu <= io)
		{
			i = (io + iu) / 2;
			comp = strcmp(str, strings.strings_sort[i]);
			if (comp == 0)
			{
				valp->v.i = strings.index[i];
				return false;
			}

			if (comp < 0)
				io = i - 1;
			else
				iu = i + 1;
		}

		/* String nicht gefunden, muss bei iu eingefuegt werden */
		if (strings.len == strings.cap)
		{
			/* Arrays vergroessern */
			t_addr old_cap = strings.cap;
			t_addr cap_inc = (MIN_CAP_ARRAY >= MIN_CAP_GROW * old_cap ? MIN_CAP_ARRAY : (int)(MIN_CAP_GROW * old_cap));

			PROTO_OUT_3("\n    grow strings array from %d by %d", old_cap, cap_inc);

			incmem((void **) &(strings.strings_ind), &(old_cap), cap_inc, sizeof(char *));
			old_cap = strings.cap;
			incmem((void **) &(strings.strings_sort), &(old_cap), cap_inc, sizeof(char *));
			old_cap = strings.cap;
			incmem((void **) &(strings.index), &(old_cap), cap_inc, sizeof(int));
			strings.cap = old_cap;
		}

		/* Noetigenfalls verschieben */
		if (iu < strings.len)
		{
			memmove(strings.strings_sort + iu + 1, strings.strings_sort + iu, sizeof(char *) * (strings.len - iu));
			memmove(strings.index + iu + 1, strings.index + iu, sizeof(int) * (strings.len - iu));
		}

		/* String wird noetigenfalls kopiert */
		if (copy)
		{
			char *s1 = (char *) (c_getmem(sizeof(char) * (strlen(str) + 1)));
			strcpy(s1, str);
			s = s1;
		}
		else
		{
			s = str;
		}

		/* Einfuegen */
		strings.strings_ind[strings.len] = s;
		strings.strings_sort[iu] = s;
		strings.index[iu] = strings.len;
		valp->v.i = strings.len;

		strings.len++;
		return (!copy);
	}


	/* Initialisiert allgemeine globalen Werte */
	void Main::set_vals(aufruf_str *aufruf_par)
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

		symbol_table = (symbol_str *) c_getmem(MAX_ANZ_SYMBOL * sizeof(symbol_str));
		symbol_max = MAX_ANZ_SYMBOL;
	}

	/* Setzt alle Instanzvariablen auf 0 */
	void Main::init()
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

		strings.cap = strings.len = 0;

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
		last_bef = 0;
		sect = 0;
		is_left = false;
		is_scann_const_vekt = false;

		gl_mline_vp = NULL;
		gl_mline_fp = NULL;
		gl_mline_anz = 0;

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
	void Main::comp_main()
	{
		PROTO_OUT_1("\ncompiler start");

		Compiler comp(this);
		comp.parse();

		PROTO_OUT_2("\ncompiler end (%d errors)", anz_comp_error);
	}


	/* Hauptfunktion Interpreter: Fuehrt den Zwischencode aus */
	void Main::int_main()
	{
		PROTO_OUT_1("\ninterpreter start");

		Interpreter intpr(this);
		interpreter = &intpr;

		intpr.int_prog();

		interpreter = NULL;

		PROTO_OUT_2("\ninterpreter end (%d errors)", anz_int_error);
	}




        // alternative args handling if OSSOLverservices are linked to CMPL
        /* Parst Aufrufparameter (wie an main() uebergeben) und fuellt damit Aufrufparameterstruktur */
        bool Main::parse_args(aufruf_str *aufruf_par, int argc, char *argv[])
        {



/* Definitions for the different args handling */

#ifdef SOLVERSERVICE
                QStringList cArgs=QCoreApplication::arguments() ;
                QString cArg;

                #define ARGS_LENGTH             cArgs.length()
                #define CONV_ARG                cArg=cArgs[i];
                #define ARG_STARTSWITH(s)       cArg.startsWith(s)
                #define PARAMETER_CONTENT       ( ( (i+1)>= cArgs.length() ) ? false : ( !cArgs[i+1].startsWith("-") ) )
                #define OUTPUT_PARAMETER        (cArgs[i].startsWith("-") && cArgs[i].length()==2)
                #define INPUT_FILE              ( (i==1) || (i==ARGS_LENGTH-1) || (!cArgs[i-1].startsWith("-")) )
#else
                std::string bArg;
                std::string cArg;
                std::string nArg;

                #define ARGS_LENGTH             argc
                #define CONV_ARG                cArg=argv[i]; bArg=argv[i-1]; if(i<argc-1) nArg=argv[i+1]
                #define ARG_STARTSWITH(s)       ( (cArg.substr(0, std::string(s).size())) == std::string(s) )
                #define PARAMETER_CONTENT       (i+1>=ARGS_LENGTH ? false :   !(nArg.substr(0, 1) == "-" ) )
                #define OUTPUT_PARAMETER        ( (cArg.substr(0, 1) == "-") && (cArg.size()==2))
                #define INPUT_FILE              ( (i==1) || (i==ARGS_LENGTH-1) || !(bArg.substr(0, 1) == "-" )  )
#endif
                #define ARG                     cArg

                int n, m;
                io_type_str *ios;

                init_aufruf_str(aufruf_par);


                if ((ARGS_LENGTH==1)) {
                      aufruf_par->solve=false;
                      return usage(argv[0], (char *)0, (char *)0);
                  }

                for ( int i = 1; i < ARGS_LENGTH; i++ ) {

                    CONV_ARG;

                    if (ARG =="-h" || ARG =="--help") {
                         aufruf_par->solve=false;
                         return usage(argv[0], (char *)0, (char *)0);
                     }

                    if (ARG =="-v" || ARG =="--version") {
                        aufruf_par->solve=false;
                        printf(USAGE_VERSION_OUT, VERSION);
                        return false;
                    }

                    /* Kennzeichen fuer Generierung */
                    if ( ARG_STARTSWITH("-g") )
                    {
                        if (ARG == "-gn")
                            aufruf_par->is_no_reduct = true;
#ifdef VAR_PROD
                        else if (ARG == "-gf")
                            aufruf_par->is_var_prod_follow = true;
#endif
                        else
                            return usage(argv[0], argv[i], USAGE_ERROR_GEN);
                        continue;
                    }


                    /* Kennzeichen fuer Berechnungen waehrend der Generierung */
                    if (ARG_STARTSWITH("-c") )
                    {
                        if (ARG_STARTSWITH("-ci"))
                            sscanf(argv[i]+3, "%d", &(aufruf_par->num_op_mode));
                        else if (ARG == "-cd")
                            aufruf_par->const_def_no_warn = true;
                        else if (ARG == "-ca")
                            aufruf_par->assign_old_no_warn = true;
                        else
                            return usage(argv[0], argv[i], USAGE_ERROR_CONST);
                        continue;
                    }

                    /* Kennzeichen fuer Ausgabedatei */
                    if (ARG_STARTSWITH("-f") )
                    {
                        if (ARG == "-fc")
                            sscanf(argv[i]+3, "%d", &(aufruf_par->comment_len));

                        else if (ARG == "-ff")
                            aufruf_par->matrix_out_freemps = true;

                        else if ( ARG_STARTSWITH("-f%")	)	/* 1.1.2 TS: inserted */
                        {
                            strncpy(aufruf_par->double_number_format, argv[i]+2, 10);
                            aufruf_par->double_number_format[10] = '\0';
                        }
                        else
                            return usage(argv[0], argv[i], USAGE_ERROR_FRM);

                        continue;
                    }


                    /* Eingabedatei */
                    if (ARG == "-i")
                    {
                        if ( !PARAMETER_CONTENT )
                            return usage(argv[0], argv[i], USAGE_ERROR_INP_NO );

                        ios = &(aufruf_par->input);
                        if (ios->io_type != IO_TYPE_NO)
                            return usage(argv[0], argv[i], USAGE_ERROR_INP_MULT);

                        ios->io_type = IO_TYPE_FILE;
                        ios->filename = argv[i+1];
                        i++;

                        continue;
                    }

#ifdef SOLVERSERVICE

                    if (ARG=="-noOutput") {
                        aufruf_par->noOutput=true;
                        aufruf_par->model=true;
                        aufruf_par->solve=false;
                        continue;
                    }

                    if (ARG=="-solver") {
                        if ( !PARAMETER_CONTENT  )
                            return usage(argv[0], argv[i], USAGE_ERROR_ARG_NO );
                        aufruf_par->solve=true;
                        aufruf_par->solver=cArgs[i+1].toStdString();
                        i++;
                        continue;
                    }

                    if (ARG=="-solverUrl") {
                        if ( !PARAMETER_CONTENT  )
                            return usage(argv[0], argv[i], USAGE_ERROR_ARG_NO );
                        aufruf_par->solve=true;
                        aufruf_par->solverService=cArgs[i+1].toStdString();
                        i++;
                        continue;
                    }

                    if (ARG=="-osol") {
                        if ( !PARAMETER_CONTENT  )
                            return usage(argv[0], argv[i], USAGE_ERROR_ARG_NO );
                        aufruf_par->solve=true;
                        aufruf_par->osolFile=cArgs[i+1].toStdString();
                        i++;
                        continue;
                    }

                    if (ARG=="-obj") {
                        if ( !PARAMETER_CONTENT  )
                            return usage(argv[0], argv[i], USAGE_ERROR_ARG_NO );
                        aufruf_par->solve=true;
                        aufruf_par->obj=cArgs[i+1].toStdString();
                        i++;
                        continue;
                    }

                    if (ARG=="-objSense") {
                        if ( !PARAMETER_CONTENT  )
                            return usage(argv[0], argv[i], USAGE_ERROR_ARG_NO );
                        aufruf_par->solve=true;
                        if (cArgs[i+1].startsWith("max") )
                            aufruf_par->objSense=1;
                        else if (cArgs[i+1].startsWith("min") )
                            aufruf_par->objSense=-1;
                        else
                            return usage(argv[0], argv[i], USAGE_ERROR_OBJSENSE );
                        i++;
                        continue;
                    }

                    if (ARG=="-solutionCsv") {
                        aufruf_par->solve=true;
                        aufruf_par->outCsv=true;
                        continue;
                    }

                    if (ARG=="-silent") {
                        aufruf_par->solve=true;
                        aufruf_par->outStdIO=false;
                        continue;
                    }

#endif
                    if ( OUTPUT_PARAMETER ) {
                        for (m = 0; m <= OUTPUT_XML; m++)
                        {
#ifndef PROTO
                         if (m != OUTPUT_PROTOKOLL)
#endif
#ifndef VAR_PROD
                           if (m != OUTPUT_PROD_LIST)
#endif
                             if ( tolower(argv[i][1]) == output_cd[m] )
                             {
#ifdef SOLVERSERVICE
                                if(m==OUTPUT_GLP || m==OUTPUT_XML) {
                                    aufruf_par->model=true;
                                    aufruf_par->solve=false;
                                }
#endif
                                ios = aufruf_par->output + m;
                                if (ios->io_type != IO_TYPE_NO)
                                    return usage(argv[0], argv[i], USAGE_ERROR_OUT_MULT);
                                ios->io_type =  IO_TYPE_FILE ;

                                if (m == OUTPUT_ERROR && PARAMETER_CONTENT)
                                    ios->io_type = IO_TYPE_STR_FILE;

                                if ( !PARAMETER_CONTENT )
                                    ios->file_fp = stdout;
                                else {
                                    ios->filename = argv[i+1];
                                    i++;
                                }

                            }
                        }
                        continue;

                    } else if( INPUT_FILE ) {
                        ios = &(aufruf_par->input);
                        if (ios->io_type == IO_TYPE_NO) {
                            ios->io_type = IO_TYPE_FILE;
                            ios->filename = argv[i];
                            continue;
                        }

                    }

                    /* Ungueltige Angabe */
                    return usage(argv[0], argv[i], USAGE_ERROR_PARAM);

                }

#ifdef SOLVERSERVICE

                if (aufruf_par->solve && (*(aufruf_par->input.filename) == '\0'))
                    return usage(argv[0], (char *)0, USAGE_ERROR_INP_NO );

                if (aufruf_par->solve && aufruf_par->model)
                    return usage(argv[0], (char *)0, USAGE_ERROR_MODE );
#endif

                /* Pruefen und fehlende Standardwerte setzen */
                return fill_aufruf_str(aufruf_par);
        }





	/* Initialisiert die Aufrufparameterstruktur mit Standardwerten */
	void Main::init_aufruf_str(aufruf_str *aufruf_par)
	{
		/* Ein-/Ausgaben */
		int n;
		io_type_str *ios;
		for(n = -1; n <= OUTPUT_XML; n++)  /* Stegger 1.2.3 */
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

		/* Sonstige Standardwerte */
		aufruf_par->exec_mode = 0;
		aufruf_par->comment_len = COMMLEN;

		aufruf_par->matrix_out_freemps = 0;

		strcpy(aufruf_par->double_number_format, "%f");		/* 1.1.2 TS: inserted */
#ifdef VAR_PROD
		aufruf_par->is_var_prod_follow = 0;
#endif
		aufruf_par->is_no_reduct = 0;

		aufruf_par->const_def_no_warn = false;
		aufruf_par->assign_old_no_warn = false;
		aufruf_par->num_op_mode = NUM_OP_CHECK_DBL;

#ifdef SOLVERSERVICE
                aufruf_par->solve = true;
                aufruf_par->model = false;
                aufruf_par->solver = "";
                aufruf_par->solverService = "";
                aufruf_par->osolFile = "";
                aufruf_par->outCsv = false;
                aufruf_par->outStdIO = true;
                aufruf_par->noOutput = false;
                aufruf_par->obj ="";
                aufruf_par->objSense=0;
#endif
	}

	/* Aufrufparameter mit Standardwerten vervollstaendigen */
	bool Main::fill_aufruf_str(aufruf_str *aufruf_par)
	{
		io_type_str *ios, *ios2;
		char *cp, *c1;
		int n;

		/* Eingabedatei muss angegeben sein */	
		if (aufruf_par->input.io_type == IO_TYPE_NO)
			return usage((char *)0, (char *)0, USAGE_ERROR_INP_NO);

		/* Wenn Ausfuehrungsmodus 0, dann wird Ausgabedatei jedenfalls erstellt */
		if (aufruf_par->exec_mode == 0)
		{
                        ios = aufruf_par->output + OUTPUT_GLP;
                        ios2 = aufruf_par->output + OUTPUT_XML;

#ifdef SOLVERSERVICE
                        if (aufruf_par->solve) {
                            if (ios2->io_type == IO_TYPE_NO )
                            {
                                ios2->io_type = IO_TYPE_FILE;

                                ios2->filename = (char *) fileIO->io_getmem((strlen(aufruf_par->input.filename) + 5), sizeof(char));
                                ios2->mem_filename = true;
                                strcpy(ios2->filename, aufruf_par->input.filename);

                                for(cp=ios2->filename, c1=(char *)0; *cp; cp++) if(*cp=='.') c1 = cp;
                                if(! c1) c1 = cp;
                                strcpy(c1, ".osil");

                              }
                        }
#endif
                        /* Zur Zeit nur Dateiausgabe, andere Arten darauf aendern */
			if (ios->io_type == IO_TYPE_NO || ios->io_type == IO_TYPE_STRING)
                        {
                            if(ios2->io_type==IO_TYPE_NO) {
				ios->io_type = IO_TYPE_FILE;
				if (*(aufruf_par->input.filename) == '\0')
				{
					ios->file_fp = stdout;
				}
				else
				{
					ios->filename = (char *) fileIO->io_getmem((strlen(aufruf_par->input.filename) + 5), sizeof(char));
					ios->mem_filename = true;
					strcpy(ios->filename, aufruf_par->input.filename);
					
					for(cp=ios->filename, c1=(char *)0; *cp; cp++) if(*cp=='.') c1 = cp;
					if(! c1) c1 = cp;
					strcpy(c1, ".mps");

				}
                            }
			}
			else if (ios->io_type == IO_TYPE_STR_FILE)
			{
				ios->io_type = IO_TYPE_FILE;
			}

                        /* stegger 1.2.3 - if XML-OUT then free mps format */
			if(ios2->io_type!=IO_TYPE_NO) {
				aufruf_par->matrix_out_freemps = true;
				//aufruf_par->is_no_reduct = true;
			}


			/* Wenn fuer Fehlerausgabe nichts angegeben, auf stderr ausgeben */
			ios = aufruf_par->output + OUTPUT_ERROR;
			if (ios->io_type == IO_TYPE_NO)
			{
				ios->io_type = IO_TYPE_FILE;
				ios->file_fp = stderr;
			}
                }

		/* Auf gueltigen Wert fuer Festlegung fuer ganzzahlige nummerische Operationen testen */
		if (aufruf_par->num_op_mode < 0 || aufruf_par->num_op_mode > 3)
			return usage((char *)0, (char *)0, USAGE_ERROR_CONST);

		return true;
	}

	/* Gibt Speicher frei, der bei Fuellen der Aufrufparameterstruktur reserviert wurde */
	void Main::free_aufruf_par(aufruf_str *aufruf_par)
	{
		int n;
		io_type_str *ios;
		for(n = -1; n <= OUTPUT_XML; n++) /* stegger 1.2.3 */
		{
			ios = (n == -1 ? &(aufruf_par->input) : (aufruf_par->output + n));
			if (ios->mem_filename)
			{
				free(ios->filename);
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

	/* Anzeige moegliche Aufrufparameter */
	bool Main::usage(const char *name, const char *param, const char *err)
	{
		FILE *fp = (err ? stderr : stdout);

		if (err)
			fprintf(fp, (param ? USAGE_INFO_ERROR_AT : USAGE_INFO_ERROR),
					(param ? param : ""), err);

		fprintf(fp, USAGE_INFO_CALL, (name ? name : ""));


                fprintf(fp, USAGE_INFO_MODEL_MODE) ;
                fprintf(fp, USAGE_INFO_INPUT);
                fprintf(fp, USAGE_INFO_OUT_M);
                fprintf(fp, USAGE_INFO_OUT_XML);
#ifdef SOLVERSERVICE
                fprintf(fp,USAGE_INFO_NOOUTPUT);
#endif

                fprintf(fp, USAGE_INFO_BLANK);

#ifdef SOLVERSERVICE
                fprintf(fp, USAGE_INFO_SOLVER_MODE);
                fprintf(fp, USAGE_INFO_SOLVER);
                fprintf(fp, USAGE_INFO_SOLVERLOCATION);
                fprintf(fp, USAGE_INFO_OSOL);
                fprintf(fp, USAGE_INFO_CSV);
                fprintf(fp, USAGE_INFO_STD);
                fprintf(fp, USAGE_INFO_SILENT);
                fprintf(fp, USAGE_INFO_OBJ);
                fprintf(fp, USAGE_INFO_OBJ_SENSE);

                fprintf(fp, USAGE_INFO_BLANK);
#endif

                fprintf(fp, USAGE_INFO_GENERAL_OPTS) ;
                fprintf(fp, USAGE_INFO_OUT_E);
#ifdef VAR_PROD
                fprintf(fp, USAGE_INFO_OUT_L);
#endif
                fprintf(fp, USAGE_INFO_OUT_S);
#ifdef PROTO
                fprintf(fp, USAGE_INFO_OUT_P);
#endif
                fprintf(fp, USAGE_INFO_GEN_NORED);
#ifdef VAR_PROD
                fprintf(fp, USAGE_INFO_GEN_FPROD);
#endif
                fprintf(fp, USAGE_INFO_CONST_MDEF);
                fprintf(fp, USAGE_INFO_ASSIGN_OLD);
                fprintf(fp, USAGE_INFO_CONST_INTOP, NUM_OP_CHECK_DBL);

                fprintf(fp, USAGE_INFO_FRM_REM, COMMLEN);
                fprintf(fp, USAGE_INFO_FRM_FREEMPS);
                fprintf(fp, USAGE_INFO_FRM_NUMFORMAT);

                fprintf(fp, USAGE_INFO_HELP);

                fprintf(fp, USAGE_INFO_VERSION);



		fprintf(fp, "\n");

		/* Keine weitere Ausfuehrung */
		return false;
	}
}


