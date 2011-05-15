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
 * Texte, fuer sprachabhaengige Anpassungen.
 *
 *************************************************************************************/

#ifndef GENTEXT_H
#define GENTEXT_H

 
#ifdef LANG_DE

/* Info zum Aufruf */
#define USAGE_INFO_CALL		"\nAufruf: %s [Optionen] [<Quelldatei>]\n"
#define USAGE_INFO_OPT		"\nOptionen:"



#define USAGE_INFO_GENERAL_OPTS     "\n Allgemeine Optionen:"

#ifdef SOLVERSERVICE
    #define USAGE_INFO_SOLVER_MODE      "\n Solver Modus:"
#endif

#define USAGE_INFO_MODEL_MODE       "\n Modell Modus:"


#define USAGE_INFO_INPUT		"\n -i <File>           : Quelldatei"
#define USAGE_INFO_OUT_M		"\n -m [<File>]         : Ausgabe des generierten Problems als MPS in File oder Standardausgabe"
#define USAGE_INFO_OUT_XML	"\n -x [<File>]         : Ausgabe des generierten Problems als OSiL XML File oder Standardausgabe" /* stegger 1.2.3 */

#define USAGE_INFO_OUT_E		"\n -e [<File>]         : Ausgabe Fehler und Warnungen " \
                                 "\n                       -e Ausgabe auf Standardfehlerausgabe" \
                                 "\n                       -e<File> Ausgabe in MplR XML format in Datei "

#define USAGE_INFO_OUT_Z		"\n -z [<File>]         : Ausgabe Modell mit Zielvariablennamen und ohne Konstanten"
#define USAGE_INFO_OUT_T		"\n -t [<File>]         : Ausgabe als Matrix (in Datei dann als CSV)"
#define USAGE_INFO_OUT_V		"\n -v [<File>]         : Ausgabe Zuordnung Variablennamen"

#define USAGE_INFO_OUT_L		"\n -l [<File>]         : Ausgabe Ersetzungen fuer Variablenprodukte"
#define USAGE_INFO_OUT_Y		"\n -y [<File>]         : Ausgabe Parse-Info (welche syntaktische Struktur wo)"
#define USAGE_INFO_OUT_S		"\n -s [<File>]         : Statistikausgabe"
#define USAGE_INFO_OUT_P		"\n -p [<File>]         : Protokollausgabe"
#define USAGE_INFO_GEN_NORED		"\n -gn                 : Keine Matrixreduktionen"
#define USAGE_INFO_GEN_FPROD		"\n -gf                 : Restriktionen fuer Variablenprodukte direkt hinter ausloesende Restriktionen"
#define USAGE_INFO_CONST_MDEF		"\n -cd                 : Keine Warnung bei doppelter Definition eines Parameters"
#define USAGE_INFO_ASSIGN_OLD		"\n -ca                 : Keine Warnung bei Verwendung von '=' als Zuweisungsoperator"
#define USAGE_INFO_CONST_INTOP		"\n -ci<X>              : Modus fuer ganzzahlige Ausdruecke (0 - 3), Standard ist %d"
#define USAGE_INFO_FRM_REM		"\n -fc<X>              : <X> ist maximale Anzahl Zeichen im Kommentar, Standard ist %d"

#define USAGE_INFO_FRM_FREEMPS		"\n -ff                 : Matrix im Free-MPS Format generieren"
#define USAGE_INFO_FRM_NUMFORMAT	"\n -f%%<format>        : Formatkennzeichen fuer Ausgabe Fliesskommazahlen, Standard ist %%f"
#define USAGE_INFO_VERSION		"\n -v                  : CMPL Version"



#ifdef SOLVERSERVICE

    #define USAGE_INFO_NOOUTPUT      "\n -noOutput           : Keine Generierung einer MPS- oder OSiL-Datei"
    #define USAGE_INFO_SOLVE         "\n -solve              : Lösen des Problems " \
                                     "\n                       Die Ergebnisse werden in ein OSrL XML file geschrieben" \
                                     "\n                       Eine Datei <cmplFileName>.osrl wird erzeugt."
    #define USAGE_INFO_SOLVER        "\n -solver <solver>    : Name des Solvers, den Sie verwenden möchten."
    #define USAGE_INFO_SOLVERLOCATION  "\n -solverUrl <url>    : URL eines Solver Services" \
                                       "\n                       Ohne Angabe wird das Problem lokan gelöst."
    #define USAGE_INFO_OSOL          "\n -osol <file>        : Dateiname einer Datei, die die Solverparameter enthält."
    #define USAGE_INFO_CSV           "\n -solutionCsv        : Optimierungsergebnisse in CSV format"\
                                 "\n                       Eine Datei <cmplFileName>.csv wird erzeugt."
    #define USAGE_INFO_ASCII           "\n -solutionAscii      : Optimierungsergebnisse in ASCII format"\
                             "\n                       Eine Datei <cmplFileName>.sol wird erzeugt."
    #define USAGE_INFO_OBJ           "\n -obj <objName>      : Name der Zielfunktion  "
    #define USAGE_INFO_OBJ_SENSE     "\n -objSense <max/min> : Maximierung oder Minimierung"

#endif //SOLVERSERVICE

#define USAGE_INFO_ERROR		"\nFehler %s%s"
#define USAGE_INFO_ERROR_AT		"\nFehler bei %s: %s"

#define USAGE_ERROR_PARAM		"Ungueltiger Aufrufparameter"
#define USAGE_ERROR_INP_MULT		"Mehrfache Angabe Eingabedatei"
#define USAGE_ERROR_INP_NO		"Keine Eingabedatei"

#ifdef SOLVERSERVICE

    #define USAGE_ERROR_ARG_NO              "Kein Wert fuer Argument angegeben"
    #define USAGE_ERROR_OBJSENSE            "Falsches Optimierungsziel"
    #define USAGE_ERROR_MODE                "Solver Modus und Modell Modus koennen nicht gleichzeitig verwendet werden."

#endif

#define USAGE_ERROR_OUT_MULT		"Mehrfache Angabe Ausgabedatei"
#define USAGE_ERROR_GEN				"Ungueltiger Parameter zur Generierung"
#define USAGE_ERROR_FRM				"Ungueltiger Parameter zum Ausgabeformat fuer die generierte Datei"
#define USAGE_ERROR_CONST			"Ungueltiger Parameter fuer Konstanten"


#define USAGE_VERSION_OUT			"CMPL Version %s\n"

#ifdef SOLVERSERVICE
    #define MSG_CMPL_STARTED                 "CMPL Model Generierung - gestartet"
    #define MSG_CMPL_FINISHED                "CMPL Model Generierung - beendet"
    #define MSG_CMPL_FAILED                  "CMPL Model Generierung - mit Fehlern beendet"
    #define MSG_OSS_STARTED                 "OSSolverServices       - gestartet"
    #define MSG_OSS_FINISHED                "OSSolverServices       - beendet"
    #define MSG_OSS_FAILED                  "OSSolverServices       - mit Fehlern beendet"
#endif


/* Fehlermeldungen */
#define ERROR_LINE_SYMBOL			"Symbol '%s' in Zeilenname '%s' nicht definiert"
#define ERROR_LINE_NAME_LEN			"Zeilenname '%s' zu lang"
#define ERROR_LINE_NAME_DUP_REPL	"Zeilenname '%s' doppelt: durch '%s' ersetzt"
#define ERROR_VARPROD_MAX			"Zuviele Variablen in einem Produkt"
#define ERROR_VARPROD_1				"Produkt aus Variablen"
#define ERROR_SYMBOL_DEF			"Symbol '%s' bereits definiert"
#define ERROR_SYMBOL_NODEF			"Symbol '%s' nicht definiert"
#define ERROR_SYMBOL_INDEF			"Symbol '%s' ist noch nicht fertig definiert"
#define ERROR_SYMBOL_TPK_MIX		"Symbol '%s' ist bereits für andere Typklasse definiert"
#define ERROR_ASSIGN_OLD			"Verwendung veralteter Zuweisungsoperator '='"
#define ERROR_LOOP_MAX				"Zu tief geschachtelte Bedingungen/Schleifen"
#define ERROR_INDIZES_MAX			"Zu viele Indizes"								
#define ERROR_IMPORT_PAR_MAX		"Zuviele Parameter bei IMPORT"
#define ERROR_CONST_DEF_DIM			"Dimensionen bei Konstantendefinition stimmen nicht ueberein"
#define ERROR_CONST_DEF_TPK_MIX		"Mischung zwischen Werten verschiedener Typklassen in einer Konstantendefinition" 
#define ERROR_CONST_DEF_SET_ELEM	"Ein Set kann nur einfache Werte enthalten, keine anderen Sets" 
#define ERROR_NODEF_CONST			"Konstante '%s' nicht definiert"
#define ERROR_DEF_CONST				"Konstante '%s' bereits definiert"
#define ERROR_DEFTYPE_CONST			"Konstante '%s' bereits für anderen Typ definiert"
#define ERROR_NODEF_VAR				"Variable '%s' nicht definiert"
#define ERROR_DEF_VAR				"Variable '%s' bereits definiert"
#define ERROR_USED_VAR				"Variable '%s' bereits benutzt"
#define ERROR_DEF_VAR_KONST			"Produktvariablenname '%s' bereits als Parameter definiert"
#define ERROR_DEF_VAR_LK			"Produktvariablenname '%s' bereits als Laufkonstante definiert"
#define ERROR_DEF_RVAR_KONST		"Range-Variablenname '%s' bereits als Parameter definiert"
#define ERROR_DEF_RVAR_LK			"Range-Variablenname '%s' bereits als Laufkonstante definiert"
#define ERROR_MATRIX_FREE_IND		"Zu viele freie Indizes in Matrix '%s'"
#define ERROR_MATRIX_PROD			"Fehlerhaftes Matrixprodukt bei '%s'"
#define ERROR_MATRIX_PROD_MAX		"Zu viele Matrixprodukte bei '%s'"
#define ERROR_MATRIX_PROD_UNV		"Unvollstaendiges Matrixprodukt"
#define ERROR_VEKTOR_LEN			"Falsche Vektorgroesse bei '%s'"
#define ERROR_VEKTOR_NO_LEN			"Vektorgroesse fuer %d. Index von '%s' nicht angegeben"
#define ERROR_VEKTOR_ELEM			"Anzahl Elemente fuer %d. Index von '%s' stimmt nicht"
#define ERROR_KOMPLEX_DN_NODEF		"Untere Schranke fuer nicht definierte Konstante %s"
#define ERROR_KOMPLEX_UP_NODEF		"Obere Schranke fuer nicht definierte Konstante %s"
#define ERROR_KOMPLEX_DN_DUPDEF		"Untere Komplexschranke %s bereits definiert"
#define ERROR_KOMPLEX_UP_DUPDEF		"Obere Komplexschranke %s bereits definiert"
#define ERROR_REK_SET_IND_MAX		"Zu tief geschachtelte Indizes"
#define ERROR_VARPROD_IND_MAX		"Variablenprodukt '%s * %s' hat zuviele Indizes"
#define ERROR_VARPROD_REAL_WARN		"Variablenprodukt '%s * %s': Produkt von real-Variablen (nur Naeherung moeglich)"
#define ERROR_VARPROD_NO_LB			"Variablenprodukt: keine untere Schranke fuer %s"
#define ERROR_VARPROD_NO_UB			"Variablenprodukt: keine obere Schranke fuer %s"
#define ERROR_LOOP_NOEND			"Endlosschleife"
#define ERROR_LOOP_WHILE			"while-Modifikator ist nur fuer Bedingungen erlaubt"
#define ERROR_LOOP_ALT_EMPTY		"Alternativen sind in Bedingungen/Schleifen ohne Kopf nicht erlaubt"
#define ERROR_LOOP_ALT_NEVER		"Alternative in Bedingung/Schleife wird nie erreicht"
#define ERROR_LOOP_CTRL_REF			"Unbekannte Schleifenreferenz '%s' bei %s"
#define ERROR_LOOP_ONCE_CONT		"continue ist nur für Schleifen erlaubt, die über eine Set iterieren"
#define ERROR_NOT_A_NUMBER_PARSE	"Operand fuer nummerische Operation ist nicht nummerisch"
#define ERROR_NOT_A_NUMBER			"Wert '%s' ist keine Zahl"
#define ERROR_NOT_AN_INDEX			"Wert '%s' ist kein Indextyp"
#define ERROR_NOT_TPK_VALUE			"Wert von '%s' ist kein einfacher Wert"
#define ERROR_NOT_TPK_SET			"Wert von '%s' ist keine Menge"
#define ERROR_CONV_IMPL				"Implizite Konvertierung von %s in %s ist nicht möglich"
#define ERROR_CONV_EXPL				"Konvertierung von %s in %s ist nicht erlaubt"
#define ERROR_INFINITE_SET			"Unendliche Menge kann nicht iteriert werden"
#define ERROR_EMPTY_SET_FREE_IND	"Leere Menge ist fuer freien Index fuer %s nicht erlaubt"
#define ERROR_SET_NOT_SUPPORTED		"Ergebnis der Mengenvereinigung ist keine darstellbare Menge"
#define ERROR_CMP_TPK_MIX			"Mischung zwischen verschiedenen Typklassen in einer Vergleichsoperation" 
#define ERROR_DIV_0					"Division durch 0"
#define ERROR_MAX_0					"Maximum von 0 Werten"
#define ERROR_MIN_0					"Minimum von 0 Werten"
#define ERROR_SET_STEP_0			"Schrittweite 0 fuer algorithmischen Set"
#define ERROR_MULT_OBJECTIVE		"Mehr als eine zu optimierende Zeile, nur die erste wird beruecksichtigt"
#define ERROR_INTERN_NO_FUNC		"Interner Fehler: Unbekannte Zwischencodefunktion"
#define ERROR_INTERN_PRV_SET_IND	"Interner Fehler: Kein vorhergegangenes SET_INDEXES"
#define ERROR_INTERN_STACK_UF		"Interner Fehler: Stackunterlauf"
#define ERROR_INTERN_NO_FREE_IND	"Interner Fehler: Kein freier Index %s"
#define ERROR_INTERN_KOMPLEX_LEN	"Interner Fehler: Fehlerhafte Matrixgroesse bei Matrix fuer Komplexschranken"
#define ERROR_INTERN_INCL_STACK		"Interner Fehler: Include-Stackunterlauf"
#define ERROR_INTERN_INCL_CHDIR		"Interner Fehler: Include-Verzeichniswechsel"
#define ERROR_NO_MEMORY				"Zuwenig Speicherplatz"
#define ERROR_INT_MAX_MIN			"Integer ausserhalb Bereich"
#define ERROR_BOUND_DN_NO			"Keine untere Schranke fuer '%s' : %ld eingesetzt"
#define ERROR_BOUND_UP_NO			"Keine untere Schranke fuer '%s' : %ld eingesetzt"
#define ERROR_BOUND_UP_IGNORE		"Obere Schranke wird ignoriert"
#define ERROR_BOUND_DN_UP_COMP		"Untere Schranke (%lf) groesser als obere (%lf)"
#define ERROR_BOUND_UP_DN_COMP		"Obere Schranke (%lf) kleiner als untere (%lf)"
#define ERROR_BOUND_CONSTRAINT		"Fuer Restriktion %s untere Schranke groesser als obere"
#define ERROR_VAR_DUP_REPL			"Variablenname '%s' doppelt: durch '%s' ersetzt"
#define ERROR_VAR_NAME_LEN			"Variablenname '%s' zu lang"
#define ERROR_IMPORT_CALL			"Fehler bei Aufruf IMPORT-Funktion"
#define ERROR_IMPORT_KOLIOP_TYPE	"Nur IMPORT von double-Matrizen moeglich"
#define ERROR_IMPORT_KOLIOP_PAR		"Fehlerhafte Parameter bei IMPORT von KOLIOP"
#define ERROR_INCLUDE_MAX			"Zu tief geschachtelte Include-Dateien"								
#define ERROR_PATH_TOO_LONG			"Pfadname zu lang"
#define ERROR_FILE_WRITE			"Schreibfehler bei Datei '%s'"
#define ERROR_FILE_READ				"Lesefehler bei Datei '%s'"
#define ERROR_FILE_INCL_DIR_NF		"Verzeichnis '%s' fuer Include-Datei nicht gefunden"
#define ERROR_FILE_INPUT_INCL		"Include-Datei '%s' nicht gefunden"
#define ERROR_FILE_INPUT_OPEN		"Quelldatei '%s' nicht gefunden\n"
/* stegger 1.3.1.b csv import */
#define ERROR_FILE_CSV_DIM			"CSV hat '%s' hat falsche Dimension\n"
#define ERROR_FILE_OUT_OPEN			"Ausgabedatei '%s' fuer -%c nicht zu oeffnen\n"
#define ERROR_COUNT					"\n%d Fehler gefunden\n"
/* stegger 1.3.1 math function error */
#define ERROR_MATH_FUNC				"Fehler in mathematischer Funktion %s"
#define ERROR_CONST_IN_OBJ_FUNC			"Unkorrekte Zielfunktion - Konstanter Werte enthalten, der ignoriert wird."
#define ERROR_NO_MODEL			"Es wurde kein Model erzeugt"



/* Fehlerausgabe */
#define ERROR_TYP_TXT_USER			"Nutzerfehler"
#define ERROR_TYP_TXT_SONST			"Fehler"
#define ERROR_TYP_TXT_SONST_FATAL	"Fataler Fehler"
#define ERROR_TYP_TXT_SONST_WARN	"Warnung"
#define ERROR_TYP_TXT_COMP			"Fehler (Compiler)"
#define ERROR_TYP_TXT_INT			"Fehler (Interpreter)"
#define ERROR_TYP_TXT_IO			"Fehler (Ein/Ausgabe)"

#define OUTTXT_ERROR				"%s%s: %s%s"
#define OUTTXT_ERROR_ZEILE			"%s%s: Datei %s Zeile %d: %s%s"
#define OUTTXT_ERROR_TOKEN			"%s%s: Datei %s Zeile %d bei '%s': %s%s"

/* Liste der Variablenproduktzerlegung */
#define OUTTXT_PRODLIST_PROD		"Produkt-Zerlegung: %s := %s * %s\n"
#define OUTTXT_PRODLIST_INT			"Integer-Zerlegung: %s := %s\n"
#define OUTTXT_PRODLIST_REAL		"Real-Normierung:   %s := %s\n"

/* Statistik-Ausgabe */
#define OUTTXT_STATISTIC			"Zielmatrix %s:\n" \
"%d Spalten (Variablen), %d Zeilen (Restriktionen)\n" \
"%d Koeffizienten ungleich 0 (von %d) (%.1lf %%)\n"

#else

/* Info zum Aufruf */
#define USAGE_INFO_CALL		"\nUsage: %s [options] [<input file>]\n"
#define USAGE_INFO_OPT		"\nOptions:"



#define USAGE_INFO_GENERAL_OPTS     "\n General options:"

#ifdef SOLVERSERVICE
    #define USAGE_INFO_SOLVER_MODE      "\n Solver mode:"
#endif

#define USAGE_INFO_MODEL_MODE       "\n Model mode:"


#define USAGE_INFO_INPUT		"\n -i <cmplFile>       : input file "
#define USAGE_INFO_OUT_M		"\n -m [<File>]         : export model in MPS format in a file or stdout"
#define USAGE_INFO_OUT_XML              "\n -x [<File>]         : export model in OSiL XML format in a file or stdout" /* stegger 1.2.3 */

#define USAGE_INFO_OUT_E		"\n -e [<File>]         : output for error messages and warnings " \
                                        "\n                       -e simple output to stderr" \
                                        "\n                       -e<File> ouput in MprL XML format to file"

#define USAGE_INFO_OUT_Z		"\n -z [<File>]         : output for generated modell"
#define USAGE_INFO_OUT_T		"\n -t [<File>]         : output for generated matrix values"
#define USAGE_INFO_OUT_V		"\n -v [<File>]         : output for replacements of variable names"

#define USAGE_INFO_OUT_L		"\n -l [<File>]         : output for replacements for products of variables"
#define USAGE_INFO_OUT_Y		"\n -y [<File>]         : output for parsed structure of the input"
#define USAGE_INFO_OUT_S		"\n -s [<File>]         : output for short statistic info"
#define USAGE_INFO_OUT_P		"\n -p [<File>]         : output for protocol"
#define USAGE_INFO_GEN_NORED		"\n -gn                 : generation option: do not make reductions"
#define USAGE_INFO_GEN_FPROD		"\n -gf                 : generation option: constraints for products of variables follow the product"
#define USAGE_INFO_CONST_MDEF		"\n -cd                 : no warning at multiple parameter definition"
#define USAGE_INFO_ASSIGN_OLD		"\n -ca                 : no warning at deprecated '=' assignment"
#define USAGE_INFO_CONST_INTOP		"\n -ci<X>              : mode for integer expressions (0 - 3), defaults to %d"
#define USAGE_INFO_FRM_REM		"\n -fc<X>              : format option: maximal length of comment, defaults to %d"

#define USAGE_INFO_FRM_FREEMPS		"\n -ff                 : format option: generate free MPS"
#define USAGE_INFO_FRM_NUMFORMAT	"\n -f%%<format>         : format option: format spezifier for float number output, defaults to %%f"
#define USAGE_INFO_VERSION		"\n -v                  : version"


#ifdef SOLVERSERVICE

    #define USAGE_INFO_NOOUTPUT		"\n -noOutput           : no generating of a MPS or OSiL file"
    #define USAGE_INFO_SOLVER           "\n -solver <solver>    : name of the solver you want to use"
    #define USAGE_INFO_SOLVERLOCATION   "\n -solverUrl <url>    : URL of the solver service" \
                                        "\n                       w/o a defined remote solver service, a local solver is used"
    #define USAGE_INFO_OSOL             "\n -osol <file>        : name of the file that contains the solver options"
    #define USAGE_INFO_CSV              "\n -solutionCsv        : optimization results in CSV format "\
                                        "\n                       A file <cmplFileName>.csv will be created."

#define USAGE_INFO_ASCII                  "\n -solutionAscii      : optimization results in ASCII format "\
                                        "\n                       A file <cmplFileName>.sol will be created."

    #define USAGE_INFO_SILENT           "\n -silent             : optimization results are not displayed "
    #define USAGE_INFO_OBJ              "\n -obj <objName>      : name of the objective function  "
    #define USAGE_INFO_OBJ_SENSE        "\n -objSense <max/min> : objective sense"

#endif //SOLVERSERVICE

#define USAGE_INFO_HELP                 "\n -h                  : get this help"
#define USAGE_INFO_BLANK                "\n"


#define USAGE_INFO_ERROR		"\nError %s%s"
#define USAGE_INFO_ERROR_AT		"\nError at %s: %s"

#define USAGE_ERROR_PARAM		"Invalid option"
#define USAGE_ERROR_INP_MULT		"Multiple input file"
#define USAGE_ERROR_INP_NO		"No input file"

#ifdef SOLVERSERVICE

    #define USAGE_ERROR_ARG_NO              "No value for argument defined"
    #define USAGE_ERROR_OBJSENSE            "Wrong objective sense"
    #define USAGE_ERROR_MODE                "Don´t use solver and model mode at the same time"

#endif

#define USAGE_ERROR_OUT_MULT		"Multiple output file"
#define USAGE_ERROR_GEN			"Invalid generation option"
#define USAGE_ERROR_FRM			"Invalid format option"
#define USAGE_ERROR_CONST		"Invalid option for parameters"

#define USAGE_VERSION_OUT		"CMPL version %s\n"

#ifdef SOLVERSERVICE
    #define MSG_CMPL_STARTED                 "CMPL model generating - running"
    #define MSG_CMPL_FINISHED                "CMPL model generating - finished"
    #define MSG_CMPL_FAILED                  "CMPL model generating - failed"
    #define MSG_OSS_STARTED                 "OSSolverServices      - running"
    #define MSG_OSS_FINISHED                "OSSolverServices      - finished"
    #define MSG_OSS_FAILED                  "OSSolverServices      - failed"
#endif

/* Fehlermeldungen */
#define ERROR_LINE_SYMBOL		"Invalid symbol '%s' in line name '%s'"
#define ERROR_LINE_NAME_LEN		"Line name '%s' is too long"
#define ERROR_LINE_NAME_DUP_REPL         "Line name '%s' is not unique: replaced by '%s'"
#define ERROR_VARPROD_MAX		"Too much variables in a product"
#define ERROR_VARPROD_1				"Products of variables are not allowed"
#define ERROR_SYMBOL_DEF			"Name '%s' is already defined"
#define ERROR_SYMBOL_NODEF			"Name '%s' is not defined"
#define ERROR_SYMBOL_INDEF			"Name '%s' is not complete defined"
#define ERROR_SYMBOL_TPK_MIX		"Name '%s' is already defined with another type class"
#define ERROR_ASSIGN_OLD			"Use of deprecated assignment with '='"
#define ERROR_LOOP_MAX				"Too much loops"
#define ERROR_INDIZES_MAX			"Too much indexes"								
#define ERROR_IMPORT_PAR_MAX		"Too much parameters for IMPORT"
#define ERROR_CONST_DEF_DIM			"Dimensions in parameter definition don't match"
#define ERROR_CONST_DEF_TPK_MIX		"Mix of different type classes in one parameter definition" 
#define ERROR_CONST_DEF_SET_ELEM	"A set can contain only integers and strings, but no other sets" 
#define ERROR_NODEF_CONST			"Constant '%s' is not defined"
#define ERROR_DEF_CONST				"Constant '%s' is already defined"
#define ERROR_DEFTYPE_CONST			"Constant '%s' is already defined with another type"
#define ERROR_NODEF_VAR				"Variable '%s' is not defined"
#define ERROR_DEF_VAR				"Variable '%s' is already defined"
#define ERROR_USED_VAR				"Variable '%s' is already used"
#define ERROR_DEF_VAR_KONST			"Name for product of variables '%s' is already defined as a parameter"
#define ERROR_DEF_VAR_LK			"Name for product of variables '%s' is already defined as an iteration parameter"
#define ERROR_DEF_RVAR_KONST		"Name for variable for range '%s' is already defined as a parameter"
#define ERROR_DEF_RVAR_LK			"Name for variable for range '%s' is already defined as an iteration parameter"
#define ERROR_MATRIX_FREE_IND		"Too much free indexes in matrix '%s'"
#define ERROR_MATRIX_PROD			"Error in matrix product at '%s'"
#define ERROR_MATRIX_PROD_MAX		"Too much matrix products at '%s'"
#define ERROR_MATRIX_PROD_UNV		"Incomplete matrix product"
#define ERROR_VEKTOR_LEN			"Vector length don't match at '%s'"
#define ERROR_VEKTOR_NO_LEN			"Vector length for the %d. index of '%s' missing"
#define ERROR_VEKTOR_ELEM			"Count of elements for the %d. index of '%s' is wrong"
#define ERROR_KOMPLEX_DN_NODEF		"Lower complex bound for non defined constant '%s'"
#define ERROR_KOMPLEX_UP_NODEF		"Upper complex bound for non defined constant '%s'"
#define ERROR_KOMPLEX_DN_DUPDEF		"Lower complex bound for '%s' is already defined"
#define ERROR_KOMPLEX_UP_DUPDEF		"Upper complex bound for '%s' is already defined"
#define ERROR_REK_SET_IND_MAX		"Too much recursiv indexes"
#define ERROR_VARPROD_IND_MAX		"Product of variables '%s * %s' has too much indexes"
#define ERROR_VARPROD_REAL_WARN		"Product of variables '%s * %s': both are real variables, no exact product is possible"
#define ERROR_VARPROD_NO_LB			"Product of variables: no lower bound for '%s'"
#define ERROR_VARPROD_NO_UB			"Product of variables: no upper bound for '%s'"
#define ERROR_LOOP_NOEND			"Unbounded loop"
#define ERROR_LOOP_WHILE			"while-modifier is not possible here"
#define ERROR_LOOP_ALT_EMPTY		"Alternative is not possible here"
#define ERROR_LOOP_ALT_NEVER		"Alternative in loop is never reached"
#define ERROR_LOOP_CTRL_REF			"Loop reference '%s' at %s not found"
#define ERROR_LOOP_ONCE_CONT		"continue is only valid in iterating loops"
#define ERROR_NOT_A_NUMBER_PARSE	"Operand for numerical operation is not a number"
#define ERROR_NOT_A_NUMBER			"Value '%s' is not a number"
#define ERROR_NOT_AN_INDEX			"Value '%s' is not a valid type for an index"
#define ERROR_NOT_TPK_VALUE			"Value of '%s' is not a simple value"
#define ERROR_NOT_TPK_SET			"Value of '%s' is not a set"
#define ERROR_CONV_IMPL				"Implicit type conversion from %s to %s is not possible"
#define ERROR_CONV_EXPL				"Type conversion from %s to %s is not permitted"
#define ERROR_INFINITE_SET			"Infinite set cannot be iterated"
#define ERROR_EMPTY_SET_FREE_IND	"Empty set for a free index at %s is not permitted"
#define ERROR_SET_NOT_SUPPORTED		"Result of set operation is a not supported set"
#define ERROR_CMP_TPK_MIX			"Mix of different type classes in a compare operation" 
#define ERROR_DIV_0					"Division by 0"
#define ERROR_MAX_0					"Maximum of 0 values"
#define ERROR_MIN_0					"Minimum of 0 values"
#define ERROR_SET_STEP_0			"Step value of 0 for algorithmic set"
#define ERROR_MULT_OBJECTIVE		"More than one line to optimize, only the first is used"
#define ERROR_INTERN_NO_FUNC		"Intern error: unknown intermediate function"
#define ERROR_INTERN_PRV_SET_IND	"Intern error: no previous SET_INDEXES"
#define ERROR_INTERN_STACK_UF		"Intern error: stack underflow"
#define ERROR_INTERN_NO_FREE_IND	"Intern error: no free index at '%s'"
#define ERROR_INTERN_KOMPLEX_LEN	"Intern error: wrong matrix length for complex bounds"
#define ERROR_INTERN_INCL_STACK		"Intern error: include file stack underflow"
#define ERROR_INTERN_INCL_CHDIR		"Intern error: change directory for include file"
#define ERROR_NO_MEMORY				"Out of memory"
#define ERROR_INT_MAX_MIN			"Integer parameter too high or too low for integer values"
#define ERROR_BOUND_DN_NO			"No lower bound for '%s': use default %ld"
#define ERROR_BOUND_UP_NO			"No upper bound for '%s': use default %ld"
#define ERROR_BOUND_UP_IGNORE		"Upper bound ignored"
#define ERROR_BOUND_DN_UP_COMP		"Lower bound (%lf) is greater than upper bound (%lf)"
#define ERROR_BOUND_UP_DN_COMP		"Upper bound (%lf) is lesser than lower bound (%lf)"
#define ERROR_BOUND_CONSTRAINT		"In constraint '%s' is the lower bound greater than the upper bound"
#define ERROR_VAR_DUP_REPL			"Variable name '%s' is not unique: replaced by '%s'"
#define ERROR_VAR_NAME_LEN			"Variable name '%s' is too long"
#define ERROR_IMPORT_CALL			"Error at calling IMPORT function"
#define ERROR_IMPORT_KOLIOP_TYPE	"Invalid type of Koliop file at IMPORT function"
#define ERROR_IMPORT_KOLIOP_PAR		"Invalid parameter at IMPORT function importing from Koliop"
#define ERROR_INCLUDE_MAX			"Too much recursiv include files"								
#define ERROR_PATH_TOO_LONG			"Path name too long"
#define ERROR_FILE_WRITE			"Write error on file '%s'"
#define ERROR_FILE_READ				"Read error on file '%s'"
#define ERROR_FILE_INCL_DIR_NF		"Directory '%s' for include file not found"
#define ERROR_FILE_INPUT_INCL		"Include file '%s' not found"
#define ERROR_FILE_INPUT_OPEN		"Input file '%s' not found\n"
/* stegger 1.3.1.b csv import */
#define ERROR_FILE_CSV_DIM			"CSV file '%s' with wrong dimension\n"
#define ERROR_FILE_OUT_OPEN			"Output file '%s' for -%c not to open\n"
#define ERROR_COUNT					"\n%d errors found\n"
/* stegger 1.3.1 math function error */
#define ERROR_MATH_FUNC				"Error in mathematical function %s"
#define ERROR_CONST_IN_OBJ_FUNC			"Objective function is not well formed - Contains a constant parameter, which is ignored."
#define ERROR_NO_MODEL                          "No model was created"


#ifdef SOLVERSERVICE
  #define ERROR_OBJ_FUNC			"Objective function not found. The first defined objective function is used."
#endif

/* Fehlerausgabe */
#define ERROR_TYP_TXT_USER			"user error"
#define ERROR_TYP_TXT_SONST			"error"
#define ERROR_TYP_TXT_SONST_FATAL	"fatal error"
#define ERROR_TYP_TXT_SONST_WARN	"warning"
#define ERROR_TYP_TXT_COMP			"error (compiler)"
#define ERROR_TYP_TXT_INT			"error (interpreter)"
#define ERROR_TYP_TXT_IO			"error (input/output)"

#define OUTTXT_ERROR				"%s%s: %s%s"
#define OUTTXT_ERROR_ZEILE			"%s%s: file %s line %d: %s%s"
#define OUTTXT_ERROR_TOKEN			"%s%s: file %s line %d at '%s': %s%s"

/* Liste der Variablenproduktzerlegung */
#define OUTTXT_PRODLIST_PROD		"product of variables: %s := %s * %s\n"
#define OUTTXT_PRODLIST_INT			"integer to binary:    %s := %s\n"
#define OUTTXT_PRODLIST_REAL		"real to binary:       %s := %s\n"

/* Statistik-Ausgabe */
#define OUTTXT_STATISTIC			"File %s:\n" \
"%d Columns (variables), %d Rows (constraints)\n" \
"%d coefficients are non zero (of %d) (%.1lf %%)\n"

#endif


#endif // !GENTEXT_H

