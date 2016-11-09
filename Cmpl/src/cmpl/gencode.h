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


#ifndef GENCODE_H
#define GENCODE_H


/* Zwischencodebefehle */
/* <par>                 : Parameter des Befehls */
/* vals_vor -- vals_nach : Aenderungen des Wertestacks */

#define PROG_END			0		/* Ende */

#define DEF_CONST			1		/* <par>  Typ wenn direkt angegeben (sonst TPP_EMPTY) (als eingebetteter Parameter) */
									/* <num>  Setzen Konstante (nach SET_INDEXES) */
										/* 0:  val -- */
										/* 1:  val_1 .. val_n anz -- */
										/* 2:  vals..anz_1 vals..anz_n anz -- */
	/* Unterkommando fuer DEF_CONST */
#define SYM_MAIN			0		/* Symbol ist Hauptsymbol; <num> ist globale Symbolnummer */
#define SYM_SUB				1		/* Symbol ist Subsymbol; <num> ist Indexnummer des Subsymbols in der Subsymboltabelle des Ausdrucks auf dem Stack */
										/* Achtung: Auf dem Stack liegen oben die zuzuweisenden Werte, erst darunter der Ausdruck für die Subsymboltabelle! */
#define SYM_MAIN_REF		2		/* Referenz-Zuweisung: Symbol ist Hauptsymbol; <num> ist globale Symbolnummer */
#define SYM_SUB_REF			3		/* Referenz-Zuweisung: Symbol ist Subsymbol; <num> ist Indexnummer des Subsymbols in der Subsymboltabelle des Ausdrucks auf dem Stack */
										/* Achtung: Auf dem Stack liegen oben die zuzuweisenden Werte, erst darunter der Ausdruck für die Subsymboltabelle! */
#define SYM_UNDEF_ERROR		9		/* Erzeugt einen Fehler wegen undefiniertem Symbol im Interpreter; nur als Platzhalter verwendet, der normalerweise später beim Parsen ersetzt wird */
			/* Zusätzlich im oberen Byte des Unterkommandos die Dimensionalität der rechten Seite */

#define INIT_CMPLDATA		2		/* Initialisierung Werte aus cmplData */

#define DEF_VAR				5		/* <num>  Definieren Variable (nach SET_INDEXES) */
#define RESET_VAR_B			6		/* Zuruecksetzen Schranken */
#define DEF_DN_VAR			7		/* untere Schranke fuer Variable */
										/*  value -- */
#define DEF_UP_VAR			8		/* obere Schranke fuer Variable */
										/*  value -- */
#define DEF_VAR_TYPE		9		/* <type> (als Unterkommando)  Festlegen Variablentyp (Typklasse TPK_VAR) */

#define SET_INDEXES			10		/* Setzen Indexinfo (fuer DEF_CONST und DEF_VAR) */
	/* Unterkommando fuer SET_INDEXES */
#define IND_GIVEN			0		/* Vorgegebene Indizes (jeweils TPK_VALUE oder TPK_SET) */
												/* count ind_n ... ind_1 -- */
#define IND_FREE			1		/* Alle Indizes sind frei (alle TP_SET_ALL) */
												/* count -- */

#define DEL_INDEXES			12		/* Loeschen letztes Indexinfo */

#define IND_TRANSPOSE		13		/* Transponierter Vektor/Matrix */
										/*  (folgt auf SET_INDEXES) */

/* stegger 1.3.1.b import values into a vector or matrix from a csv file */
#define READ_CSV			16		/* <num>  Einlesen Vektor/Matrix mit sovielen Dimensionen (num darf nur 1 oder 2 sein) */
										/*	filename -- values .. */

/* stegger 1.3.1.b read a value from stdin */
#define READ_STDIN			17		/* <par>  Einlesen eines Werts */
										/*	prompt_string -- value */

#define CMPL_SOLVE			19		/* Funktion im Zusammenhang mit Lösung eines CMPL-Problems */
	/* Unterkommando fuer CMPL_SOLVE */
#define CMPL_PROBLEM_SOLVE	0		/* Asynchroner Aufruf für Loesen eines oder mehrere CMPL-Probleme */
										/* val_1 .. -- */
#define CMPL_PROBLEM_STAT	1		/* Abfrage Status fuer ein CMPL-Problem */
										/* val_problem -- val_status */
#define CMPL_PROBLEM_WAIT	2		/* Warten auf Loesung fuer ein CMPL-Problem */
										/* val_problem -- val_success */
#define CMPL_PROBLEM_KILL	3		/* Abbrechen der Loesung fuer ein CMPL-Problem */
										/* val_problem -- val_success */

#define FETCH_CONST			20		/* <num>  Laden Konstante */
										/* ind_1 .. ind_n -- const_val */
	/* Unterkommando wie fuer DEF_CONST */

#define F_CONST_FREE		21		/* <num>  Laden Konstantenvektor (nach SET_INDEXES) */
										/*  -- const_val */
	/* Unterkommando wie fuer DEF_CONST */

#define FETCH_TMP			25		/* <num>  Laden Laufkonstante */
										/*  -- value */
#define STORE_TMP			26		/* <num>  Setzen Laufkonstante */
										/* value --  */

#define FETCH_VALS			28		/* <num>  Laden aller Konstantenwerte nach SET_INDEXES */
										/*  -- val_1 .. val_n anz */
	/* Unterkommando wie fuer DEF_CONST */

#define LIT_VAL				30		/* <int>/<double>/<istring>	Literalen Wert auf den Stack */
										/*  -- value */
#define LIT_VAL_SHORT		31		/* <int> (eingebetteter Parameter)		Literalen Short-Wert auf den Stack (Typ immer TP_INT) */
										/*  -- value */

#define LIT_SET				32		/* <par> (eingebetteter Parameter)		Literal angegebenen Set auf den Stack
									 		eingebetter Parameter ist Typ des Sets (enum etp TP_SET_* */
									/* Fuer TP_SET_ALL, TP_SET_EMPTY:			-- set */
									/* Fuer TP_SET_HALF_LB, TP_SET_HALF_UB:		value -- set */
									/* Fuer TP_SET_ALG:							from step to -- set */
									/* Fuer TP_SET_ALG_S1:						from to -- set */
									/* Fuer TP_SET_ENUM:						val_1 .. val_n anz -- set */

#define CONSTRUCT_TUPEL		35		/* construct a new tupel or tupelset by tupel construction and push it on the value stack */
										/* val_1 .. val_n list_count -- tupel */
#define CONSTRUCT_TUPELSET	36		/* construct a new tupelset from an enumeration of tupels or tupelsets and push it on the value stack */
										/* tupel_1 .. tupel_2 list_count -- tupelset */ 
#define CONSTRUCT_TPS_REV	37		/* constructs a new tupelset from one value with setting for <code>_freeIndRev</code> */
										/* val -- tupelset */
	/* Unterkommando fuer CONSTRUCT_TPS_REV */
#define TPS_IND_FREE		0		/* treat scalar value as free */
#define TPS_IND_NON_FREE	1		/* treat set value as non-free */

#define TUPEL_DISAGGR_REV	38		/* disaggregate a tupel and push its parts in reverse order on the value stack */
										/* <num> (eingebetteter Parameter): rank which the tupel must have */
										/* tupel -- ind_n .. ind_1 */

#define TERM_BEG			40		/* Neuer Term */
#define TERM_VAR			41		/* <num>  Variable zum Term */
										/*  ind_n .. ind_1 --  */
#define TERM_VAR_FREE		42		/* <num>  Variablenvektor zum Term (nach SET_INDEXES) */
#define TERM_END			43		/* Ende Term, Koeffizient speichern, noetigenfalls Produkt aufloesen */
										/*  koeff_val --  */
#define TERM_NEG			44		/* 1.1.3 TS: Negiert den Negationsfaktor fuer Terme */

#define LINE_BEG			50		/* <name>  Neue Zeile beginnen */
#define LINE_MOD			51		/* <line_mod> (eingebetteter Parameter)  Zeile U, O, G, N */
#define LINE_END			52		/* Abspeichern erzeugte Zeile */
#define LINE_OBJ_TO			53		/* <num> (eingebetteter Parameter) 1.1.3 TS: Angabe Optimierungsrichtung */

#define NUM_OP				60		/* <sub>	Numerische Operation */
	/* Unterkommando fuer NUM_OP */
#define ADD					0		/* Addieren (auch Stringverkettung) */
												/* val1 val2 -- val1+val2 */
#define SUB 				1		/* Subtrahieren */
												/* val1 val2 -- val1-val2 */
#define MULT				2		/* Multiplizieren */
												/* val1 val2 -- val1*val2 */
#define DIVD				3		/* Dividieren */
												/* val1 val2 -- val1/val2 */
#define DIVI				4		/* Dividieren(int) */
												/* val1 val2 -- val1 DIV val2 */
#define MODI				5		/* Divisionsrest(int) */
												/* val1 val2 -- val1 MOD val2 */
#define POW					6		/* Potenzieren */
												/* val1 val2 -- val1^val2 */
#define NEG					7		/* Vorzeichen tauschen */
												/* val1 -- -val1 */

#define LOG_OP				61		/* <sub>	Logische Operation */
	/* Unterkommando fuer LOG_OP */
#define NOT					0		/* logisches Not */
												/* flag -- NOT(flag) */
#define AND					1		/* logisches And */
												/* flag1 flag2 -- flag1&&flag2 */
#define OR					2		/* logisches Or */
												/* flag1 flag2 -- flag1||flag2 */

#define SET_OP				62		/* <sub>	Mengenoperation */
	/* Unterkommando fuer SET_OP: wie fuer NUM_OP */
/*#define ADD				0*/		/* Vereinigung */
												/* val1 val2 -- val1+val2 */
/*#define MULT				2*/		/* Durchschnitt */
												/* val1 val2 -- val1*val2 */
#define INTERSECT_RR		10		/* Tupelset-Durchschnitt mit Rangreduktion */
												/* val1 val2 -- val1*>val2 */

#define CMP_OP				65		/* <sub>	Vergleichsoperation */
	/* Unterkommando fuer CMP_OP */
#define EQ					0		/* Gleichheit testen */
												/* val1 val2 -- flag */
#define EQ2					1		/* Gleichheit testen (bei Sets in Unterschied zu EQ auch Reihenfolge testen; sonst wie EQ) */
												/* val1 val2 -- flag */
#define GE					2		/* val1>=val2 testen */
												/* val1 val2 -- flag */
#define LE					3		/* val1<=val2 testen */
												/* val1 val2 -- flag */
#define GT					4		/* Nur fuer Sets val1>val2 testen */
												/* val1 val2 -- flag */
#define LT					5		/* Nur fuer Sets val1<val2 testen */
												/* val1 val2 -- flag */
#define INSET				6		/* Test ob Element val1 in Set val2 */
												/* val1 val2 -- flag */


#define COUNT_OP			66		/* <sub>	Zaehloperation */
	/* Unterkommando fuer COUNT_OP */
#define CNT_INC				0		/* Setzt Anzahl Elemente auf dem Stack um 1 hoch */
												/* anz val -- val anz+1 */


#define CONV_OP				70		/* <sub>	Ohne oder mit Formatangabe */
									/* <par>    Zieltyp als (short) enum etp */
	/* Unterkommando fuer CONV_OP */
#define NO_FORMAT			0		/* Keine Formatangabe */
										/* val -- conv(val) */
#define WITH_FORMAT			1		/* Mit Formatangabe (nur fuer Konvertierung in string) */
										/* val1 val2 -- format(val1,val2) */

#define TYPE_STR			71		/* <sub>	Typ-String fuer konstanten Ausdruck oder fuer Variable */
	/* Unterkommando fuer TYPE_STR */
#define TYPE_EXPR			0		/* Typ-String fuer konstanten Ausdruck */
										/* val -- val(string) */
#define TYPE_VAR			1		/* <num> Typ-String fuer Variable */
										/* ind_n .. ind_1 -- val(string) */

/* stegger 1.3.1 several math functions */
#define MATH_FUNC			74		/* <sub>	Mathematische Funktion */
										/* val_in -- val_out(double) */
	/* Unterkommando fuer MATH_FUNC */
#define F_SQRT				1		/* sqrt function */
#define F_EXP				2		/* exp function */
#define F_LOG				3		/* logarithmus naturalis */
#define F_LOG10				4		/* common logarithm  */
#define F_LOG2				5		/* logarithmus dualis  */
#define F_RAND				6		/* random */
#define F_SIN				7		/* sine measured in radians*/
#define F_COS				8		/* cosine measured in radians */
#define F_TAN				9		/* tangent measured in radians */
#define F_ACOS				10		/* arc cosine of in radians. */
#define F_ASIN				11		/* arc sine of in radians. */
#define F_ATAN				12		/* arc tangent of in radians. */
#define F_SINH				13		/* hyperbolic sine */
#define F_COSH				14		/* hyperbolic cosine */
#define F_TANH				15		/* hyperbolic tangent */
#define F_ABS				16		/* absolute value */
#define F_CEIL				17		/* smallest integer value greater than or equal to a given value */
#define F_FLOOR				18		/* largest integer value less than or equal to a given value */
#define F_ROUND				19		/* simple round */
#define F_SEED				20		/* seed of a random number */

#define AGGR_OP				75		/* <sub>	Aggregat-Operation */
	/* Unterkommando fuer AGGR_OP */
#define MAX_VAL				0		/* Maximum aus Werten */
										/* val_1 .. val_n anz -- max_val */
#define MIN_VAL				1		/* Minimum aus Werten */
										/* val_1 .. val_n anz -- min_val */
#define MAX_VAL_NULL		2		/* Wie MAX_VAL, aber auch 0 Werte erlaubt, Ergebnis ist dann TPP_NULL */
#define MIN_VAL_NULL		3		/* Wie MIN_VAL, aber auch 0 Werte erlaubt, Ergebnis ist dann TPP_NULL */

#define UTIL_OP				76		/* <sub>	Sonstige Operation */
	/* Unterkommando fuer UTIL_OP */
#define LEN_VAL				0		/* Laenge fuer einen einzelnen Wert: bei Set ist das Anzahl Elemente, sonst Anzahl Zeichen im String */
										/* val -- len */

#define DEF_FUNC			77		/* <num>  Wieviele Elemente der Matrix sind definiert (nach SET_INDEXES) */
										/*   -- value(int) */
#define DEFSET_FUNC			78		/* <num>  Definitionsset des ersten freien Index der Matrix (nach SET_INDEXES) */
										/*   -- value(set) */
#define DEFSET_EXTNUM		79		/* <num>  Hoechster oder niedrigster nummerischer Indexwert des ersten freien Index der Matrix (nach SET_INDEXES) */
										/*   -- value(int) */
	/* Unterkommando fuer DEFSET_EXTNUM: genau wie fuer AGGR_OP */

#define GOTO				80		/* <addr> Ausfuehrung ab addr */
	/* Unterkommando fuer GOTO */
#define GOTO_DIREKT			0		/* Direkter Sprung zu addr */
#define GOTO_INDIREKT		1		/* Indirekter Sprung: an addr wird weitere Adresse erwartet und zu dieser gesprungen */

#define LOOP_CTRL			85		/* Start oder Ende einer Bedingungsschleife */
	/* Unterkommando fuer GOTO */
#define START_LOOP			0		/* Start Schleife */
#define START_IF			1		/* Start Bedingung */
#define START_ASSIGN		2		/* Start lokale Definition */
#define START_EMPTY			3		/* Start Bedingungsschleife ohne Schleifenkopf */
										/* jeweils eingebetteter Parameter fuer Nummer des zu initialisierenden Iterationskontexts */
										/* jeweils int-Parameter fuer alle Modifikatoren (bitweise) */
										/* außer bei START_EMPTY Wert vom Stack (bei START_LOOP Set, bei START_IF binary) */
#define LOOP_END			9		/* Ende Schleife oder Bedingung */
	/* Modifikatoren fuer LOOP_CTRL: START* (als Zweierpotenzen) */
#define HEAD_MOD_REF		1		/* Fuer Zuweisung: Referenz zuweisen statt Werte (auch in Zuweisungen außerhalb Schleifenkopf) */
#define HEAD_MOD_WHILE		2		/* Fuer START_IF: Statt einmaliges Durchgehen solange iterieren, bis Bedingung nicht mehr erfuellt */


#define TEST_ITER			86		/* <addr1> <addr2> <addr3> Testen und Iteration fuer Schleife bzw. Bedingung. */
										/* eingebetteter Parameter fuer Nummer des Iterationskontexts */
										/* Ausfuehrung und Stacknutzung abhaengig von Art und Modifikatoren der letzten mit LOOP_CTRL gestarteten Bedingungsschleife */
										/* addr1: Start der Schleife, die hier iteriert wird */
										/* addr2: Sprungziel fuer Iterationsende (fuehrt immer auf LOOP_CTRL:LOOP_END) */
										/* addr3: Sprungziel fuer nicht erfuellte Bedingung (fuehrt zur naechsten alternativen Bedingung oder auf LOOP_CTRL:LOOP_END) */

#define TEST_ALTER			87		/* Test fuer alternative Bedingung */
	/* Unterkommando fuer TEST_ALTER */
#define TEST_ALTER_IF		0		/* <addr> Test fuer alternative Bedingung; Sprung zu addr, wenn nicht erfuellt */
										/* eingebetteter Parameter fuer Nummer des Iterationskontexts */
										/* flag --  */
#define TEST_ALTER_DEF		1		/* alternative Default-Bedingung (immer erfuellt, also kein Sprung vorzusehen) */
										/* eingebetteter Parameter fuer Nummer des Iterationskontexts */


#define VAR_LIN_BEG			91		/* Beginn Linearisierung Var.produkt */


#define OUTPUT_MSG			93		/* output user defined message  */
	/* Unterkommando fuer USR_MSG */
/* ERROR_LVL_NORMAL, ERROR_LVL_FATAL:  Nutzerdefinierte Fehlermeldung ausgeben, bei ERROR_LVL_FATAL danach Abbruch */
										/* val -- */
#define USR_MSG				10		/* Meldung ausgeben */
										/* val -- */
#define USR_MSG_NL			11		/* Meldung ausgeben, mit abschliessendem new line */
										/* val -- */
#define ONLY_NL				12		/* Nur Ausgabe new line */

/* 1.1.3 TS: SRC_FILE fuer include hinzugefuegt, SRC_LINE hierher verschoben */
#define SRC_FILE			98		/* <string>  CMPL file name */
#define SRC_LINE			99		/* <int>  Aktuelle Quelltextzeile */


#define CODE_TABLE_COUNT	100



/* Definition der Beschreibung der Zwischencodebefehle */
/*  Bezeichnung,	Funktion zur Ausfuehrung */
#define INIT_CODE_TABLE		{	\
    {	"PROG_END",			new IntFuncProgEnd(glvar)			},		/* 0 */			\
	{	"DEF_CONST",		new IntFuncDefConst(glvar)			},		\
	{	"INIT_CMPLDATA",	new IntFuncInitCmplData(glvar)		},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"DEF_VAR",			new IntFuncDefVar(glvar)			},		\
	{	"RESET_VAR_B",		new IntFuncResetVarB(glvar)			},		\
	{	"DEF_DN_VAR",		new IntFuncDefDnVar(glvar)			},		\
	{	"DEF_UP_VAR",		new IntFuncDefUpVar(glvar)			},		\
	{	"DEF_VAR_TYPE",		new IntFuncDefVarType(glvar)		},		\
		\
    {	"SET_INDEXES",		new IntFuncSetIndexes(glvar)		},		/* 10 */		\
	{	"",						NULL							},		\
	{	"DEL_INDEXES",		new IntFuncDelIndexes(glvar)		},		\
	{	"IND_TRANSPOSE",	new IntFuncIndTranspose(glvar)		},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"READ_CSV",			new IntFuncReadCSV(glvar)			},		\
	{	"READ_STDIN",		new IntFuncReadStdin(glvar)			},		\
	{	"",						NULL							},		\
	{	"CMPL_SOLVE",		new IntFuncCmplSolve(glvar)			},		\
		\
    {	"FETCH_CONST",		new IntFuncFetchConst(glvar)		},		/* 20 */		\
	{	"F_CONST_FREE",		new IntFuncFConstFree(glvar)		},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"FETCH_TMP",		new IntFuncFetchTmp(glvar)			},		\
	{   "STORE_TMP",        new IntFuncStoreTmp(glvar)			},		\
	{   "",                     NULL         					},		\
	{	"FETCH_VALS",		new IntFuncFetchVals(glvar)			},		\
	{	"",						NULL							},		\
		\
    {	"LIT_VAL",			new IntFuncLitVal(glvar)			},		/* 30 */		\
	{	"LIT_VAL_SHORT",	new IntFuncLitValShort(glvar)		},		\
	{	"LIT_SET",			new IntFuncLitSet(glvar)			},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"CONSTRUCT_TUPEL",	new IntFuncConstructTupel(glvar)	},		\
	{	"CONSTRUCT_TUPELSET",new IntFuncConstructTupelset(glvar)},		\
	{	"CONSTRUCT_TPS_REV",new IntFuncConstructTupelsetFreeIndRev(glvar)},	\
	{	"TUPEL_DISAGGR_REV",new IntFuncDisaggrTupelRev(glvar)	},		\
	{	"",						NULL							},		\
		\
    {	"TERM_BEG",			new IntFuncTermBeg(glvar)			},		/* 40 */ 		\
	{	"TERM_VAR",			new IntFuncTermVar(glvar)			},		\
	{	"TERM_VAR_FREE",	new IntFuncTermVarFree(glvar)		},		\
	{	"TERM_END",			new IntFuncTermEnd(glvar)			},		\
	{	"TERM_NEG",			new IntFuncTermNegFak(glvar)		},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
		\
    {	"LINE_BEG",			new IntFuncLineBeg(glvar)			},		/* 50 */		\
	{	"LINE_MOD",			new IntFuncLineMod(glvar)			},		\
	{	"LINE_END",			new IntFuncLineEnd(glvar)			},		\
	{	"LINE_OBJ_TO",		new IntFuncLineObjTo(glvar)			},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
		\
    {	"NUM_OP",			new IntFuncNumOp(glvar)				},		/* 60 */		\
	{	"LOG_OP",			new IntFuncLogOp(glvar)				},		\
	{	"SET_OP",			new IntFuncSetOp(glvar)				},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"CMP_OP",			new IntFuncCmpOp(glvar)				},		\
	{	"COUNT_OP",			new IntFuncCountOp(glvar)			},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
		\
    {	"CONV_OP",			new IntFuncConvOp(glvar)			},		/* 70 */		\
	{	"TYPE_STR",			new IntFuncTypeStr(glvar)			},		\
	{	"",						NULL							},		\
    {	"",						NULL							},		\
    {	"MATH_FUNC",		new IntFuncMathFunc(glvar)			},		\
	{	"AGGR_OP",			new IntFuncAggrOp(glvar)			},		\
	{	"UTIL_OP",			new IntFuncUtilOp(glvar)			},		\
	{	"DEF_FUNC",			new IntFuncDefFunc(glvar)			},		\
	{	"DEFSET_FUNC",		new IntFuncDefsetFunc(glvar)		},		\
	{	"DEFSET_EXTNUM",	new IntFuncDefsetExtnum(glvar)		},		\
		\
    {	"GOTO",				new IntFuncGoto(glvar)				},		/* 80 */		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"LOOP_CTRL",		new IntFuncLoopCtrl(glvar)			},		\
	{	"TEST_ITER",		new IntFuncTestIter(glvar)			},		\
	{	"TEST_ALTER",		new IntFuncTestAlter(glvar)			},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
		\
    {	"",						NULL							},		/* 90 */		\
	{	"VAR_LIN_BEG",		new IntFuncVarLinBeg(glvar)			},		\
	{	"",						NULL							},		\
	{	"OUTPUT_MSG",		new IntFuncOutputMsg(glvar)			},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"",						NULL							},		\
	{	"SRC_FILE",			new IntFuncSrcFile(glvar)			},		\
	{	"SRC_LINE",			new IntFuncSrcLine(glvar)			}		\
}


#endif // !GENCODE_H

