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
 * main() fuer Start als separates Programm
 *
 *************************************************************************************/

#include "Main.hh"

#ifdef SOLVERSERVICE
#include "ecmpl.hh"
#endif

/* Hauptfunktion fuer Aufruf als eigenes Programm */
int main(int argc, char *argv[])
{

#ifdef SOLVERSERVICE
        QCoreApplication app(argc, argv);


#ifdef UNIX
        setlocale(LC_ALL, "");
        setlocale(LC_NUMERIC, "C");
#endif

#endif

	cmpl::Main glvar;
        return glvar.start_args(argc, argv);

}

