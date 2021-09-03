
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

/**
 * This file contains the pre-defined symbols for cmpl
 * @author  Thomas Schleiff/Mike Steglich
 */

// readonly symbols
#define M(n,s,p)		INS_PREDEF_SYM(n, false, true, s, p)
// read-write symbols
#define MW(n,s,p)		INS_PREDEF_SYM(n, false, false, s, p)

// readonly subsymbols
#define S(n,s,p)		INS_PREDEF_SYM(n, true, true, s, p)
// read-write subsymbols
#define SW(n,s,p)		INS_PREDEF_SYM(n, true, false, s, p)



	// object types
	M ( "par", 				specialSymbolObjTypePar,		PREDEFSYMVAL_OBJTYPE_PAR )
	M ( "var", 				specialSymbolObjType,			PREDEFSYMVAL_OBJTYPE_VAR )
	M ( "obj", 				specialSymbolObjType,			PREDEFSYMVAL_OBJTYPE_OBJ )
	M ( "con", 				specialSymbolObjType,			PREDEFSYMVAL_OBJTYPE_CON )

	// data types
	M ( "real", 			specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_REAL )
	M ( "int", 				specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_INT )
	M ( "bin", 				specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_BIN )
	M ( "numeric",			specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_NUMERIC )
	M ( "formula", 			specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_FORMULA )
	M ( "string",			specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_STRING )
	M ( "indexpart",		specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_INDEXPART )
	M ( "tuple", 			specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_TUPLE )
	M ( "set", 				specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_SET )
	M ( "interval",			specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_INTERVAL )
	M ( "function",			specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_FUNCTION )
	M ( "container", 		specialSymbolDataType,			PREDEFSYMVAL_DATATYPE_CONTAINER )
	M ( "type", 			specialSymbolDataTypeType,		PREDEFSYMVAL_DATATYPE_TYPE )
	M ( "objecttype",		specialSymbolDataTypeObj,		PREDEFSYMVAL_DATATYPE_OBJTYPE )

	// special values
	M ( "true",				specialSymbolKnownVal,			PREDEFSYMVAL_VALUE_TRUE )
	M ( "false",			specialSymbolKnownVal,			PREDEFSYMVAL_VALUE_FALSE )
	M ( "null",				specialSymbolKnownVal,			PREDEFSYMVAL_VALUE_NULL )
	M ( "inf",				specialSymbolKnownVal,			PREDEFSYMVAL_VALUE_INFINITE )
	M ( "invalid",			specialSymbolKnownVal,			PREDEFSYMVAL_VALUE_INVALID )

	// functions
	M ( "sum",				specialSymbolFuncEqKnownVal,	PREDEFSYMVAL_FUNC_SUM )

	M ( "min",				specialSymbolOptDirMin,			PREDEFSYMVAL_FUNC_MIN )
	M ( "max",				specialSymbolOptDirMax,			PREDEFSYMVAL_FUNC_MAX )

    M ( "and",				specialSymbolFuncEqKnownVal,	PREDEFSYMVAL_FUNC_AND )
    M ( "or",				specialSymbolFuncEqKnownVal,	PREDEFSYMVAL_FUNC_OR )

	M ( "mod",				specialSymbolFuncEqKnownVal,	PREDEFSYMVAL_FUNC_MOD )
	M ( "div",				specialSymbolFuncEqKnownVal,	PREDEFSYMVAL_FUNC_DIV )

	M ( "srand",			specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_SRAND )
	M ( "rand",				specialSymbolFuncEqKnownVal,	PREDEFSYMVAL_FUNC_RAND )

	M ( "count",			specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_COUNT )
	M ( "def",				specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_DEF )
	M ( "valid",			specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_VALID )
	M ( "dim",				specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_DIM )
	M ( "defset",			specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_DEFSET )
	M ( "validset",			specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_VALIDSET )
	M ( "len",				specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_LEN )
	M ( "rank",				specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_RANK )

    M ( "array",            specialSymbolFuncEqKnownVal,    PREDEFSYMVAL_FUNC_ARRAY )

	M ( "echo",				specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_ECHO )
	M ( "error",			specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_ERROR )
	M ( "format",			specialSymbolFuncKnownVal,		PREDEFSYMVAL_FUNC_FORMAT )

    // subsymbols
    SW( "as_var",           specialSymbolNo,                PREDEFSYMVAL_SUB_ASVAR )
    SW( "as_con",           specialSymbolNo,                PREDEFSYMVAL_SUB_ASCON )
    SW( "as_obj",           specialSymbolNo,                PREDEFSYMVAL_SUB_ASOBJ )
    SW( "as_string",        specialSymbolNo,                PREDEFSYMVAL_SUB_ASSTRING )


	// pseudo symbols
	M ( "$arg",				specialSymbolNo,				ICS_SPECIAL_ARG )
	M ( "$this",			specialSymbolNo,				ICS_SPECIAL_THIS )
	M ( "$func",			specialSymbolNameOnly,			9999 )
    M ( "$curTuple",		specialSymbolKnownVal,			ICS_SPECIAL_CURTUPLE )
    M ( "$curFullTuple",    specialSymbolKnownVal,			ICS_SPECIAL_CURFULLTUPLE )

	MW( "$curDestName",		specialSymbolKnownVal,			ICS_SPECIAL_CURDESTNAME )
    M ( "$curDestTuple",	specialSymbolKnownVal,			ICS_SPECIAL_CURDESTTUPLE )

	M ( "$varByName",		specialSymbolNo,				ICS_SPECIAL_VARBYNAME )
	M ( "$varByNo",			specialSymbolNo,				ICS_SPECIAL_VARBYNO )
	M ( "$conByName",		specialSymbolNo,				ICS_SPECIAL_CONBYNAME )
	M ( "$conByNo",			specialSymbolNo,				ICS_SPECIAL_CONBYNO )

	// pseudo subsymbols
	S ( "$objectType",		specialSymbolObjType,			ICS_SPECIAL_OBJECTTYPE )
    S ( "$type",		    specialSymbolDataType,			ICS_SPECIAL_DATATYPE )
	S ( "$typeBase",		specialSymbolDataType,			ICS_SPECIAL_TYPEBASE )
	S ( "$typePar",			specialSymbolKnownVal,			ICS_SPECIAL_TYPEPAR )
	S ( "$isReadOnly",		specialSymbolKnownVal,			ICS_SPECIAL_ISREADONLY )
	
	SW( "$destName",		specialSymbolKnownVal,			ICS_SPECIAL_DESTNAME )
    SW( "$destTuple",		specialSymbolKnownVal,			ICS_SPECIAL_DESTTUPLE )
    SW( "$destNameTuple",	specialSymbolKnownVal,			ICS_SPECIAL_DESTNAMETUPLE )
    S ( "$destFullName",	specialSymbolKnownVal,			ICS_SPECIAL_DESTFULLNAME )

	SW( "$fullFormula",		specialSymbolNo,				ICS_SPECIAL_FULLFORMULA )
	SW( "$formulaArray",	specialSymbolNo,				ICS_SPECIAL_FORMULAARRAY )

