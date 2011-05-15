/***********************************************************************
 *  This code is part of cmplFrame
 *
 *  Copyright (C) 2010 Mike Steglich - Technical University
 *  of Applied Sciences  Wildau, Germany
 *  and Thomas Schleiff - Halle(Saale), Germany
 *
 *  cmplFrame and CMPL are projects of the Technical University of
 *  Applied Sciences Wildau and the Institute for Operations Research
 *  and Business Management at the Martin Luther University
 *  Halle-Wittenberg.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  cmplFrame is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  cmplFrame is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef CFSTATUS_H
#define CFSTATUS_H

// cmplFrame status definitions

#define SOLVE_OK 1
#define CMPL_FAILED -1
#define SOLVER_FAILED -2

#define SEND_OK 1

#define KNOCK_FAILED -1
#define IDLE 1
#define BUSY 2
#define NORESPONSE 3
#define ERROR 4

#define RETRIEVE_OK 1
#define CANCEL_OK 1

#define OK 0

#define CMPLFRAME_CANCELED -99
#define PROG_DONT_RUN -100


#define ACTION_SOLVE 1
#define ACTION_SEND 2
#define ACTION_KNOCK 3
#define ACTION_RETRIEVE 4
#define ACTION_CANCEL 5

#define CMPL_MODEL 1
#define CMPL_SOLVE 2


#define NOT_DEFINED "#nn#"


#endif // CFSTATUS_H
