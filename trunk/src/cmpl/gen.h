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
 * Allgemeine Makrodefinitionen, insbesondere fuer Beschraenkungen.
 *
 *************************************************************************************/

#ifndef GEN_H
#define GEN_H


#include "gentext.h"

#define VERSION		"1.5.0"			/* version */


#define UPPER_BOUND				/* Restriktionen mit nur einer Variablen in Schranken */
#define RANGE					/* beidseitig beschraenkte Restriktionen */

#define VAR_PROD				/* Erweiterungen fuer Variablenprodukte */

#define FONDS_NAME		"RHS"				/* Name fuer rechte Seiten */
#define RANGE_NAME		"RANGE"				/* Name fuer Ranges */
#define BOUND_NAME		"BOUND"				/* Name fuer Schranken */

//#define DST_NAME_LEN	9		/* Max. Laenge Namen in Zieldatei + 1*/

#define DST_NAME_LEN	(glvar->aufruf_p->matrix_out_freemps ? 100 : 9)  /* 1.1.2 stegger - max lenght parameter for names */
#define DST_NAME_LEN_1  100		  	/* 1.1.2 stegger - used for several structs in genglvar.h */

#define COMMLEN		60			/* Maximallaenge Kommentarsatz in MPS */

#define SYM_NAME_LEN	51		/* Maximale Symbolnamenlaenge + 1 */

#define ZC_NAME_LEN		55		/* Max. Laenge Namen waehrend Verarbeitung */

#define COMP_STR_LEN	2000	/* Max. Laenge Namen/String fuer Compiler */

#define SHORT_NAME_VAR	"_V_%d"	/* Wenn Variablenname zu lang oder doppelt, durch diesen ersetzen */
#define SHORT_NAME_LINE	"_L_%d"	/* Wenn Zeilenname zu lang oder doppelt, durch diesen ersetzen */

#define MAX_PROG		50000	/* Startlaenge Array fuer Zwischencodeprogramm */

#define MAX_MEM			10000	/* Startlaenge Array fuer Speicherverwaltung */

#define ERRBUFLEN   	1000	/* Laenge Puffer fuer Fehlermeldungen */
#define TEMPBUFLEN		1000	/* Laenge Puffer fuer String fuer Wertaufbereitung */

#define MAX_INCL_STACK	20		/* 1.1.3 TS: Groesse des Stacks fuer Include (maximale Schachtelung dafuer) */
#define FILE_PATH_BUF_LEN	2000	/* 1.1.3 TS: Puffergroesse fuer Dateinamen/Pfadnamen */

#define MAX_ANZ_SYMBOL	200		/* Startlaenge Array fuer Symbole */
#define MAX_ANZ_TMP		100		/* Maximal Schachtelungstiefe fuer Bedingungen/Schleifen */

#define MAX_ANZ_IND		9		/* Maximale Anzahl Indizes */
#define MAX_ANZ_IND_1	(MAX_ANZ_IND+1)

#define MAX_ANZ_VPROD	5		/* Max. Anzahl Matrixprodukte in einem Term */

#define MAX_LEN_STACK	10000	/* Startlaenge Array fuer Elemente auf Stack */
#define MAX_IND_INF		10		/* Max. geschachtelte SET_INDEXES */

#define MIN_CAP_ARRAY		30		/* Minimalkapazität für Array */
#define MIN_CAP_GROW		0.1		/* Minimales relatives Wachstum für das Array (aber immer mindestens Minimalkapazität) */
#define MIN_ARRAY_DENSITY	0.5		/* Mindestbelegungsdichte für das Array */

#define LINE_NAME_DEF	"line_$1$"	/* Default fuer Zeilenname */
#define NUM_LINE_NAME	'$'		/* Kennzeichen fuer Einfuegung in Zeilenname */

#define FILE_NAME_LEN	501		/* Max. Laenge fuer Dateinamen (mit Pfad) */

#define LINE_NANE_CONCAT_1		"_"		/* Verkettung fuer Zeilennamen: Trennung zwischen den Praefixen */
#define LINE_NANE_CONCAT_2		"_"		/* Verkettung fuer Zeilennamen: Trennung zwischen Praefix und Laufkonstantenersetzung */
#define LINE_NANE_CONCAT_3		"_"		/* Verkettung fuer Zeilennamen: Trennung zwischen den Laufkonstantenersetzungen */

#ifndef RANGE
#define MAKE_NAME_RANGE	"_Z_"	/* Variablenname fuer Nachbildung Range */
#endif


#ifdef VAR_PROD			/* Variablenprodukte erlaubt */

#define MAX_ANZ_VAR_PROD	5	/* Max. Anzahl von Variablen in einem Produkt */

#define MAKE_NAME_PROD	"%s_%s"		/* Symbolname fuer Produkt */
#define MAKE_NAME_SPLIT	"_%s"		/* Symbolname fuer Binaeraufteilung und fuer normierte Realvariable */

#define MAKE_SNAME_VP	"_P%d_"		/* wenn generierter Symbolname zu lang, diesen Symbolname verwenden */

#endif /* VAR_PROD */


/* stegger 1.3.1.b redefine of snprintf for mingw32 */
#if defined(MINGW32)
	#define snprintf _snprintf
	#define vsnprintf _vsnprintf
#include <io.h>
#endif


#endif // !GEN_H

