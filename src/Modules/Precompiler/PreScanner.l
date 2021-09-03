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


%{
#include "Precompiler.hh"

/****** macro definition for error handling ****/
#define ERRHANDLER				precomp->modp()->ctrl()->errHandler()

/****** macro definitions for output to the protocol file ****/
#if PROTO
#define PROTO_OUT(a)			{ if (precomp->modp()) { ostream *__p = precomp->modp()->protoStream(); if (__p) { *__p << a; __p.flush(); } } }
#define PROTO_OUTL(a)			{ if (precomp->modp()) { ostream *__p = precomp->modp()->protoStream(); if (__p) { *__p << a << endl; } } }
#else
#define PROTO_OUT(a)
#define PROTO_OUTL(a)
#endif

%}

%option prefix="precmpl"
%option reentrant
/*%option ansi-prototypes*/
%option debug
%option noyywrap
%option nounput

	/* Scanner states */
		/* cmpl code */
%x SC_Cmpl
		/* C comment */
%x SC_Comment

		/* cmpl header */
%x SC_Header
		/* cmpl header: comment up to EOL */
%x SC_HLComment
		/* cmpl header: C comment */
%x SC_HCComment
		/* cmpl header: C comment with error */
%x SC_HCCommentErr

		/* position mark */
%x SC_Position


%%

	/* set initial scanner state */
	{
		if (precomp->isHeaderMode()) BEGIN(SC_Header);
		else BEGIN(SC_Cmpl);
	}


	/* cmpl header */
<SC_Cmpl>^[\ \t\015]*%											{ BEGIN(SC_Header); precomp->headerStart(); precomp->curPos()->addCols(yyleng); }

<SC_Header>(#|"//").*											{ BEGIN(SC_HLComment); }
<SC_Header>"/*"													{ BEGIN(SC_HCComment); precomp->curPos()->addCols(yyleng); }
<SC_HCComment>"*/"												{ BEGIN(SC_Header); precomp->curPos()->addCols(yyleng); }
<SC_HCCommentErr>"*/"											{ BEGIN(SC_Cmpl); precomp->curPos()->addCols(yyleng); }
<SC_HCComment,SC_HCCommentErr>.									{ precomp->curPos()->addCols(yyleng); }

<SC_Header>[^\ \t\015\"\n]+                                     { precomp->headerWord(yytext, false, precomp->curPos()); precomp->curPos()->addCols(yyleng); }
<SC_Header>\"([^\"\n]|\"\")*\"									{ precomp->headerWord(yytext, true, precomp->curPos()); precomp->curPos()->addCols(yyleng); }
<SC_Header>[\ \t\015]+                                          { precomp->headerSep(strlen(yytext)); precomp->curPos()->addCols(yyleng); }

<SC_Header,SC_HLComment>\n										{ BEGIN(SC_Cmpl);
																	bool doIncl = precomp->headerEnd(false);
																	precomp->curPos()->addLines();
																	if (doIncl) {
																		yyin = precomp->openIncludeFile(false);
																		yypush_buffer_state(yy_create_buffer( yyin, YY_BUF_SIZE, yyg ), yyg );
																	}
																	else {
																		precomp->resStream() << endl;
																	}
																}
<SC_HCComment>\n												{ BEGIN(SC_HCCommentErr); precomp->outErrorCCommentLf(); precomp->curPos()->addLines(); precomp->resStream() << endl; }
<SC_HCCommentErr>\n												{ precomp->curPos()->addLines(); precomp->resStream() << endl; }


	/* position marks */
<SC_Cmpl>^"##"/(([Dd]ef[Ll]oc)|([Ss]tart[Pp]os)|([Ll]ine)|([Cc]ol)):.*			{ BEGIN(SC_Position); precomp->curPos()->addCols(yyleng); }

<SC_Position>[Dd]ef[Ll]oc:.*									{ precomp->curPos()->addCols(7); precomp->parseDefLocPM(yytext+7); }
<SC_Position>[Ss]tart[Pp]os:.*									{ precomp->curPos()->addCols(9); precomp->parseStartPosPM(yytext+9); }
<SC_Position>[Ll]ine:.*											{ precomp->curPos()->addCols(5); precomp->parseLinePM(yytext+5); }
<SC_Position>[Cc]ol:.*											{ precomp->curPos()->addCols(4); precomp->parseColPM(yytext+4); }

<SC_Position>\n													{ BEGIN(SC_Cmpl); }
<SC_Position>.													{ precomp->outErrorPositionMark(yytext); }


	/* comments in cmpl */
<SC_Cmpl>(#|"//").*												{ /* empty */ }
<SC_Cmpl>"/*"													{ BEGIN(SC_Comment); precomp->storeCommentStartPos(); precomp->curPos()->addCols(yyleng); }
<SC_Cmpl>"*/"													{ precomp->outErrorCCommentEWS(); precomp->curPos()->addCols(yyleng); }
<SC_Comment>"*/"[\ \t\015]+\n									{ BEGIN(SC_Cmpl); precomp->curPos()->addLines(); precomp->outPositionAfterComment(true); }
<SC_Comment>"*/"												{ BEGIN(SC_Cmpl); precomp->curPos()->addCols(yyleng); precomp->outPositionAfterComment(false); }

<SC_Comment>[^*\n]+												|
<SC_Comment>.													{ precomp->curPos()->addCols(yyleng); }
<SC_Comment>\n													{ precomp->curPos()->addLines(); }


	/* other characters: copy to result */
<SC_Cmpl>[^%#*/\"\n]+											|
<SC_Cmpl>\"([^\"\n]|\\\")*\"                                    |
<SC_Cmpl>.														{ precomp->curPos()->addCols(yyleng); precomp->resStream() << yytext; }
<SC_Cmpl>\n														{ precomp->curPos()->addLines(); precomp->resStream() << endl; }


	/* end of buffer */
<*><<EOF>>														{ PROTO_OUTL("<<EOF>>");
																	BEGIN(SC_Cmpl);
																	bool doIncl = precomp->headerEnd(true);
																	precomp->resStream() << endl;
																	yypop_buffer_state( yyg );

																	if (doIncl) {
																		yyin = precomp->openIncludeFile(true);
																		if ( YY_CURRENT_BUFFER )
																			yypush_buffer_state( yy_create_buffer( yyin, YY_BUF_SIZE, yyg ), yyg );
																		else
																			yy_switch_to_buffer( yy_create_buffer( yyin, YY_BUF_SIZE, yyg ), yyg );
																	}
																	else {
																		precomp->closeIncludeFile();

																		if ( YY_CURRENT_BUFFER ) {
																			precomp->outStartPos(*(precomp->curPos()));
																		}
																		else {
																			const char *buf;
																			FILE *fp = precomp->iterInput(&buf);
																			if (precomp->isHeaderMode()) BEGIN(SC_Header);
																			if (fp)
																				yy_switch_to_buffer( yy_create_buffer (fp, YY_BUF_SIZE, yyg ), yyg );
																			else if (buf)
																				yy_scan_string( buf, yyg );
																			else
																				yyterminate();
																		}
																	}
																}



%%

namespace cmpl
{
	/**
	 * executes the scanner
	 */
	void Precompiler::doScan()
	{
		// get first input
		const char *buf;
		FILE *fp = iterInput(&buf);

		// if not input then return
		if (!fp && !buf)
			return;

		// initialize the scanner
		yylex_init(&_scanner);
		precmplset_debug(_traceScanning, _scanner);

		// set the first input
		if (fp)
			yy_switch_to_buffer (yy_create_buffer (fp, YY_BUF_SIZE, _scanner), _scanner);
		else
			yy_scan_string (buf, _scanner);

		// run scanner
		yylex(_scanner, this);

		// release scanner
		yylex_destroy(_scanner);
	}
}

