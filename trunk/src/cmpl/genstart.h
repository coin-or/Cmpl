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
 * Strukturen fuer Aufrufparameter und Ergebnisse
 * bzw. fuer direkte Datenuebergabe
 *
 *************************************************************************************/

#ifndef GENSTART_H
#define GENSTART_H


#include <stdio.h>
#include <string.h>



#ifdef SOLVERSERVICE
#include <string>
#include <cmath>
using namespace std;
#endif


/* Typen fuer Ein-/Ausgabe */
#define IO_TYPE_NO		0	/* Keine solche Ausgabe (fuer Eingabe nicht erlaubt) */
#define IO_TYPE_FILE		1	/* Datei (Dateiname und Dateipointer in io_type_str gesetzt) */
#define IO_TYPE_STRING		2	/* String (Stringquelle/-ziel in io_type_str gesetzt)
								   	(statt String fuer besondere Ausgaben auch extra Struktur dafuer) */
#define IO_TYPE_STR_FILE	3	/* Intern ueber String, aber beim Aufruf String in Datei schreiben
								   bzw. aus Datei lesen	(Datei und String in io_type_str gesetzt) */

/* Ausgabearten (als Index in aufruf_str.output) */
#define OUTPUT_GLP			0	/* Ausgabe des generierten Problems als MPS */
#define OUTPUT_ERROR		1	/* Ausgabe Fehler und Warnungen (eigene Struktur) */
#define OUTPUT_MODELL		2	/* Ausgabe Modell mit Zielvariablennamen und ohne Konstanten */
#define OUTPUT_MATRIX		3	/* Ausgabe als Matrix (eigene Struktur) (in Datei dann als CSV) */
#define OUTPUT_VAR_LIST		4	/* Ausgabe Zuordnung Variablennamen (eigene Struktur) */
#define OUTPUT_PROD_LIST	5	/* Ausgabe Ersetzungen fuer Variablenprodukte (eigene Struktur) */
#define OUTPUT_PARSE_LIST	6	/* Ausgabe Parse-Info (welche syntaktische Struktur wo) (eigene Struktur) */
#define OUTPUT_STATISTIC    7	/* Statistikausgabe (Struktur out_str_statistic) (-s beim Aufruf) */
#define OUTPUT_PROTOKOLL    8	/* Protokollausgabe (-p beim Aufruf) */
#define OUTPUT_XML			9	/* Ausgabe des generierten Problems als XML  (-x beim Aufruf) */ /* stegger 1.2.3 */

/* Fehlertypen und Fehlerschweregrade */
#define ERROR_TYPE_USER		-1	/* Nutzerdefinierter Fehler */
#define ERROR_TYPE_SONST	0	/* Sonstiger Fehler */
#define ERROR_TYPE_COMP		1	/* Compiler-Fehler */
#define ERROR_TYPE_INT		2	/* Interpreter-Fehler */
#define ERROR_TYPE_IO		3	/* Ein/Ausgabe-Fehler */

#define ERROR_LVL_NORMAL	0	/* Normal (Fehler, aber erstmal weitere Abarbeitung) */
#define ERROR_LVL_FATAL		1	/* Fatal (Abarbeitung wird abgebrochen) */
#define ERROR_LVL_WARN		-1	/* Warnung (kein Fehler) */
								   
/* Anfangsgroessen bzw. Schrittweite fuer Vergroesserungen fuer Ein-/Ausgabestrings */
#define IO_LEN_STRING		10000		/* Fuer alle Ein-/Ausgabestrings ausser Protokoll */
#define IO_LEN_STR_PROT		100000		/* Fuer Ausgabestring fuer Protokoll */
#define IO_ANZ_STRUCT		500			/* Fuer Ausgabestrukturen (ausser Statistik) */


/* Moeglichkeiten fuer Tests/Typwandlungen fuer nummerische Operationen */
#define NUM_OP_NOCHECK		0			/* Ganzzahlige Operationen werden ohne Pruefungen auf Bereichsueberschreitung ausgefuehrt */
#define NUM_OP_CHECK_DBL	1			/* Operationen werden als double ausgefuehrt und nur dann wieder in ganzzahlig gewandelt, wenn keine Bereichsueberschreitung */
#define NUM_OP_CHECK_ERR	2			/* Operationen werden als double ausgefuehrt und dann in ganzzahlig gewandelt, wenn dabei Bereichsueberschreitung, dann Fehlermeldung */
#define NUM_OP_ALWAYS_DBL	3			/* Operationen werden als double ausgefuehrt und das Ergebnis bleibt immer double */


namespace cmpl
{
	/* Struktur fuer Ein-/Ausgabe */
	typedef struct {
		int io_type;						/* Typ (einer von IO_TYPE_*) */
		char *filename;						/* Dateiname */
		FILE *file_fp;						/* Dateipointer */
		bool mem_filename;			/* Speicher fuer Dateiname selbst reserviert */
		bool file_open;			/* file_fp ist geoeffnete Datei, die auch wieder zu schliessen ist */
		char *io_string;					/* String */
		void *out_str;						/* Pointer auf Struktur fuer Ausgabe, wenn nicht direkt als String gespeichert */
		size_t string_size;					/* Aktuelle Laenge in io_string bzw. Anzahl in out_str */
		size_t string_mem;					/* Reservierte Laenge in io_string bzw. Anzahl in out_str */
	} io_type_str;
											   
	/* Struktur fuer Aufrufparameter */
	typedef struct {
		io_type_str input;						/* Eingabe */
		io_type_str output[OUTPUT_XML+1];	/* Ausgabe der verschiedenen Arten OUTPUT_*  stegger 1.2.3 */

		int exec_mode;							/* Ausfuehrungsmodus */

		int comment_len;					/* -fc<Kommentarlaenge>: maximale Kommentarlaenge fuer MPS */

		bool matrix_out_freemps;		/* -ff beim Aufruf, Matrix im Free-MPS-Format (0/1) */
		
		char double_number_format[11];			/* 1.1.2 TS: format spezifier for double output */
#ifdef VAR_PROD
		bool is_var_prod_follow;	/* -gf beim Aufruf, zusaetzliche Restriktionen
												direkt nach Erzeugung in Matrix */
#endif
		bool is_no_reduct;			/* -gn beim Aufruf, jede Restriktion wird auch
												in eine Matrixzeile umgesetzt */

		bool const_def_no_warn;	/* -cd beim Aufruf: keine Warnung bei mehrfacher Definition einer Konstanten */
		bool assign_old_no_warn;	/* -ca beim Aufruf: Alter Zuweisungsoperator '=' ohne Warnung erlaubt */
		int num_op_mode;					/* -ci<Modus> beim Aufruf, fuer Umgang mit ganzzahligen Werten in nummerischen Operationen
													Wert fuer Modus entsprechend NUM_OP_* */

#ifdef SOLVERSERVICE
                bool solve;             /* after creating the osil file the model is to be solved*/
                bool model;

                string solver;
                string solverService;
                string osolFile;
                bool outCsv;
                bool outStdIO;
                bool noOutput;
                std::string obj;
                int objSense; /* 0 - nothing , 1 - max , 2 - min */
#endif

	} aufruf_str;


	/* Spezielle Strukturen fuer Ausgaben */
	/* Hilfsstruktur fuer Variablen */
	typedef struct {
		char *name;							/* Variablenname: Speicher hier reserviert */
		int cnt_ind;						/* Anzahl Indizes */
		int ind_u[MAX_ANZ_IND_1];			/* Indexwert / Unterer Indexwert bei freiem Index */
		int ind_o[MAX_ANZ_IND_1];			/* Indexwert / Oberer Indexwert bei freiem Index */
	} out_str_var;

	/* Fuer Fehler */
	typedef struct {
		int typ;							/* ERROR_TYPE_* (0: unbestimmt, 1: Compiler, 2: Interpreter, 3: Ein/Ausgabe, -1: Nutzerdefinierter Fehler) */
		int level;							/* ERROR_LVL_* (0: Normal, 1: Fatal Error (Abarbeitung danach beendet), -1: Warnung (kein Fehler)) */
		int src_zeile;						/* Quelltextzeile (-1: nicht quelltextbezogen) */
		int src_pos;						/* Zeichenposition im Quelltext (-1: nicht quelltextbezogen oder keine Positionsangabe) */
		char *src_file;						/* Name Quelltextdatei (Speicher hier reserviert) */
		char *src_token;					/* Token im Quelltext (nur Compilerfehler) */
		char *fehler_text;					/* Fehlertext */
	} out_str_error;

	/* Fuer Variablenprodukte (OUTPUT_PROD_LIST) */
	typedef struct {
		int typ;							/* Typ: 0:Produkt, 1:Integerzerlegung, 2:Realnormierung */
		out_str_var dest_var;				/* Generierte Ersatzvariable (wenn typ==1, dann letzter Index frei) */
		out_str_var src_var1;				/* Erste Ausgangsvariable */
		out_str_var src_var2;				/* Zweite Ausgangsvariable (nur wenn typ==0) */
	} out_str_prod_list;

	/* Fuer Kurzstatistik (OUTPUT_STATISTIC) */
	typedef struct {
		char *dest_name;					/* Name der Zieldatei (Speicher hier nicht reserviert!) */
		int cols;							/* Spaltenanzahl */
		int rows;							/* Zeilenanzahl */
		int koeff;							/* Koeffizienten ungleich 0 */
	} out_str_statistic;
											   

	   
	/* Funktionen fuer den Start der Ausfuehrung */
	extern int start_args(int argc, char *argv[]);
	extern int start_par(aufruf_str *aufruf_par);


#ifdef CMPL_MAIN_CC
	/* Kennzeichen fuer Ausgaben in den Aufrufparametern (Index entsprechend OUTPUT_*) */
	char output_cd[] = {
		'm',	/* Ausgabe des generierten Problems als MPS File*/
		'e',	/* Ausgabe Fehler und Warnungen */
		'z',	/* Ausgabe Modell mit Zielvariablennamen und ohne Konstanten */
		't',	/* Ausgabe als Matrix (in Datei dann als CSV) */
		'v',	/* Ausgabe Zuordnung Variablennamen */
		'l',	/* Ausgabe Ersetzungen fuer Variablenprodukte */
		'y',	/* Ausgabe Parse-Info (welche syntaktische Struktur wo) */
		's',	/* Statistikausgabe */
		'p',	/* Protokollausgabe */
                'x'	/* Ausgabe des generierten Problems als XML */  /* stegger 1.2.3 */
	};
#else
	extern char output_cd[];
#endif
}

#endif // !GENSTART_H

