
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

#include "FileIO.hh"
#include <unistd.h>


namespace cmpl
{
	/* constructor */
	FileIO::FileIO(CmplCore *m)
	{
		glvar = m;

        }


	/* Oeffnen/Bereitstellen Dateien/Daten fuer Eingaben/Ausgaben, die von Anfang an benoetigt werden */
	void FileIO::io_start_files()
	{
		int n;
		io_type_str *ios;

		/* Eingabedatei (erstmal nur Datei moeglich) */
		ios = &(glvar->aufruf_p->input);
		if (ios->io_type == IO_TYPE_FILE || ios->io_type == IO_TYPE_STR_FILE)
		{
			if (ios->file_fp == (FILE *)0)
			{
				ios->file_fp = fopen(ios->filename, "r");
				ios->file_open = true;
				if (ios->file_fp == (FILE *)0)
				{
                                        ios->file_open = false;
					snprintf(glvar->errbuf, ERRBUFLEN, ERROR_FILE_INPUT_OPEN, ios->filename);
					glvar->errbuf[ERRBUFLEN] = '\0';
					output_error((io_type_str *)0, ERROR_TYPE_IO, ERROR_LVL_FATAL,
							(char *)0, glvar->errbuf);
				}
			}
		}

		/* Ausgabedatei: Protokoll */
#ifdef PROTO
		ios = glvar->aufruf_p->output + OUTPUT_PROTOKOLL;
		if (ios->io_type == IO_TYPE_STRING || ios->io_type == IO_TYPE_STR_FILE)
		{
			ios->io_string = (char *) io_getmem(IO_LEN_STR_PROT, sizeof(char));
			ios->io_string[0] = '\0';
			ios->string_size = 0;
			ios->string_mem = IO_LEN_STR_PROT;
		}
		else if (ios->io_type == IO_TYPE_FILE && ios->file_fp == (FILE *)0)
		{
			/* Datei oeffnen */
			ios->file_fp = io_open_file(ios, OUTPUT_PROTOKOLL, &ios->file_open);
		}

		if (ios->io_type != IO_TYPE_NO)
			glvar->is_protokoll = true;
#endif
	}

	/* opens the file for a type of output */
	FILE* FileIO::io_open_file(io_type_str *ios, int out_art, bool *new_opened)
	{
		FILE *fp;

		/* Wenn keine Dateiausgabe, dann nichts zu tun */
		*new_opened = false;
		if (ios->io_type != IO_TYPE_FILE && ios->io_type != IO_TYPE_STR_FILE)
			return (FILE *)0;

		/* Wenn schon offene Datei, dann einfach zurueck */
		if (ios->file_fp)
			return ios->file_fp;

		/* Datei oeffnen */
		fp = fopen(ios->filename, "w");
		if (! fp)
		{
			/* Wenn Fehlerdatei nicht zu oeffnen, dann doch auf stderr ausgeben */
			if (out_art == OUTPUT_ERROR)
				ios->file_fp = stderr;

			snprintf(glvar->errbuf, ERRBUFLEN, ERROR_FILE_OUT_OPEN,
					ios->filename, output_cd[out_art]);
			glvar->errbuf[ERRBUFLEN] = '\0';
			output_error((io_type_str *)0, ERROR_TYPE_IO, ERROR_LVL_NORMAL,
					(char *)0, glvar->errbuf);

			return (out_art == OUTPUT_ERROR ? stderr : (FILE *)0);
		}

		*new_opened = true;
		return fp;
	}

	/* Noch notwendige Ausgaben auf Dateien; Dateien schliessen */
	void FileIO::io_out_string_files()
	{
		io_type_str *ios;
		int n;

		PROTO_OUT_1("\nWrite additional output files\n");

                for(n = -1; n <= OUTPUT_PROTOKOLL; n++)  /*stegger 1.2.3 */
		{
			ios = (n == -1 ? &(glvar->aufruf_p->input) : (glvar->aufruf_p->output + n));
			PROTO_OUT_3("output %d : type %d\n", n, ios->io_type);

			if (ios->io_type == IO_TYPE_FILE)
			{
				if (ios->file_fp && ios->file_open)
				{
					fclose(ios->file_fp);
					ios->file_fp = (FILE *)0;
					ios->file_open = false;

					/* Weitere Ausgaben in das File nicht m�glich
					   Fuer Fehlerausgabe aber ersatzweise jetzt wieder stderr */
					if (n == OUTPUT_ERROR)
						ios->file_fp = stderr;
					else
						ios->io_type = IO_TYPE_NO;
				}
			}
			else if (n >= 0 && ios->io_type == IO_TYPE_STR_FILE)
			{
				if (n==OUTPUT_ERROR || (ios->io_string || ios->out_str))
				{
					bool opened;
					FILE *fp = io_open_file(ios, n, &opened);
					if (fp) {
						if (fp == stdout || fp == stderr)
							fprintf(fp, "\n");

						if (ios->io_string)
						{
							ios->io_string[ios->string_size] = '\0';
							fprintf(fp, ios->io_string);
						}
						else switch(n)
						{
							case OUTPUT_ERROR:
								out_file_error(fp, (out_str_error *)ios->out_str, ios->string_size, true);
								break;
                                                }

						if (opened)
							fclose(fp);
					}

					/* Weitere Ausgaben in das File nicht m�glich, aber in den String */
					ios->io_type = IO_TYPE_STRING;
				}
			}
		}
	}

	/* Fehlerausgabe */
	/* 1.1.3 TS: Zusaetzlich zur Quelltextzeile in Fehlermeldungen auch Name der Datei ausgeben */
	void FileIO::output_error(io_type_str *ios, int typ, int level, const char *token, const char *text)
	{
		/* Entweder glvar oder ios sollten als Kontext angegeben sein */
		bool ios_ersatz = false;
		if (! ios)
		{
			if (glvar)
				ios = glvar->aufruf_p->output + OUTPUT_ERROR;
			else
			{
				/* Ersatzweises io_type_str, fuer Ausgabe auf stderr */
				ios = (io_type_str *) io_getmem(1, sizeof(io_type_str));
				ios_ersatz = true;
				ios->io_type = IO_TYPE_FILE;
				ios->file_fp = stderr;
			}
		}

		if (ios->io_type != IO_TYPE_NO)
		{
			if (ios->io_type == IO_TYPE_FILE)
			{
				const char *s1, *s2, *txt_typlvl, *fn;
				int zl;

				/* Noetigenfalls noch Datei oeffnen; ersatzweise stderr verwenden */
				FILE *fp;
				bool opened = false;
				if (!ios->file_fp && glvar)
					ios->file_fp = io_open_file(ios, OUTPUT_ERROR, &opened);
				fp = (ios->file_fp != (FILE *)0 ? ios->file_fp : stderr);

				/* Zeilenvorschub am Ende */
				s1 = ""; 	/*(fp == stderr || fp == stdout ? "\n" : "");*/
				s2 = "\n"; 	/*(fp == stderr || fp == stdout ? "" : "\n");*/
				txt_typlvl = error_typ_lvl(typ, level);

				fn = (glvar && glvar->akt_src_file ? glvar->akt_src_file : "()");
				zl = (glvar ? glvar->akt_src_line : 0);

				if (token)
					fprintf(fp, OUTTXT_ERROR_TOKEN, s1, txt_typlvl, fn, zl, token, text, s2);
				else if (glvar->akt_src_line > 0)
					fprintf(fp, OUTTXT_ERROR_ZEILE, s1, txt_typlvl, fn, zl, text, s2);
				else
					fprintf(fp, OUTTXT_ERROR, s1, txt_typlvl, text, s2);
			}
			else
			{
				out_str_error *dest;
				io_test_getmem_struct(ios, 1, IO_ANZ_STRUCT, sizeof(out_str_error));
				dest = ((out_str_error *)(ios->out_str)) + ios->string_size;

				/* Zielstruktur fuellen */
				dest->typ = typ;
				dest->level = level;
				dest->src_zeile = (glvar ? glvar->akt_src_line : 0);
				dest->src_pos = 0;	/*TODO (derzeit Ausgabe in Struktur nicht verwendet)*/

				if (glvar && glvar->akt_src_file)
				{
					dest->src_file = (char *) io_getmem(strlen(glvar->akt_src_file) + 1, sizeof(char));
					strcpy(dest->src_file, glvar->akt_src_file);
				}
				else
					dest->src_file = (char *)0;

				if (token)
				{
					/*TODO (derzeit Ausgabe in Struktur nicht verwendet)*/
					dest->src_token = (char *) io_getmem(strlen(token) + 1, sizeof(char));
					strcpy(dest->src_token, token);
				}
				else
					dest->src_token = (char *)0;

				dest->fehler_text = (char *) io_getmem(strlen(text) + 1, sizeof(char));
				strcpy(dest->fehler_text, text);

				ios->string_size++;
			}
		}

		/* Wenn hier Ersatz-ios, dann wieder freigeben */
		if (ios_ersatz)
			free(ios);

		if (glvar)
		{
			/* Fehler zaehlen */
			if (level == ERROR_LVL_WARN)
			{
				glvar->anz_warning++;
			}
			else
			{
				glvar->anz_error++;

				if (typ == ERROR_TYPE_COMP)
					glvar->anz_comp_error++;
				else if (typ == ERROR_TYPE_INT)
					glvar->anz_int_error++;

				/* Bei fatalem Fehler hier Abbruch */
				if (level == ERROR_LVL_FATAL)
					longjmp(*(glvar->jmp_env), 1);
			}
		}
	}

	/* Gibt Text fuer Typ-Name fuer Fehlerausgabe zurueck */
	const char* FileIO::output_typename(enum etp tp)
	{
		switch (tp)
		{
			case TP_DBL: case TP_VAR_DBL: return "real";
			case TP_INT: case TP_VAR_INT: return "integer";
			case TP_BIN: case TP_VAR_BIN: return "binary";
			case TP_ISTR: return "string";
			case TP_STR: return "string";
			case TPU_NUM: return "number";

			case TP_SET_ALL: return "set (infinite all)";
			case TP_SET_ALL_NUM: return "set (infinite all integers)";
			case TP_SET_ALL_STR: return "set (infinite all string)";
			case TP_SET_EMPTY: return "set (empty)";
			case TP_SET_HALF_LB: return "set (infinite with lower bound)";
			case TP_SET_HALF_UB: return "set (infinite with upper bound)";
			case TP_SET_ALG: return "set (algorithmic)";
			case TP_SET_ALG0_1: return "set (interval from 0)";
			case TP_SET_ALG1_1: return "set (interval from 1)";
			case TP_SET_ENUM: return "set (enum)";
			case TP_SET_SYM_ARR: return "set (for array)";

			case TPP_EMPTY: return "<empty>";
			case TPP_NULL: return "<null>";
			case TPP_CMD: return "<command>";
			case TPP_ADDR: return "<address>";
			case TPP_PTR: return "<pointer>";

			case TPP_MIX_TPK: return "(mixed type classes)";
			case TPK_VAR: return "(type class var)";
			case TPK_VALUE: return "(type class value)";
			case TPK_SET: return "(type class set)";

			case TP_VAR_NEW: return "(var new defined)";
			case TP_VAR_F_NEW: return "(var new defined)";
			
			case TPK_PROBLEM_MAIN: return "cmpl-problem";
			case TPK_PROBLEM_SOL: return "(solution of cmpl-problem)";
			case TPK_PROBLEM_VC: return "(vars/constraints of cmpl-problem)";
			case TPK_PROBLEM_RES: return "(result of cmpl-problem)";
		}

		/* Kein bekannter Typ, sollte nicht vorkommen */
		return "()";
	}

	/* Gibt Text fuer Typ und Level fuer einen Fehler zurueck */
	const char* FileIO::error_typ_lvl(int typ, int level)
	{
		if (level == ERROR_LVL_WARN)
			return ERROR_TYP_TXT_SONST_WARN;
		else if (typ == ERROR_TYPE_USER)
			return ERROR_TYP_TXT_USER;
		else if (typ == ERROR_TYPE_IO)
			return ERROR_TYP_TXT_IO;
		else if (typ == ERROR_TYPE_COMP)
			return ERROR_TYP_TXT_COMP;
		else if (level == ERROR_LVL_FATAL)
			return ERROR_TYP_TXT_SONST_FATAL;
		else if (typ == ERROR_TYPE_INT)
			return ERROR_TYP_TXT_INT;
		else
			return ERROR_TYP_TXT_SONST;
	}

	/* Gibt temp_buf in die Ausgabe art aus */
	void FileIO::output_temp_string(int art)
	{
		io_type_str *ios = glvar->aufruf_p->output + art;
		if (ios->io_type == IO_TYPE_FILE)
		{
			if (ios->file_fp == (FILE *)0)
			{
				ios->file_fp = fopen(ios->filename, "w");
				ios->file_open = true;
				if (ios->file_fp == (FILE *)0)
				{
					/* Keine weiteren Ausgabeversuche fuer diese Art */
					ios->io_type = IO_TYPE_NO;
					snprintf(glvar->errbuf, ERRBUFLEN, ERROR_FILE_OUT_OPEN,
							ios->filename, output_cd[art]);
					glvar->errbuf[ERRBUFLEN] = '\0';
					output_error((io_type_str *)0, ERROR_TYPE_IO, ERROR_LVL_NORMAL,
							(char *)0, glvar->errbuf);
				}
			}

			fprintf(ios->file_fp, glvar->temp_buf);
		}
		else if (ios->io_type == IO_TYPE_STRING || ios->io_type == IO_TYPE_STR_FILE)
		{
			int sl = strlen(glvar->temp_buf) + 1;
			if (ios->string_size + sl >= ios->string_mem)
			{
				int new_size = ios->string_mem + sl + (art == OUTPUT_PROTOKOLL ? IO_LEN_STR_PROT : IO_LEN_STRING);
				ios->io_string = (char *) realloc(ios->io_string, new_size * sizeof(char));
				ios->string_mem = new_size;
			}

			strncpy(ios->io_string + ios->string_size, glvar->temp_buf, sl);
			ios->string_size += sl - 1;
		}
	}

	void* FileIO::io_getmem(size_t len, size_t size)
	{
		return calloc(len, size);
	}

	/* Testet ob noch genuegend Speicher reserviert und reserviert noetigenfalls neuen */
	void FileIO::io_test_getmem_struct(io_type_str *ios, int anz, int step, size_t size)
	{
		/* Noetigenfalls Speicher reservieren */
		if (!ios->out_str || ios->string_size + anz >= ios->string_mem)
		{
			if (! ios->out_str)
			{
				ios->out_str = io_getmem(anz + step, size);
				ios->string_mem = anz + step;
			}
			else
			{
				int new_size = ios->string_mem + anz + step;
				ios->out_str = realloc(ios->out_str, new_size * size);
				ios->string_mem = new_size;
			}
		}
	}


	/* geprueftes Schreiben */
	void FileIO::t_write(int fd, char *buf, int len, char *fname)
	{
		const char *fn = (fname ? fname : "");
		if(len != write(fd, buf, len)) {
			snprintf(glvar->errbuf, ERRBUFLEN, ERROR_FILE_WRITE, fn);
			glvar->errbuf[ERRBUFLEN] = '\0';
			output_error((io_type_str *)0, ERROR_TYPE_IO, ERROR_LVL_FATAL,
						(char *)0, glvar->errbuf);
		}

		PROTO_OUT_3("\n File %s : %d bytes written", fn, len);
	}

	/* geprueftes Lesen */
	void FileIO::t_read(int fd, char *buf, int len, char *fname)
	{
		const char *fn = (fname ? fname : "");
		if(len != read(fd, buf, len)) {
			snprintf(glvar->errbuf, ERRBUFLEN, ERROR_FILE_READ, fn);
			glvar->errbuf[ERRBUFLEN] = '\0';
			output_error((io_type_str *)0, ERROR_TYPE_IO, ERROR_LVL_FATAL,
						(char *)0, glvar->errbuf);
		}

		PROTO_OUT_3("\n File %s : %d bytes read", fn, len);
	}




	/* Dateiname ohne Pfad */
	char* FileIO::name_wo_path(char *str)
	{
		/* Sucht letztes Vorkommen eines Pfadtrenners ("/" oder "\") */
		int n = strlen(str) - 2;
		for (; n > 0; n--)
		{
			if (str[n] == '/' || str[n] == '\\')
				return str + n + 1;
		}
		return str;
	}


	/* ****** io_write_dest *******************************************************/

	void FileIO::io_write_dest()
	{

#ifdef SOLVERSERVICE
            if (!glvar->args->noOutput())
#endif
               if (glvar->args->varprod.isOut())
                    io_write_dest_varprod(); //ToDo -> CmplInstance.writeVarProd()
        }




	/* ****** io_write_dest_others *******************************************************/

        void FileIO::io_write_dest_varprod()
	{

            PROTO_OUT_2("Output for replacements for products of variables: %d\n", glvar->args->varprod.outMode());
            int n;


            if (!glvar->args->varprod.open()) {

                snprintf(glvar->errbuf, ERRBUFLEN, ERROR_FILE_OUT_OPEN,
                         glvar->args->varprod.fileName().c_str() , 's');
                glvar->errbuf[ERRBUFLEN] = '\0';
                output_error((io_type_str *)0, ERROR_TYPE_IO, ERROR_LVL_FATAL,
                             (char *)0, glvar->errbuf);
            }

            struct var_split_str *akt_spl = glvar->var_split_grd;
            struct var_prod_str *akt_prod = glvar->var_prod_grd;

            if (akt_spl || akt_prod)
            {
                char str1[ZC_NAME_LEN+300], str2[ZC_NAME_LEN+300], str3[ZC_NAME_LEN+300];
                struct term_var_str::tv_str *tvp;
                bool opened = false;
                FILE *fp = (FILE *)0;
                ind_inf_one_str *inf_p;

                if (akt_spl) {
                    for(; akt_spl; akt_spl = akt_spl->vs_next_vs) {

                        if(akt_spl->vs_ivar.tv_varp->v_type == TP_VAR_INT) {
                            /* Aufteilung Integervariable */
                            tvp = &(akt_spl->vs_bvars);
                            glvar->symbol_akt_p = glvar->symbol_table + (tvp->tv_sym_num / MAX_ANZ_IND_1);
                            glvar->ind_inf_ptr->i_anz_ind = tvp->tv_sym_num % MAX_ANZ_IND_1;
                            for(n=0, inf_p=glvar->ind_inf_ptr->i_ind; n<glvar->ind_inf_ptr->i_anz_ind-1; n++, inf_p++) {
                                inf_p->is_free = false;
                                inf_p->cur_val = tvp->tv_inds[n].v.i;
                                inf_p->cur_tp = tvp->tv_inds[n].t;
                            }
                            inf_p->is_free = true;
                            inf_p->iter_set.set.t = TP_SET_ALG1_1;
                            inf_p->iter_set.set.v.i = tvp->tv_inds[n].v.i;

                            ferror_symname(str1, true, ZC_NAME_LEN+300);
                            tvstr_symname(str2, &(akt_spl->vs_ivar), ZC_NAME_LEN+300);

                            cout << "integer to binary: " << str1 << " := " << str2 << endl;

                        }
                        else {
                            /* Normierung Realvariable */
                            tvstr_symname(str1, &(akt_spl->vs_bvars), ZC_NAME_LEN+300);
                            tvstr_symname(str2, &(akt_spl->vs_ivar), ZC_NAME_LEN+300);

                            cout << "real to binary: " << str1 << " := " << str2 << endl;

                        }

                    }
                }

                if(akt_prod) {
                    /* Produktvariable */
                    for(; akt_prod; akt_prod = akt_prod->vp_next_vp) {
                        tvstr_symname(str1, &(akt_prod->vp_prod), ZC_NAME_LEN+300);
                        tvstr_symname(str2, &(akt_prod->vp_fakt1), ZC_NAME_LEN+300);
                        tvstr_symname(str3, &(akt_prod->vp_fakt2), ZC_NAME_LEN+300);
                        cout << "product of variables: " << str1 << " := " << str2 << " * " << str3 <<endl;
                    }
                }

            }

            cout << endl;

            glvar->args->varprod.close();
        }


	/* Generieren eines Variablennamens */
	void FileIO::io_make_varname(char *str, var_str *var_p)
	{
		char s[2*DST_NAME_LEN+10];
		char *sp, *sa;
		int n;
		sp = s;

		strncpy(sp, glvar->symbol_akt_p->s_name, DST_NAME_LEN+1);
		sp[DST_NAME_LEN+2] = '\0';

		while(*sp) sp++;
		sa = sp;
		if(glvar->ind_inf_ptr->i_anz_ind > 0) {
                       *(sp++) = '[';
			for(n=0; n<glvar->ind_inf_ptr->i_anz_ind && sp-s <= DST_NAME_LEN; n++) {
                                if(n > 0 || glvar->ind_inf_ptr->i_ind[n].cur_tp == TP_ISTR)
                                    if (n>0)
                                        *(sp++) = ',';

				if (glvar->ind_inf_ptr->i_ind[n].cur_tp == TP_ISTR)
					snprintf(sp, DST_NAME_LEN, "%s", glvar->strings.strings_ind[glvar->ind_inf_ptr->i_ind[n].cur_val]);
				else
					snprintf(sp, DST_NAME_LEN, "%d", glvar->ind_inf_ptr->i_ind[n].cur_val);

                              sp[DST_NAME_LEN+1] = '\0';
				while(*sp) sp++;
                        }

                        *(sp++) = ']';
                        sp++;
		}

		*sp = '\0';
		if(sp-s >= DST_NAME_LEN) {
#ifdef PROTO
			if(glvar->is_protokoll) {
				ferror_symname(glvar->errbuf, false, ERRBUFLEN);
				PROTO_OUT_3("\n  Name of variable '%s' too long (for %s)", s, glvar->errbuf);
			}
#endif
			SPRINT_ERR_2(ERROR_VAR_NAME_LEN, s);
			output_error((io_type_str *)0, ERROR_TYPE_IO, ERROR_LVL_WARN,
					(char *)0, glvar->errbuf);
		}
		s[DST_NAME_LEN-1] = '\0';
		strcpy(str, s);
	}

	/* Ausgabename fuer Variable */
	void FileIO::tvstr_symname(char *str, struct term_var_str::tv_str *tvp, int maxsize)
	{
		int n;
		ind_inf_one_str *inf_p;

		glvar->symbol_akt_p = glvar->symbol_table + (tvp->tv_sym_num / MAX_ANZ_IND_1);
		glvar->ind_inf_ptr->i_anz_ind = tvp->tv_sym_num % MAX_ANZ_IND_1;

		for(n=0, inf_p=glvar->ind_inf_ptr->i_ind; n<glvar->ind_inf_ptr->i_anz_ind; n++, inf_p++)
		{
			inf_p->cur_val = tvp->tv_inds[n].v.i;
			inf_p->cur_tp = tvp->tv_inds[n].t;
		}
		ferror_symname(str, false, maxsize);
	}

	/* Ausgabename fuer Variable und Konstante (auch Matrix) */
	void FileIO::ferror_symname(char *str, bool mfl, int maxsize)
	{
		int n;
		char ind_str[70];
		ind_inf_one_str *inf_p;

		strncpy(str, glvar->symbol_akt_p->s_name, ZC_NAME_LEN-1);
		str[ZC_NAME_LEN-1] = '\0';

		if(glvar->ind_inf_ptr->i_anz_ind > 0)
		{
			strcat(str, "[");
			for(n=0, inf_p=glvar->ind_inf_ptr->i_ind; n<glvar->ind_inf_ptr->i_anz_ind; n++, inf_p++)
			{
				if(strlen(str) > maxsize-70)
				{
					strcat(str, " ...");
					break;
				}
				if(n>0) strcat(str, ",");

				if(mfl && inf_p->is_free)
				{
					write_set_to_str(str + strlen(str), &(inf_p->iter_set.set), 50);
				}
				else
				{
					if (inf_p->cur_tp == TP_INT)
					{
						sprintf(ind_str, "%ld", inf_p->cur_val);
					}
					else
					{
						if (strlen(glvar->strings.strings_ind[inf_p->cur_val]) <= 20)
							sprintf(ind_str, "\"%s\"", glvar->strings.strings_ind[inf_p->cur_val]);
						else
						{
							snprintf(ind_str, 20, "\"%s", glvar->strings.strings_ind[inf_p->cur_val]);
							ind_str[20] = '\0';
							strcat(ind_str, "...\"");
						}
					}
					strcat(str, ind_str);
				}
			}
			strcat(str, "]");
		}
	}

	/* Set als String fuer Ausgabe in string schreiben, dest muss schon mit maxlen+1 Zeichen bereitgestellt sein */
	char* FileIO::write_set_to_str(char *dest, val_str *set, int maxlen)
	{
		/* Wenn kein Set, dann nichts zurueck */
		*dest = '\0';
		if (TYPECLASS(set->t) == TPK_SET)
		{
			/* Wenn expliziter Definitionsset, dann dafuer aufrufen */
			if (SET_USE_ARRAY(set->t) && set->v.array->defset.t != TPP_EMPTY)
				return write_set_to_str(dest, &(set->v.array->defset), maxlen);

			/* Außer fuer TP_SET_ENUM immer einfache Ausgabe */
			if (set->t != TP_SET_ENUM)
			{
				switch (set->t)
				{
					case TP_SET_EMPTY:
						snprintf(dest, maxlen, "set()");
						break;
					case TP_SET_ALL:
						snprintf(dest, maxlen, "..");
						break;
					case TP_SET_ALL_NUM:
						snprintf(dest, maxlen, ".integer.");
						break;
					case TP_SET_ALL_STR:
						snprintf(dest, maxlen, ".string.");
						break;
					case TP_SET_HALF_LB:
						snprintf(dest, maxlen, "%ld..", set->v.i);
						break;
					case TP_SET_HALF_UB:
						snprintf(dest, maxlen, "..%ld", set->v.i);
						break;
					case TP_SET_ALG0_1:
						snprintf(dest, maxlen, "0..%ld", set->v.i);
						break;
					case TP_SET_ALG1_1:
						snprintf(dest, maxlen, "1..%ld", set->v.i);
						break;
					case TP_SET_ALG:
						{
							set_ea_str *eap = set->v.set;
							if (eap->u.a.step_val == 1)
								snprintf(dest, maxlen, "%ld..%ld", eap->u.a.start_val, eap->u.a.end_val);
							else
								snprintf(dest, maxlen, "%ld(%ld)%ld", eap->u.a.start_val, eap->u.a.step_val, eap->u.a.end_val);
						}
						break;
					case TP_SET_SYM_ARR:
						{
							array_str *arr = set->v.array;
							snprintf(dest, maxlen, "(defset d:%ld i:%ld s:%ld)", (long)(arr->a_dir.cnt), (long)(arr->a_num.cnt), (long)(arr->a_str.cnt));
						}
						break;
				}
			}
			else
			{
				if (set->v.set->len == 0)
				{
					snprintf(dest, maxlen, "set()");
				}
				else
				{
					SetIterator iter(glvar, set);
					int l = 0;
				
					l += snprintf(dest, maxlen, "set( ");

					iter.iter_set_start(true);
					while (!iter.set_end && l < maxlen - 10)
					{
						if (iter.cur_val.t == TP_INT)
							l += snprintf(dest+l, maxlen-l-6, "%ld, ", iter.cur_val.v.i);
						else if (iter.cur_val.t == TP_ISTR)
							l += snprintf(dest+l, maxlen-l-6, "\"%s\", ", glvar->strings.strings_ind[iter.cur_val.v.i]);

						if (l >= maxlen - 6)
							break;

						iter.iter_set_next();
					}

					if (iter.set_end)
					{
						dest[l-2] = '\0';
						snprintf(dest+l-2, maxlen, " )");
					}
					else
					{
						snprintf(dest+l, maxlen, "...)");
					}
				}
			}

			dest[maxlen] = '\0';
		}

		return dest;
	}




	/* Erzeugt einen Ausgabenamen aus out_str_var */
	void FileIO::out_var_name(out_str_var *var_p, char *buf, int maxlen)
	{
		strncpy(buf, var_p->name, maxlen-10);
		buf[maxlen-10] = '\0';

		if (var_p->cnt_ind > 0)
		{
			int rest = maxlen - strlen(buf) - 5;
			char *s = buf + strlen(buf);
			int i;

			s[0] = '[';
			for (i = 0, s++; i < var_p->cnt_ind; i++)
			{
				if (var_p->ind_u[i] == var_p->ind_o[i])
					snprintf(s, rest, "%d,", var_p->ind_u[i]);
				else
					snprintf(s, rest, "%d..%d,", var_p->ind_u[i], var_p->ind_o[i]);

				s[rest] = '\0';
				rest -= strlen(s);
				s += strlen(s);
			}
			s[-1] = ']';
			s[0] = '\0';
		}
	}

	/* Ausgabe aus out_str_error fuer Fehler */
	/* 1.1.3 TS: Zusaetzlich zur Quelltextzeile in Fehlermeldungen auch Name der Datei ausgeben */
	void FileIO::out_file_error(FILE *fp, out_str_error *out_err_p, size_t anz, bool xml)
	{
		if (xml)
		{
			/* stegger 1.3.1b error and message output to OSmL file */
			PROTO_OUT_1("\nStart writing xml error file\n");

			fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			fprintf(fp, "<mprl xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://www.coliop.org/schemas/Mprl.xsd\">\n");

	        fprintf(fp, "\t<general>\n");

			if (glvar->anz_error > 0)
				fprintf(fp, "\t\t<generalStatus>error</generalStatus>\n");
			else if (glvar->anz_warning > 0)
				fprintf(fp, "\t\t<generalStatus>warning</generalStatus>\n");
			else
				fprintf(fp, "\t\t<generalStatus>normal</generalStatus>\n");
    
			fprintf(fp, "\t\t<instanceName>%s</instanceName>\n", (glvar->aufruf_p->input.filename ? name_wo_path(glvar->aufruf_p->input.filename) : ""));

			fprintf(fp, "\t\t<mplName>CMPL</mplName>\n");

			
			if (glvar->anz_error > 0)
				fprintf(fp, "\t\t<message>cmpl finished with errors</message>\n");
			else if (glvar->anz_warning > 0)
				fprintf(fp, "\t\t<message>cmpl finished with warnings</message>\n");
			else
				fprintf(fp, "\t\t<message>cmpl finished normal</message>\n");

			fprintf(fp, "\t</general>\n");

			if (anz > 0)
			{
				out_str_error *errormsg;
				int i;

				fprintf(fp, "\t<mplResult numberOfMessages=\"%i\">\n", (int)anz);
        
				for (i = 0, errormsg = out_err_p; i < anz; i++, errormsg++)
				{
					fprintf(fp, "\t\t<mplMessage");

					if (errormsg->level == ERROR_LVL_WARN)
						fprintf(fp, " type =\"warning\"");
					else
						fprintf(fp, " type =\"error\"");
				  

					fprintf(fp, " file=\"%s\"", errormsg->src_file);
					fprintf(fp, " line=\"%d\"", errormsg->src_zeile);
					fprintf(fp, " description=\"%s\"", errormsg->fehler_text);

					fprintf(fp,"/>\n" );
         		}

        		fprintf(fp, "\t</mplResult>\n");
			}

         	fprintf(fp, "</mprl>");
		}
		else
		{
			/* Alte Fehlerausgabe */
			int n;
			const char *s1, *s2, *txt_typlvl, *fn;
			s1 = "";	/*(fp == stderr || fp == stdout ? "" : "\n");*/
			s2 = "\n";	/*(fp == stderr || fp == stdout ? "" : "\n");*/

			for (n = 0; n < anz; n++, out_err_p++)
			{
				txt_typlvl = error_typ_lvl(out_err_p->typ, out_err_p->level);
				fn = (out_err_p->src_file ? out_err_p->src_file : "()");

				if (out_err_p->src_token)
					fprintf(fp, OUTTXT_ERROR_TOKEN, s1, txt_typlvl,
							fn, out_err_p->src_zeile, out_err_p->src_token, out_err_p->fehler_text, s2);
				else if (out_err_p->src_zeile >= 0)
					fprintf(fp, OUTTXT_ERROR_ZEILE, s1, txt_typlvl,
							fn, out_err_p->src_zeile, out_err_p->fehler_text, s2);
				else
					fprintf(fp, OUTTXT_ERROR, s1, txt_typlvl, out_err_p->fehler_text, s2);
			}
		}

		if (fp == stderr || fp == stdout)
			fprintf(fp, "\n");
	}


	/* stegger 1.3.1 read a double value from stdin */
	void FileIO::io_read_stdin(const char *prompt, val_str *retp)
	{
		double v;

		fprintf(stdout, "\n%s", prompt);
		fscanf(stdin, "%lf", &v);

		retp->t = TP_DBL;
		retp->v.f = v;
	}

	/* stegger 1.3.1.b import values into a vector or matrix from a csv file */
	void FileIO::io_read_csv(int dim, const char *fname, IntFuncBase *callback)
	{
		FILE *csv_fp;
		char val_buf[60];
		char csvName[512];
		char *p;
		long line_count;
		long element_count; 
		double d_buf;

		
		if (dim != 1 && dim != 2)
		{
			output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_CONST_DEF_DIM);
			return;
		}

		// Dateiname mit Pfad bestimmen
#ifdef CHDIR
        char dir_sep = glvar->dir_sep_char;
#else
        char dir_sep = '/';
#endif
		if (fname[0] == dir_sep)
		{
			strncpy(csvName, fname, 511);
			csvName[511] = '\0';
		}
		else
		{
			if( glvar->incl_dir_name[glvar->incl_stack_ptr - 1].length()!=0  )
			{
				SPRINT_TEMP_4("%s%c%s", glvar->incl_dir_name[glvar->incl_stack_ptr - 1].c_str(), dir_sep, fname);
				strncpy(csvName, glvar->temp_buf, 511);
			}
			else
				strncpy(csvName, fname, 511);
			csvName[511] = '\0';
		}

#ifdef CHDIR
		/* Pfad-Separator tauschen, damit in CMPL unabhaengig vom Betriebssystem immer '/' verwendet werden kann */
		if (dir_sep != '/')
		{
			for (p = csvName; *p != '\0'; p++)
			{
				if (*p == '/')
					*p = dir_sep;
			}
		}
#endif
	
		PROTO_OUT_3("\nread csv file '%s' dim %d", csvName, dim);

		csv_fp = fopen( csvName, "r" );
		if (! csv_fp)
		{
			SPRINT_ERR_2(ERROR_FILE_INPUT_OPEN, csvName);
			output_error((io_type_str *)0, ERROR_TYPE_IO, ERROR_LVL_FATAL, (char *)0, glvar->errbuf);
		}
		else
		{
			p = val_buf;
			line_count = 0;
			element_count = 0;
			
			while(!feof(csv_fp))
			{
				*p = fgetc(csv_fp);
				if (*p==';' || *p==',' || *p=='\n' || *p=='\r' || feof(csv_fp))
				{

					d_buf = atof(val_buf);
					
					callback->push_dbl(d_buf);
					element_count++;
					
					if (*p=='\n' || *p=='\r' || feof(csv_fp))
					{
						callback->push_int(element_count); /* col count */
						element_count = 0;
						line_count++;
					
						if (dim==1 && line_count>1)
							break;
					}

					p = val_buf;
				}
				else
				{
					p++;
				}
			}

			fclose(csv_fp);
			
			if (dim > line_count)
			{
				// TS 24.03.2010: Ich finde auch eine einzeilige Matrix ok, deshalb hier hoechstens eine Warnung ausgeben
				SPRINT_ERR_2(ERROR_FILE_CSV_DIM, fname);
				output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);
			} 

			if (dim == 2)
				callback->push_int(line_count);  /* row count */
		}
		
		PROTO_OUT_2("\nread csv file '%s' finished", csvName);
	}
	
	


}

