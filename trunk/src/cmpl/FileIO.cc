
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

#include "FileIO.hh"
#include <unistd.h>


namespace cmpl
{
	/* constructor */
	FileIO::FileIO(Main *m)
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

		for(n = -1; n <= OUTPUT_XML; n++)  /*stegger 1.2.3 */
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

							case OUTPUT_PROD_LIST:
								out_file_prod_list(fp, (out_str_prod_list *)ios->out_str, ios->string_size);
								break;

							case OUTPUT_STATISTIC:
								out_file_statistic(fp, (out_str_statistic *)ios->out_str);
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



	/* 1.1.3 TS: Ausgabe der Zielfunktion und der Optimierungsrichtung */
	void FileIO::io_write_obj_to(FILE *fp)
	{
		int z;
		int anz_obj = 0;

		glvar->dest_akt = glvar->dest_grd;
		for(z=0; glvar->dest_akt; z++) {
			if (glvar->dest_akt->d_line_obj_to != 0)
			{
				PROTO_OUT_3("Writing objective name and sense: %s %d\n", glvar->dest_akt->d_name, glvar->dest_akt->d_line_obj_to);

				anz_obj++;
				if (anz_obj == 1)
				{
					/* Erste Angabe der Optimierungsrichtung: in die MPS-Datei ausgeben */
					fprintf(fp, "* OBJNAME %s\n", glvar->dest_akt->d_name);
					if (glvar->dest_akt->d_line_obj_to > 0)
						fprintf(fp, "* OBJSENSE max\n");
					else
						fprintf(fp, "* OBJSENSE min\n");
				}
			}

			glvar->dest_akt = glvar->dest_akt->d_nextline;
		}

		/* Wenn mehr als eine Angabe Optimierungsrichtung, dann Warnung ausgeben */
		if (anz_obj > 1)
			output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN,
					(char *)0, ERROR_MULT_OBJECTIVE);
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
	/* Schreiben der Zielmatrix */
	void FileIO::io_write_dest()
	{
#ifdef SOLVERSERVICE
            if (!glvar->aufruf_p->noOutput) {
#endif
		io_write_dest_mps();
                io_write_dest_xml();
#ifdef SOLVERSERVICE
            }
#endif
		io_write_dest_others();
	}



	/* ****** io_write_dest_mps *******************************************************/
	/* Schreiben der Zielmatrix */
	void FileIO::io_write_dest_mps()
	{
		int n, m;
		int nl, nl1, anzl, n1, z, w;
		int anz_sp, anz_zl;
		val_str *vp;
		l_elem_str *le, *le1;
		double v, v1;
		char *cp;
		char nstr[101], *bp;

		bool is_int_var;
		int vt;

		char vstr[101];
		char c_leer;

		/* Ausgabedatei bestimmen */
		io_type_str *ios;
		FILE *fp;
		int fd;
		bool fp_new_opened;

		int out_art = OUTPUT_GLP;

		ios = glvar->aufruf_p->output + out_art;
		if (ios->io_type == IO_TYPE_NO)
			return;

		/* Zur Zeit gibt es nur File-Ausgabe */
		if (ios->io_type != IO_TYPE_FILE)
			return;

		PROTO_OUT_2("\nStart writing output file type -%c\n", output_cd[out_art]);

		fp = ios->file_fp = io_open_file(ios, out_art, &fp_new_opened);
		fd = fileno(fp);

		for(n=0,anz_sp=0; n<glvar->akt_defnum; n++) if(glvar->dest_names[n].n_is_koeff) {
			anz_sp++;
		}
		anz_zl = glvar->akt_s_line - 1;

		c_leer = ' ';

		/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
		if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, "%s", "* CMPL - Free-MPS - Export\n");
		else  fprintf(fp, "%s", "* CMPL - MPS - Export\n");

		n = (glvar->aufruf_p->comment_len < TEMPBUFLEN ? glvar->aufruf_p->comment_len : TEMPBUFLEN);
		n = (n > 0 ? n : 0);
		snprintf(glvar->temp_buf, n, "NAME%10c%s\n", c_leer, (ios->filename ? name_wo_path(ios->filename) : ""));
		glvar->temp_buf[n] = '\0';
		fprintf(fp, "%s", glvar->temp_buf);

		/* 1.1.3 TS: Gegebenenfalls Ausgabe der Zielfunktion und Optimierungsrichtung in die MPS-Datei */
		io_write_obj_to(fp);

		fprintf(fp, "ROWS\n");
		glvar->dest_akt = glvar->dest_grd;
		for(z=0; glvar->dest_akt; z++) {
			char md;
			switch(glvar->dest_akt->d_mode) {
			case MODE_G:
				md = 'E';
				break;
			case MODE_U:
				md = 'G';
				break;
			case MODE_O:
				md = 'L';
				break;
			case MODE_N:
				md = 'N';
				break;
			}

			/* 1.1.2 stegger -  Free-MPS / MPS - format differences  */
			if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, " %c  %s\n", md, glvar->dest_akt->d_name);
			else  fprintf(fp, " %c  %-8.8s\n", md, glvar->dest_akt->d_name);

			glvar->dest_akt = glvar->dest_akt->d_nextline;
		}

		fprintf(fp, "COLUMNS\n");
		is_int_var = false;
		for(vt=0,w=0; vt<2; vt++) for(n=0; n<glvar->akt_defnum; n++)
		   if(glvar->dest_names[n].n_is_koeff) {
			if(vt==0 && glvar->dest_names[n].n_type != TP_VAR_DBL ||
							vt==1 && glvar->dest_names[n].n_type == TP_VAR_DBL) continue;
			if(vt==1 && !is_int_var) {
				/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
				if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp,"%cGVANF%c'MARKER'  %c'INTORG'\n", c_leer, c_leer, c_leer);
				else fprintf(fp,"%4cGVANF%5c'MARKER'  %15c'INTORG'\n", c_leer, c_leer, c_leer);
				is_int_var = true;
			}
			/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
			if (glvar->aufruf_p->matrix_out_freemps)  snprintf(nstr, 100, "%c%s", c_leer, glvar->dest_names[n].n_name);
			else snprintf(nstr, 100, "%4c%-8.8s", c_leer, glvar->dest_names[n].n_name);

			glvar->dest_akt = glvar->dest_grd;
			for(z=0,m=0; glvar->dest_akt; z++) {
				for(n1=0, le=glvar->dest_akt->d_valp; n1<glvar->dest_akt->d_anz_val; n1++, le++) {
					if(le->le_col>=n) {
						if(le->le_col>n) break;
						w++;
						vp = &(le->le_val);
						if (vp->t == TP_DBL)
							/* 1.1.2 TS: aufruf_p->double_number_format used for format */
							snprintf(vstr, 100, glvar->aufruf_p->double_number_format, vp->v.f);
						else
							snprintf(vstr, 100, "%ld", vp->v.i);


						if(m==0) {
							/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
							if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, "%s  %s  %s", nstr, glvar->dest_akt->d_name, vstr);
							else fprintf(fp, "%s  %-8.8s  %12.12s", nstr, glvar->dest_akt->d_name, vstr);
						}
						else {
							/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
							if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, " %s  %s\n", glvar->dest_akt->d_name, vstr);
							else fprintf(fp, "   %-8.8s  %12.12s\n", glvar->dest_akt->d_name, vstr);
						}
						m++;
						if(m==2) m=0;
						break;
					}
				}
				glvar->dest_akt = glvar->dest_akt->d_nextline;
			}
			if(m==1) fprintf(fp, "\n");
		}
		if(is_int_var) {
			/* 1.1.2 stegger -  Free-MPS / MPS - format differences  */
			if (glvar->aufruf_p->matrix_out_freemps)  fprintf(fp,"%cGVEND%c'MARKER'  %c'INTEND'\n", c_leer, c_leer, c_leer);
			else fprintf(fp,"%4cGVEND%5c'MARKER'  %15c'INTEND'\n", c_leer, c_leer, c_leer);
		}

		fprintf(fp, "RHS\n");
		glvar->dest_akt = glvar->dest_grd;
		for(z=0,m=0; glvar->dest_akt; z++) {
			v = DBL_VAL(glvar->dest_akt->d_fonds);
			if(! IS_EQ(v, 0.))
			{

                            if (glvar->dest_akt->d_mode==MODE_N) {
                                     output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, ERROR_CONST_IN_OBJ_FUNC);
                            } else {

				if(glvar->dest_akt->d_fonds.t != TP_DBL)
					snprintf(vstr, 100, "%ld", glvar->dest_akt->d_fonds.v.i);
				/* 1.1.2 TS: aufruf_p->double_number_format used for format */
				else snprintf(vstr, 100, glvar->aufruf_p->double_number_format, glvar->dest_akt->d_fonds.v.f);

				if(m==0) {
					/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
					if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, "%c%s  %s  %s", c_leer, FONDS_NAME, glvar->dest_akt->d_name, vstr);
					else fprintf(fp, "%4c%-8.8s  %-8.8s  %12.12s", c_leer, FONDS_NAME, glvar->dest_akt->d_name, vstr);
				}
				else {
					/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
					if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, " %s %s\n", glvar->dest_akt->d_name, vstr);
					else fprintf(fp, "   %-8.8s  %12.12s\n", glvar->dest_akt->d_name, vstr);
				}

				m++;
				if(m==2) m=0;
                            }
			}
			glvar->dest_akt = glvar->dest_akt->d_nextline;
		}
		if(m==1) fprintf(fp, "\n");

#ifdef RANGE
		fprintf(fp, "RANGES\n");
		glvar->dest_akt = glvar->dest_grd;
		for(z=0,m=0; glvar->dest_akt; z++) {
			if(glvar->dest_akt->d_is_range) {
				if(glvar->dest_akt->d_range.t != TP_DBL)
					snprintf(vstr, 100, "%ld", glvar->dest_akt->d_range.v.i);
				else
					/* 1.1.2 TS: aufruf_p->double_number_format used for format */
					snprintf(vstr, 100, glvar->aufruf_p->double_number_format, glvar->dest_akt->d_range.v.f);

				if(m==0) {
					/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
					if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, "%c%s %s %s", c_leer, RANGE_NAME, glvar->dest_akt->d_name, vstr);
					else fprintf(fp, "%4c%-8.8s  %-8.8s  %12.12s", c_leer, RANGE_NAME, glvar->dest_akt->d_name, vstr);
				}
				else {
					/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
					if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, " %s %s\n", glvar->dest_akt->d_name, vstr);
					else fprintf(fp, "   %-8.8s  %12.12s\n", glvar->dest_akt->d_name, vstr);
				}
				m++;
				if(m==2) m=0;
			}
			glvar->dest_akt = glvar->dest_akt->d_nextline;
		}
		if(m==1) fprintf(fp, "\n");
#endif

		fprintf(fp, "BOUNDS\n");
		for(n=0; n<glvar->akt_defnum; n++) if(glvar->dest_names[n].n_is_koeff) {
			if(glvar->dest_names[n].n_is_dn_b || glvar->dest_names[n].n_is_up_b) {
				if(glvar->dest_names[n].n_is_dn_b) {
					vp = &(glvar->dest_names[n].n_dn_b);
					v = DBL_VAL(*vp);
					if(!IS_EQ(v, 0.)) {
						if (vp->t == TP_DBL)
							/* 1.1.2 TS: aufruf_p->double_number_format used for format */
							snprintf(vstr, 100, glvar->aufruf_p->double_number_format, v);
						else
							snprintf(vstr, 100, "%ld", vp->v.i);

						/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
						if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, " LO %s  %s %s\n", BOUND_NAME, glvar->dest_names[n].n_name, vstr);
						else fprintf(fp, " LO %-8.8s  %-8.8s  %12.12s\n", BOUND_NAME, glvar->dest_names[n].n_name, vstr);
					}
				}
				else {
					/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
					if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, " MI %s %s\n", BOUND_NAME, glvar->dest_names[n].n_name);
					else fprintf(fp, " MI %-8.8s  %-8.8s\n", BOUND_NAME, glvar->dest_names[n].n_name);
				}
				if(glvar->dest_names[n].n_is_up_b) {
					vp = &(glvar->dest_names[n].n_up_b);
					if (vp->t == TP_DBL)
						/* 1.1.2 TS: aufruf_p->double_number_format used for format */
						snprintf(vstr, 100, glvar->aufruf_p->double_number_format, vp->v.f);
					else
						snprintf(vstr, 100, "%ld", vp->v.i);

					/* 1.1.2 stegger - Free-MPS / MPS - format differences  */
					if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, " UP %s %s %s\n", BOUND_NAME, glvar->dest_names[n].n_name, vstr);
					else fprintf(fp, " UP %-8.8s  %-8.8s  %12.12s\n", BOUND_NAME, glvar->dest_names[n].n_name, vstr);
				} 
				else {
					if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, " PL %s %s\n", BOUND_NAME, glvar->dest_names[n].n_name);
					else fprintf(fp, " PL %-8.8s  %-8.8s\n", BOUND_NAME, glvar->dest_names[n].n_name);
				}
			}
			else {
				/* 1.1.2 stegger -  Free-MPS / MPS - format differences  */
				if (glvar->aufruf_p->matrix_out_freemps) fprintf(fp, " FR %s %s\n", BOUND_NAME, glvar->dest_names[n].n_name);
				else fprintf(fp, " FR %-8.8s  %-8.8s\n", BOUND_NAME, glvar->dest_names[n].n_name);
			}
		}

		fprintf(fp, "ENDATA\n");


		/* Ausgabe Hauptergebnisdatei fertig */
		if (ios->file_fp && fp_new_opened)
		{
			fclose(ios->file_fp);
			ios->file_fp = (FILE *)0;
		}

	}


	/* Fuegt ein Element an eine Liste aus long-Werten an */	//TODO: entfaellt, stattdessen Verwendung std::list<long>
	long_list *FileIO::add_long_list(long i, long_list **llist, long_list *last_elem)
	{
		long_list *ptr=NULL;
		
		ptr = *llist;
		if (ptr == NULL)
		{
			/* first element */
			ptr = (long_list *) malloc(sizeof(long_list));
			if(ptr==NULL) 
				output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_NO_MEMORY);
			*llist = ptr;
			last_elem=ptr;
		}
		else
		{
			/* add a element */
			ptr=last_elem;
			ptr->next = (long_list *) malloc(sizeof(long_list));
			if(ptr==NULL) 
				output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_NO_MEMORY);
			ptr = ptr->next;
			last_elem=ptr;
		}

		ptr->i = i;
		ptr->next = NULL;
		
		return last_elem;
	}


	/* Fuegt ein Element an eine Liste aus string-Werten an */	//TODO: entfaellt, stattdessen Verwendung std::list<std::string>
	char_list *FileIO::add_char_list(char *cp, char_list **clist, char_list *last_elem)
	{
		char_list *ptr=NULL;
		
		ptr = *clist;
		if (ptr == NULL)
		{
			/* first element */
			ptr = (char_list *) malloc(sizeof(char_list));
			if(ptr==NULL) 
				output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_NO_MEMORY);
			*clist = ptr;
			last_elem=ptr;
		}
		else
		{
			/* add a element */
			ptr=last_elem;
			ptr->next = (char_list *) malloc(sizeof(char_list));
			if(ptr==NULL) 
				output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_NO_MEMORY);
			ptr = ptr->next;
			last_elem=ptr;
		}

		snprintf(ptr->c, 100, "%s", cp);
		ptr->c[100] = '\0';
		ptr->next = NULL;
		
		return last_elem;
	}


	/* ****** io_write_dest_xml *******************************************************/
	void FileIO::io_write_dest_xml()
	{
		int n, m;
		int nl, nl1, anzl, n1, n2, n3, z, nzColumnCount;
		int anz_sp, anz_zl, anz_obj, anz_con;
		val_str *vp;
		l_elem_str *le, *le1;
		double v, v1;
                //char *cp;
		char nstr[101], *bp;

		bool is_int_var;
		int vt;

		char vstr[101];
		char vstr1[101];

		long nzCount=0;

		dest_str *objRow;

		char_list *valM = NULL;
		char_list *lastVal = NULL;
		long_list *rowIdx = NULL;
		long_list *lastRowIdx = NULL;

		/* Ausgabedatei bestimmen */
		io_type_str *ios;
		FILE *fp;
		int fd;
		bool fp_new_opened;

		int out_art = OUTPUT_XML;

		ios = glvar->aufruf_p->output + out_art;
		if (ios->io_type == IO_TYPE_NO)
			return;

		/* Zur Zeit gibt es nur File-Ausgabe */
		if (ios->io_type != IO_TYPE_FILE)
			return;

		PROTO_OUT_2("\nStart writing output file type -%c\n", output_cd[out_art]);

		fp = ios->file_fp = io_open_file(ios, out_art, &fp_new_opened);
		fd = fileno(fp);

                for(n=0,anz_sp=0; n<glvar->akt_defnum; n++)
                    if(glvar->dest_names[n].n_is_koeff) {
			anz_sp++;
		}
                if (anz_sp==0)
                    output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_NO_MODEL);



		anz_zl = glvar->akt_s_line - 1;

		glvar->dest_akt = glvar->dest_grd;
		anz_obj=1;


		fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n");
		fprintf(fp, "<osil xmlns=\"os.optimizationservices.org\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"os.optimizationservices.org http://www.optimizationservices.org/schemas/2.0/OSiL.xsd\">\n");
		
		fprintf(fp, "\t<instanceHeader>\n");
		fprintf(fp, "\t\t<name>%s</name>\n",   (glvar->aufruf_p->input.filename ? name_wo_path(glvar->aufruf_p->input.filename) : ""));
                fprintf(fp, "\t\t<description>generated by CMPL v%s</description>\n", VERSION);
		//fprintf(fp, "\t\t<description></description>\n");
		fprintf(fp, "\t</instanceHeader>\n");

		fprintf(fp, "\t<instanceData>\n");

		fprintf(fp, "\t\t<variables numberOfVariables=\"%i\">\n", anz_sp);

		for(n=0; n<glvar->akt_defnum; n++)
		{
			if(glvar->dest_names[n].n_is_koeff)
			{
				fprintf(fp, "\t\t\t<var name=\"%s\" ", glvar->dest_names[n].n_name);
				if (VAR_WITH_TYPE(glvar->dest_names[n].n_type))
				{
					switch(glvar->dest_names[n].n_type)
					{
						case TP_VAR_DBL:
							sprintf(vstr,"C");
							break;
						case TP_VAR_INT:
							sprintf(vstr,"I");
							break;
						case TP_VAR_BIN:
							sprintf(vstr,"B");
							break;
					}

					fprintf(fp, " type=\"%s\"",  vstr);
				}

				sprintf(vstr,"NOT-DEFINED");
				sprintf(vstr1,"NOT-DEFINED");

				if(glvar->dest_names[n].n_is_dn_b || glvar->dest_names[n].n_is_up_b)
				{
					if(glvar->dest_names[n].n_is_dn_b)
					{
						vp = &(glvar->dest_names[n].n_dn_b);
						v = DBL_VAL(*vp);
						if(!IS_EQ(v, 0.))
						{
							if (vp->t == TP_DBL)
								snprintf(vstr, 100, glvar->aufruf_p->double_number_format, v);
							else
								snprintf(vstr, 100, "%ld", vp->v.i);
						}
						else
							sprintf(vstr, "0");
					}
					else
					{
						sprintf(vstr, "-INF"); /* w/o lower bound definition -> -INF as default*/
					}
					if(glvar->dest_names[n].n_is_up_b)
					{
						vp = &(glvar->dest_names[n].n_up_b);
						if (vp->t == TP_DBL)
							snprintf(vstr1, 100, glvar->aufruf_p->double_number_format, vp->v.f);
						else
							snprintf(vstr1, 100, "%ld", vp->v.i);
					}
				}

				if( strcmp(vstr,"NOT-DEFINED") != 0)
					  fprintf(fp, " lb=\"%s\"", vstr);
				if( strcmp(vstr1,"NOT-DEFINED") != 0)
					  fprintf(fp, " ub=\"%s\"", vstr1);

				fprintf(fp, "/>\n");
			}
		}
		fprintf(fp, "\t\t</variables>\n");

		n=0;
		nzColumnCount=0;
		fprintf(fp, "\t\t<objectives numberOfObjectives=\"%i\">\n", anz_obj);
		glvar->dest_akt = glvar->dest_grd;

#ifdef SOLVERSERVICE

                bool objFound=false;

                while(glvar->dest_akt)
                {
                        if (glvar->dest_akt->d_mode==MODE_N)
                        {
                                n++;
                                //objName=glvar->dest_akt->d_name;
                                if ( glvar->dest_akt->d_name==glvar->aufruf_p->obj) {
                                    objRow=glvar->dest_akt;
                                    objFound=true;
                                    break;
                                }
                                if (n==1) {
                                    objRow=glvar->dest_akt;
                                    if (glvar->aufruf_p->obj.length()==0)
                                        objFound=true;
                                }

                        }
                        if (objFound)
                            break;
                        glvar->dest_akt = glvar->dest_akt->d_nextline;
                }

                if (!objFound) {
                   output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, ERROR_OBJ_FUNC);
                }

                fprintf(fp, "\t\t\t<obj name=\"%s\" maxOrMin=\"", objRow->d_name );

                if (glvar->aufruf_p->objSense!=0) {
                    if ( glvar->aufruf_p->objSense  > 0)
                            fprintf(fp, "max\"");
                    else
                            fprintf(fp, "min\"");
                }
                else if (objRow->d_line_obj_to != 0)
                {
                    if ( objRow->d_line_obj_to  > 0)
                        fprintf(fp, "max\"");
                    else
                        fprintf(fp, "min\"");
                }
                else
                        fprintf(fp, "max\""); /* max as default */

                fprintf(fp, " numberOfObjCoef=\"%i\">\n",  objRow->d_anz_val);

                n2 = 0;
                objFound=false;

                for (n=0; n<glvar->akt_defnum; n++)
                {
                        if (glvar->dest_names[n].n_is_koeff)
                        {
                                glvar->dest_names[n].n_akt_pos=n2;
                                n2++;
                                objFound=false;
                                glvar->dest_akt = glvar->dest_grd;
                                while (glvar->dest_akt)
                                {
                                    if (glvar->dest_akt==objRow) {

                                        objFound=true;

                                        for(n1=0, le=glvar->dest_akt->d_valp; n1<glvar->dest_akt->d_anz_val; n1++, le++)
                                        {
                                            if(le->le_col>=n)
                                            {
                                                if(le->le_col>n)
                                                        break;

                                                vp = &(le->le_val);
                                                if (vp->t == TP_DBL)
                                                        snprintf(vstr, 100, glvar->aufruf_p->double_number_format, vp->v.f);
                                                else
                                                        snprintf(vstr, 100, "%ld", vp->v.i);

                                                fprintf(fp, "\t\t\t\t<coef idx=\"%i\">%s</coef>\n", glvar->dest_names[n].n_akt_pos, vstr);
                                            }
                                        }

                                    }
                                    if (objFound)
                                        break;
                                    glvar->dest_akt = glvar->dest_akt->d_nextline;
                                }
                        }
                        else
                                glvar->dest_names[n].n_akt_pos = -1;
                }

#else

		while(glvar->dest_akt)
		{
			if (glvar->dest_akt->d_mode==MODE_N)
			{
				n++;
				objRow=glvar->dest_akt;
				fprintf(fp, "\t\t\t<obj name=\"%s\" maxOrMin=\"", glvar->dest_akt->d_name);
				if (glvar->dest_akt->d_line_obj_to != 0)
				{
					if (glvar->dest_akt->d_line_obj_to > 0)
						fprintf(fp, "max\"");
					else
						fprintf(fp, "min\"");
				}
				else
					fprintf(fp, "max\""); /* max as default */

				fprintf(fp, " numberOfObjCoef=\"%i\">\n",  glvar->dest_akt->d_anz_val);
			}
			glvar->dest_akt = glvar->dest_akt->d_nextline;
			if (n > 0)
				break; /* only the first objective is supported - all other objectives are used as unbounded constraints */
		}	


		n2 = 0;

		for (n=0; n<glvar->akt_defnum; n++)
		{
			if (glvar->dest_names[n].n_is_koeff)
			{
				glvar->dest_names[n].n_akt_pos=n2;
				n2++;
				n3=0;
				
				glvar->dest_akt = glvar->dest_grd;
				while (glvar->dest_akt)
				{
					if (glvar->dest_akt->d_mode==MODE_N)
					{
						n3++;
						for(n1=0, le=glvar->dest_akt->d_valp; n1<glvar->dest_akt->d_anz_val; n1++, le++)
						{
							if(le->le_col>=n)
							{
								if(le->le_col>n)
									break;
								
								vp = &(le->le_val);
								if (vp->t == TP_DBL)
									snprintf(vstr, 100, glvar->aufruf_p->double_number_format, vp->v.f);
								else
									snprintf(vstr, 100, "%ld", vp->v.i);
							
								fprintf(fp, "\t\t\t\t<coef idx=\"%i\">%s</coef>\n", glvar->dest_names[n].n_akt_pos, vstr);
							}
						}
					}
					glvar->dest_akt = glvar->dest_akt->d_nextline;
					if (n3>0) break; /* only the first objective is supported - all other objectives are used as unbounded constraints */
				}
			}
			else
				glvar->dest_names[n].n_akt_pos = -1;
                }
#endif
		fprintf(fp, "\t\t\t</obj>\n");
                fprintf(fp, "\t\t</objectives>\n");


		fprintf(fp, "\t\t<constraints numberOfConstraints=\"%i\">\n", anz_zl-anz_obj);

		glvar->dest_akt = glvar->dest_grd;
		while (glvar->dest_akt)
		{
			if (glvar->dest_akt!=objRow)
			{
				/* count of the nonzero coefficients */
				nzCount += glvar->dest_akt->d_anz_val;

				if(glvar->dest_akt->d_fonds.t != TP_DBL)
					snprintf(vstr, 100, "%ld", glvar->dest_akt->d_fonds.v.i);

				/* 1.1.2 TS: aufruf_p->double_number_format used for format */
				else
					snprintf(vstr, 100, glvar->aufruf_p->double_number_format, glvar->dest_akt->d_fonds.v.f);

				fprintf(fp, "\t\t\t<con ");

				switch(glvar->dest_akt->d_mode)
				{
					case MODE_G:
						fprintf(fp, "name=\"%s\" lb=\"%s\" ub=\"%s\"/>\n",  glvar->dest_akt->d_name, vstr, vstr );
						break;
					case MODE_U:
						fprintf(fp, "name=\"%s\" lb=\"%s\"/>\n",  glvar->dest_akt->d_name, vstr );
						break;
					case MODE_O:
						fprintf(fp, "name=\"%s\" ub=\"%s\"/>\n", glvar->dest_akt->d_name, vstr );
						break;
					case MODE_N:
					   fprintf(fp, "name=\"%s\"/>\n", glvar->dest_akt->d_name);
						break;
				}
			}

                        if (glvar->dest_akt==objRow) {
                            v = DBL_VAL(glvar->dest_akt->d_fonds);
                            if(! IS_EQ(v, 0.))
                                     output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, ERROR_CONST_IN_OBJ_FUNC);

                        }
			
			glvar->dest_akt = glvar->dest_akt->d_nextline;
		}
		fprintf(fp, "\t\t</constraints>\n");

		  
		fprintf(fp, "\t\t<linearConstraintCoefficients numberOfValues=\"%li\">\n", nzCount);

		fprintf(fp, "\t\t\t<start>\n");
		nzCount=0;

		for (n=0; n<glvar->akt_defnum; n++)
		{
			z = 0;
			if (glvar->dest_names[n].n_is_koeff)
			{
				fprintf(fp, "\t\t\t\t<el>%ld</el>\n",nzCount);
				
				glvar->dest_akt = glvar->dest_grd;
				while(glvar->dest_akt)
				{
					/* dest_akt actual row in the matrix */
					if (glvar->dest_akt!=objRow)
					{
						/* d_anz_val - count of the nonzero variable elements in the actual row*/
						/* d_valp - structure for col and value  */
						for(n1=0, le=glvar->dest_akt->d_valp; n1<glvar->dest_akt->d_anz_val; n1++, le++)
						{
							/* le_col - col number */
							if(le->le_col>=n)
							{
								if(le->le_col>n)
									break;
								
								nzCount++;
							
								lastRowIdx = add_long_list((glvar->dest_akt->d_lineno - z -1), &rowIdx, lastRowIdx);
								/* le_val - value of the element */
								vp = &(le->le_val);
								if (vp->t == TP_DBL)
									snprintf(vstr, 100, glvar->aufruf_p->double_number_format, vp->v.f);
								else
									snprintf(vstr, 100, "%ld", vp->v.i);

								lastVal = add_char_list(vstr, &valM, lastVal);
							}
						}
					}
					else
						z++;
			
					glvar->dest_akt = glvar->dest_akt->d_nextline;
				}
			}
		}
		fprintf(fp, "\t\t\t\t<el>%ld</el>\n",nzCount); /* start index of the nonexistent variable (number of the variables +1) */
		fprintf(fp, "\t\t\t</start>\n");


		fprintf(fp, "\t\t\t<rowIdx>\n");
		fprintf(fp, "\t\t\t\t<el>%i</el>\n",rowIdx->i );
		while(rowIdx->next!=NULL) {
			rowIdx=rowIdx->next;
			fprintf(fp, "\t\t\t\t<el>%i</el>\n",rowIdx->i );
		}
		fprintf(fp, "\t\t\t</rowIdx>\n");


		fprintf(fp, "\t\t\t<value>\n");
		fprintf(fp, "\t\t\t\t<el>%s</el>\n",valM->c);
		while(valM->next!=NULL) {
			valM=valM->next;
			fprintf(fp, "\t\t\t\t<el>%s</el>\n",valM->c);
		}
		fprintf(fp, "\t\t\t</value>\n");
		
		fprintf(fp, "\t\t</linearConstraintCoefficients>\n");
		   
		fprintf(fp, "\t</instanceData>\n");
		fprintf(fp,  "</osil>\n");


		/* Ausgabe Hauptergebnisdatei fertig */
		if (ios->file_fp && fp_new_opened)
		{
			fclose(ios->file_fp);
			ios->file_fp = (FILE *)0;
		}

		free(valM);
		free(rowIdx);

		PROTO_OUT_2("\nFinished - writing output file type -%c\n", output_cd[out_art]);
	}



	/* ****** io_write_dest_others *******************************************************/

	void FileIO::io_write_dest_others()
	{
		int n;
		int anz_sp, anz_zl;
		int w;
		/* Ausgabedatei bestimmen */
		io_type_str *ios;

		for(n=0,anz_sp=0; n<glvar->akt_defnum; n++) if(glvar->dest_names[n].n_is_koeff) {
				anz_sp++;
			}
			anz_zl = glvar->akt_s_line - 1;

#ifdef VAR_PROD
		ios = glvar->aufruf_p->output + OUTPUT_PROD_LIST;
		if(ios->io_type != IO_TYPE_NO) {
			PROTO_OUT_3("Output for replacements for products of variables %d : %d\n", OUTPUT_PROD_LIST, ios->io_type);

			struct var_split_str *akt_spl = glvar->var_split_grd;
			struct var_prod_str *akt_prod = glvar->var_prod_grd;
			if (akt_spl || akt_prod)
			{
				char str1[ZC_NAME_LEN+300], str2[ZC_NAME_LEN+300], str3[ZC_NAME_LEN+300];
				struct term_var_str::tv_str *tvp;
				bool opened = false;
				FILE *fp = (FILE *)0;
				ind_inf_one_str *inf_p;

				if (ios->io_type == IO_TYPE_FILE) {
					fp = io_open_file(ios, OUTPUT_PROD_LIST, &opened);
					if (fp == stdout)
						fprintf(fp, "\n");
				}

				if (akt_spl) {
					for(; akt_spl; akt_spl = akt_spl->vs_next_vs) {
						if (fp) {
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
								fprintf(fp, OUTTXT_PRODLIST_INT, str1, str2);
							}
							else {
								/* Normierung Realvariable */
								tvstr_symname(str1, &(akt_spl->vs_bvars), ZC_NAME_LEN+300);
								tvstr_symname(str2, &(akt_spl->vs_ivar), ZC_NAME_LEN+300);
								fprintf(fp, OUTTXT_PRODLIST_REAL, str1, str2);
							}
						}
						else {
							fill_out_prod_list((akt_spl->vs_ivar.tv_varp->v_type == TP_VAR_INT ? 1 : 2),
									&(akt_spl->vs_bvars), &(akt_spl->vs_ivar), (struct term_var_str::tv_str *)0);
						}
					}
				}

				if(akt_prod) {
					/* Produktvariable */
					for(; akt_prod; akt_prod = akt_prod->vp_next_vp) {
						if (fp) {
							tvstr_symname(str1, &(akt_prod->vp_prod), ZC_NAME_LEN+300);
							tvstr_symname(str2, &(akt_prod->vp_fakt1), ZC_NAME_LEN+300);
							tvstr_symname(str3, &(akt_prod->vp_fakt2), ZC_NAME_LEN+300);
							fprintf(fp, OUTTXT_PRODLIST_PROD, str1, str2, str3);
						}
						else {
							fill_out_prod_list(0, &(akt_prod->vp_prod), &(akt_prod->vp_fakt1), &(akt_prod->vp_fakt2));
						}
					}
				}

				if (fp && opened)
					fclose(fp);
			}
		}
#endif /* VAR_PROD */

		ios = glvar->aufruf_p->output + OUTPUT_STATISTIC;
		if(ios->io_type != IO_TYPE_NO) {
			PROTO_OUT_3("Output statistic %d : %d\n", OUTPUT_STATISTIC, ios->io_type);

			/* Jedenfalls erstmal Struktur fuellen */
			out_str_statistic *out_stat_p = (out_str_statistic *) io_getmem(1, sizeof(out_str_statistic));
			out_stat_p->dest_name = glvar->aufruf_p->output[OUTPUT_GLP].filename;
			out_stat_p->cols = anz_sp;
			out_stat_p->rows = anz_zl;
			out_stat_p->koeff = w;

			/* Entweder direkt ausgeben oder erstmal merken */
			if (ios->io_type == IO_TYPE_FILE) {
				bool opened;
				FILE *fp = io_open_file(ios, OUTPUT_STATISTIC, &opened);
				if (fp)
				{
					out_file_statistic(fp, out_stat_p);
					if (opened)
						fclose(fp);
				}
				free(out_stat_p);
			}
			else {
				ios->out_str = out_stat_p;
				ios->string_size = 1;
				ios->string_mem = 1;
			}
		}
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
			for(n=0; n<glvar->ind_inf_ptr->i_anz_ind && sp-s <= DST_NAME_LEN; n++) {
				if(n > 0 || glvar->ind_inf_ptr->i_ind[n].cur_tp == TP_ISTR)
					*(sp++) = '_';

				if (glvar->ind_inf_ptr->i_ind[n].cur_tp == TP_ISTR)
					snprintf(sp, DST_NAME_LEN, "%s", glvar->strings.strings_ind[glvar->ind_inf_ptr->i_ind[n].cur_val]);
				else
					snprintf(sp, DST_NAME_LEN, "%d", glvar->ind_inf_ptr->i_ind[n].cur_val);

				sp[DST_NAME_LEN+1] = '\0';
				while(*sp) sp++;
			}
			/* Alternativer Name ueber make_ind_letters entfaellt mit der Einfuehrung von String-Indizes (noetigenfalls wird sowieso ein eindeutiger kurzer Name erzeugt)
			if(sp-s >= DST_NAME_LEN) {
				sp = sa;
				*(sp++) = '_';
				for(n=0; n<glvar->ind_inf_ptr->i_anz_ind && sp-s < DST_NAME_LEN+180; n++) {
					make_ind_letters(glvar, &sp, glvar->ind_inf_ptr->i_ind[n].cur_val);
				}
			}
			*/
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


	/* Fuellt ein weiteres Element fuer die Variablenproduktliste */
	void FileIO::fill_out_prod_list(int typ, struct term_var_str::tv_str *dp, struct term_var_str::tv_str *sp1, struct term_var_str::tv_str *sp2)
	{
		io_type_str *ios = glvar->aufruf_p->output + OUTPUT_PROD_LIST;
		out_str_prod_list *dest;
		out_str_var *out_var;
		struct term_var_str::tv_str *src_var;
		symbol_str *symbol_p;
		int n, i;

		/* Noetigenfalls Speicher reservieren */
		io_test_getmem_struct(ios, 1, IO_ANZ_STRUCT, sizeof(out_str_prod_list));

		/* Zielstruktur fuellen */
		dest = ((out_str_prod_list *)(ios->out_str)) + ios->string_size;
		dest->typ = typ;

		for (n = 0; n < (typ == 0 ? 3 : 2); n++)
		{
			src_var = (n == 0 ? dp : (n == 1 ? sp1 : sp2));
			out_var = (n == 0 ? &(dest->dest_var) : (n == 1 ? &(dest->src_var1) : &(dest->src_var2)));

			symbol_p = glvar->symbol_table + (src_var->tv_sym_num / MAX_ANZ_IND_1);
			out_var->name = (char *) io_getmem(strlen(symbol_p->s_name) + 1, sizeof(char));
			strcpy(out_var->name, symbol_p->s_name);

			out_var->cnt_ind = src_var->tv_sym_num % MAX_ANZ_IND_1;
			for (i = 0; i < out_var->cnt_ind; i++)
				out_var->ind_u[i] = out_var->ind_o[i] = src_var->tv_inds[i].v.i;	// Typ des Index wird hier erstmal nicht berücksichtigt, da sowieso nur für Ausgabe der Produktvariablenliste verwendet

			/* Wenn typ == 1 dann in generierter Variable letzter Index frei, 1..angegebener Index */
			if (typ == 1 && n == 0)
				out_var->ind_u[out_var->cnt_ind-1] = 1;
		}

		ios->string_size++;
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

	/* Ausgabe aus out_str_prod_list fuer Liste Variablenprodukte */
	void FileIO::out_file_prod_list(FILE *fp, out_str_prod_list *out_pl_p, size_t anz)
	{
		int n;
		char dest[300], src1[300], src2[300];

		for (n = 0; n < anz; n++, out_pl_p++)
		{
			out_var_name(&(out_pl_p->dest_var), dest, 300);
			out_var_name(&(out_pl_p->src_var1), src1, 300);
			if (out_pl_p->typ == 0)
				out_var_name(&(out_pl_p->src_var2), src2, 300);

			if (out_pl_p->typ == 0)
				fprintf(fp, OUTTXT_PRODLIST_PROD, dest, src1, src2);
			else if (out_pl_p->typ == 1)
				fprintf(fp, OUTTXT_PRODLIST_INT, dest, src1);
			else if (out_pl_p->typ == 2)
				fprintf(fp, OUTTXT_PRODLIST_REAL, dest, src1);
		}
	}

	/* Ausgabe aus out_str_statistic fuer Kurzstatistik */
	void FileIO::out_file_statistic(FILE *fp, out_str_statistic *out_stat_p)
	{
		int anz_koef = out_stat_p->cols * out_stat_p->rows;
		fprintf(fp, OUTTXT_STATISTIC,
			(out_stat_p->dest_name ? out_stat_p->dest_name : ""),
			out_stat_p->cols, out_stat_p->rows,
			out_stat_p->koeff, anz_koef, (double)((100. * out_stat_p->koeff)/anz_koef));
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
		int i,c;
		
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

