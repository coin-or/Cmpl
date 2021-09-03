/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010, 2011
 *  Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany and Thomas Schleiff - Halle(Saale),
 *  Germany
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


#include <sstream>

#include "CmplVal.hh"
#include "SetBase.hh"
#include "SetIterator.hh"
#include "Tuple.hh"
#include "../Control/ModuleBase.hh"


using namespace std;


namespace cmpl
{

	/**
	 * write cmpl value to stream
	 * @param ostr			output stream
	 * @param modp			calling module
	 * @param mode			mode for output: 0=direct; 1=part of other value
	 */
	void CmplVal::write(ostream& ostr, ModuleBase *modp, int mode) const
	{
		if (useValP()) {
			v.vp->write(ostr, modp, mode);
		}
		else if (!isSpecial()) {
			switch (t) {
				case TP_EMPTY:
					ostr << (mode == 0 ? "invalid" : " ");
					break;

				case TP_NULL:
					ostr << (mode == 0 ? "null" : "()");
					break;

				case TP_BLANK:
					ostr << (mode == 0 ? "<blank>" : "");
                    break;

				case TP_REAL:
					ostr << v.r;
					break;

				case TP_INFINITE:
					ostr << (v.i >= 0 ? "inf" : "-inf");
					break;

				case TP_INT:
					ostr << v.i;
					break;
					
				case TP_BIN:
					if (mode == 0)
						ostr << (v.i != 0 ? "true" : "false");
					else
						ostr << v.i;
					break;
					
				case TP_STR:
					if (mode != 0)
						ostr << '"';
					if (modp)
						ostr << modp->data()->globStrings()->at(v.i);
					else
						ostr << "<string:" << v.i << ">";
					if (mode != 0)
						ostr << '"';
					break;
				
				case TP_STRINGP:
					if (mode != 0)
						ostr << '"';
					ostr << *v.s;
					if (mode != 0)
						ostr << '"';
					break;
				
				case TP_INTERVAL_EMPTY:
					ostr << "<empty_interval>";
					break;
				
				case TP_INTERVAL_ALL:
					ostr << (mode == 0 ? "-inf..inf" : "..");
					break;
				
				case TP_INTERVAL_0_INF:
					ostr << (mode == 0 ? "0..inf" : "0..");
					break;
				
				case TP_INTERVAL_1_INF:
					ostr << (mode == 0 ? "1..inf" : "1..");
					break;
				
				case TP_INTERVAL_LB_R:
					ostr << v.r << (mode == 0 ? "..inf" : "..");
					break;
				
				case TP_INTERVAL_LB_I:
					ostr << v.i << (mode == 0 ? "..inf" : "..");
					break;
				
				case TP_INTERVAL_UB_R:
					ostr << (mode == 0 ? "-inf.." : "..") << v.r;
					break;
				
				case TP_INTERVAL_UB_I:
					ostr << (mode == 0 ? "-inf.." : "..") << v.i;
					break;
				
				case TP_INTERVAL_0_UB_R:
					ostr << "0.." << v.r;
					break;
				
				case TP_INTERVAL_0_UB_I:
					ostr << "0.." << v.i;
					break;
				
				case TP_INTERVAL_1_UB_R:
					ostr << "1.." << v.r;
					break;
				
				case TP_INTERVAL_1_UB_I:
					ostr << "1.." << v.i;
					break;
				
				case TP_TUPLE_EMPTY:
					ostr << "[]";
					break;
				
				case TP_ITUPLE_NULL:
					ostr << "[null]";
					break;
				
				case TP_ITUPLE_1INT:
					ostr << '[' << v.i << ']';
					break;
				
				case TP_ITUPLE_1STR:
					ostr << '[' << '"';
					if (modp)
						ostr << modp->data()->globStrings()->at(v.i);
					else
						ostr << "<string:" << v.i << ">";
					ostr << '"' << ']';
					break;
				
				case TP_SET_R1_0_UB:
					if (mode == 0)
						ostr << "[0.." << v.i << ']';
					else
						ostr << "0.." << v.i;
					break;
				
				case TP_SET_R1_0_UB_MNF:
					if (mode == 0)
                        ostr << "[/set(0.." << v.i << ")]";
					else
                        ostr << "/set(0.." << v.i << ')';
					break;
				
				case TP_SET_R1_1_UB:
					if (mode == 0)
                        ostr << "[1.." << v.i << ']';
					else
						ostr << "1.." << v.i;
					break;
				
				case TP_SET_R1_1_UB_MNF:
					if (mode == 0)
                        ostr << "[/set(1.." << v.i << ")]";
					else
                        ostr << "/set(1.." << v.i << ')';
					break;
				
				case TP_SET_EMPTY:
					ostr << "set()";
					break;
				
				case TP_SET_NULL:
					ostr << "[*null]";
					break;
				
				case TP_SET_1INT:
					if (mode == 0)
						ostr << '[';
					ostr << '*' << v.i;
					if (mode == 0)
						ostr << ']';
					break;
				
				case TP_SET_1STR:
					if (mode == 0)
						ostr << '[';
					if (modp)
						ostr << "*\"" << modp->data()->globStrings()->at(v.i) << "\"";
					else
						ostr << "*<string:" << v.i << ">";
					if (mode == 0)
						ostr << ']';
					break;

				case TP_SET_R1_LB_INF:
					if (mode == 0)
						ostr << '[' << v.i << "..]";
					else
						ostr << v.i << "..";
					break;
				
				case TP_SET_R1_LB_INF_MNF:
					if (mode == 0)
                        ostr << "[/set(" << v.i << "..)]";
					else
                        ostr << "/set(" << v.i << "..)";
					break;

				case TP_SET_R1_INF_UB:
					if (mode == 0)
						ostr << "[.." << v.i << ']';
					else
						ostr << ".." << v.i;
					break;

				case TP_SET_R1_INF_UB_MNF:
					if (mode == 0)
                        ostr << "[/set(.." << v.i << ")]";
					else
                        ostr << "/set(.." << v.i << ')';
					break;

                case TP_SET_R1_IINF:
                    if (mode == 0)
                        ostr << "[..]";
                    else
                        ostr << "..";
                    break;

                case TP_SET_R1_IINF_MNF:
                    if (mode == 0)
                        ostr << "[/set(..)]";
                    else
                        ostr << "/set(..)";
                    break;

                case TP_SET_R1A:
                    if (mode == 0)
                        ostr << "[*]";
                    else
                        ostr << "*";
                    break;

                case TP_SET_R1A_MNF:
                    if (mode == 0)
                        ostr << "[/]";
                    else
                        ostr << "/";
                    break;

                case TP_SET_R1A_INT:
                    if (mode == 0)
                        ostr << "[0..]";
                    else
                        ostr << "0..";
                    break;

                case TP_SET_R1A_INT_MNF:
                    if (mode == 0)
                        ostr << "[/set(0..)]";
                    else
                        ostr << "/set(0..)";
                    break;

                case TP_SET_R1A_STR:
                    ostr << "set[1](string)";
                    break;

                case TP_SET_R1A_STR_MNF:
                    if (mode == 0)
                        ostr << "[/set[1](string)]";
                    else
                        ostr << "/set[1](string)";
                    break;

                case TP_SET_ALL:
                    if (mode == 0)
                        ostr << "[]";
                    else
                        ostr << "";
                    break;

                case TP_SET_ALL_MNF:
                    if (mode == 0)
                        ostr << "[/[]]";
                    else
                        ostr << "/[]";
                    break;

                case TP_SET_ALL_INT:
                    ostr << "set(int)";
                    break;

                case TP_SET_ALL_INT_MNF:
                    if (mode == 0)
                        ostr << "[/set(int)]";
                    else
                        ostr << "/set(int)";
                    break;

                case TP_SET_ALL_STR:
                    ostr << "set(string)";
                    break;

                case TP_SET_ALL_STR_MNF:
                    if (mode == 0)
                        ostr << "[/set(string)]";
                    else
                        ostr << "/set(string)";
                    break;

//				case TP_FORMULA_EMPTY:
//					ostr << "<empty_formula>";
//					break;
				
				case TP_OBJECT_TYPE:
					if (v.i == VAL_OBJECT_TYPE_PAR)
						ostr << "par";
					else if (v.i == VAL_OBJECT_TYPE_VAR)
						ostr << "var";
					else if (v.i == VAL_OBJECT_TYPE_CON)
						ostr << "con";
					else if (v.i == VAL_OBJECT_TYPE_OBJ)
						ostr << "obj";
					break;

				default:
					ostr << "<unknown>";
			}
		}
		else {
			// values cannot be handled here; handle it instead in module Interpreter if neccessary
			switch (t) {
				case TP_ARRAY:
					ostr << "<array>";
					break;

				case TP_SYMBOL_VAL:
					ostr << "<symbol_value>";
					break;
				
				case TP_LIST_SIMPLE:
				case TP_LIST_SPARSE:
					ostr << "<simple_list>";
					break;
				
				case TP_LIST_TUPLE:
					ostr << "<tuple_list>";
					break;
				
				case TP_LIST_COMP:
				case TP_LIST_COMP_SPARSE:
					ostr << "<compound_list>";
					break;
				
				case TP_REF_LIST:
					ostr << "<list_reference>";
					break;
				
				default:
					ostr << "<unknown>";
				
			}
		}
	}


	/**
	 * write cmpl value to stream
	 * @param ostr			output stream
	 * @param v				cmpl value
	 */
	ostream& operator<< (ostream& ostr, const CmplVal& v)
	{
		v.write(ostr, NULL, 0);
		return ostr;
	}



	/**
	 * try to get this value as a real value
	 * @param r			return of real value
	 * @param tcl		conversion level
	 * @param modp		calling module (if NULL, then no conversion from stored string)
	 * @return			true if conversion is successful
	 */
    bool CmplVal::toReal(realType& r, TypeConversionLevel tcl, ModuleBase *modp) const
	{
		if (t == TP_REAL) {
			r = v.r;
			return true;
		}
		else if (tcl >= typeConversionIdent) {
            const CmplVal *src = (t == TP_TUPLE && tuple()->rank() == 1 ? tuple()->at(0) : this);
			if (src->t == TP_REAL) {
				r = src->v.r;
				return true;
			}
			else if (tcl >= typeConversionFromSub) {
				if (src->t == TP_INFINITE) {
					r = (src->v.i > 0 ? CMPLREAL_INFPOS : CMPLREAL_INFNEG);
					return true;
				}
				else if (tcl >= typeConversionExact) {
					if (src->t == TP_INT || src->t == TP_BIN || src->t == TP_ITUPLE_1INT) {
						r = (realType) src->v.i;
						return true;
					}
					else if (tcl >= typeConversionAll) {
						if (src->t == TP_STR || src->t == TP_STRINGP || src->t == TP_ITUPLE_1STR) {
							const char *c = src->stringAsCharP(modp);
							if (c) {
								char *ce;
								errno = 0;
								r = STR_TO_CMPLREAL2(c, &ce);
								if (ce > c)
									return true;
							}
						}
                        else if (src->isSet() && SetBase::finite(*src) && SetBase::cnt(*src) == 1) {
                            CmplValAuto a;
                            SetIterator::at(a, *src, 0);
                            return a.toReal(r, tcl, modp);
						}
					}
				}
			}
		}

		return false;
	}


	/**
	 * try to get this value as an int value
	 * @param r			return of int value
	 * @param tcl		conversion level
	 * @param modp		calling module (if NULL, then no conversion from stored string)
	 * @return			true if conversion is successful
	 */
    bool CmplVal::toInt(intType& r, TypeConversionLevel tcl, ModuleBase *modp) const
	{
		if (t == TP_INT) {
			r = v.i;
			return true;
		}
		else if (tcl >= typeConversionIdent) {
            const CmplVal *src = (t == TP_TUPLE && tuple()->rank() == 1 ? tuple()->at(0) : this);
			if (src->t == TP_INT || src->t == TP_ITUPLE_1INT) {
				r = src->v.i;
				return true;
			}
			else if (tcl >= typeConversionFromSub) {
				if (src->t == TP_BIN) {
					r = src->v.i;
					return true;
				}
				else if (tcl >= typeConversionValue) {
					if (src->t == TP_REAL && src->v.r >= (realType)CMPLINT_MIN && src->v.r <= (realType)CMPLINT_MAX) {
						r = (intType) src->v.r;
						return (tcl >= typeConversionLoseAcc || round(src->v.r) == src->v.r);
					}
					else if (tcl >= typeConversionAll) {
						if (src->t == TP_STR || src->t == TP_STRINGP || src->t == TP_ITUPLE_1STR) {
							const char *c = src->stringAsCharP(modp);
							if (c) {
								char *ce;
								errno = 0;
								r = STR_TO_CMPLINT2(c, &ce);
								if (ce > c && !errno)
									return true;
							}
						}
                        else if (src->isSet() && SetBase::finite(*src) && SetBase::cnt(*src) == 1) {
                            CmplValAuto a;
                            SetIterator::at(a, *src, 0);
                            return a.toInt(r, tcl, modp);
						}
					}
				}
			}
		}

		return false;
	}


	/**
	 * try to get this value as a boolean value
	 * @param r			return of boolean value
	 * @param tcl		conversion level
	 * @param modp		calling module (if NULL, then no conversion from stored string)
	 * @return			true if conversion is successful
	 */
    bool CmplVal::toBool(bool& r, TypeConversionLevel tcl, ModuleBase *modp) const
	{
		if (t == TP_BIN) {
			r = v.i;
			return true;
		}
		else if (tcl >= typeConversionIdent) {
            const CmplVal *src = (t == TP_TUPLE && tuple()->rank() == 1 ? tuple()->at(0) : this);
            if (src->t == TP_BIN || src->t == TP_CBHEAD_BIN) {
				r = src->v.i;
				return true;
			}
			else if (tcl >= typeConversionValue) {
				if (src->t == TP_INT || src->t == TP_ITUPLE_1INT) {
					r = (src->v.i != 0);
					return (tcl >= typeConversionLoseAcc || (src->v.i == 0 || src->v.i == 1));
				}
				else if (src->t == TP_REAL) {
					r = (src->v.r != 0);
					return (tcl >= typeConversionLoseAcc || (src->v.r == 0 || src->v.r == 1));
				}
				else if (tcl >= typeConversionAll) {
					if (src->t == TP_STR || src->t == TP_STRINGP || src->t == TP_ITUPLE_1STR) {
						const char *c = src->stringAsCharP(modp);
						r = (c && *c != '\0');
						return true;
					}
					else if (src->isSet()) {
                        r = ! SetBase::empty(*src);
						return true;
					}
				}
			}
		}

		return false;
	}


	/**
	 * try to get this value as a number
	 * @param r			return of number (gets type TP_INT, TP_REAL or TP_INFINITE) 
	 * @param tcl		conversion level
	 * @param modp		calling module (if NULL, then no conversion from stored string)
	 * @return			true if conversion is successful
	 */
    bool CmplVal::toNumber(CmplVal& r, TypeConversionLevel tcl, ModuleBase *modp) const
	{
		if (t == TP_INT || t == TP_REAL || t == TP_INFINITE) {
			r.t = t;
			if (t == TP_REAL)
				r.v.r = v.r;
			else
				r.v.i = v.i;
			return true;
		}
		else if (tcl >= typeConversionIdent) {
            const CmplVal *src = (t == TP_TUPLE && tuple()->rank() == 1 ? tuple()->at(0) : this);
			if (src->t == TP_INT || src->t == TP_ITUPLE_1INT || src->t == TP_REAL || src->t == TP_INFINITE) {
				r.t = (src->t == TP_ITUPLE_1INT ? TP_INT : src->t);
				if (t == TP_REAL)
					r.v.r = src->v.r;
				else
					r.v.i = src->v.i;
				return true;
			}
			else if (tcl >= typeConversionFromSub) {
				if (src->t == TP_BIN) {
					r.t = TP_INT;
					r.v.i = src->v.i;
					return true;
				}
				else if (tcl >= typeConversionAll) {
					if (src->t == TP_STR || src->t == TP_STRINGP || src->t == TP_ITUPLE_1STR) {
						const char *c = src->stringAsCharP(modp);
						if (c) {
							char *cer, *cei;

							errno = 0;
							r.v.i = STR_TO_CMPLINT2(c, &cei);
                            if (!errno && *cei == '\0') {
								r.t = TP_INT;
								return true;
							}
							else {
								int errnoi = errno;

								errno = 0;
								realType rr = STR_TO_CMPLREAL2(c, &cer);

								if (cer > c || (cei > c && !errnoi)) {
									if (cei >= cer && !errnoi) {
										r.t = TP_INT;
										return true;
									}
									else if (errno == ERANGE && rr != 0) {
										r.v.i = (rr > 0 ? 1 : -1);
										r.t = TP_INFINITE;
										return true;
									}
									else {
										r.v.r = rr;
										r.t = TP_REAL;
										return true;
									}
								}
							}
						}
					}
                    else if (src->isSet() && SetBase::finite(*src) && SetBase::cnt(*src) == 1) {
                        CmplValAuto a;
                        SetIterator::at(a, *src, 0);
                        return a.toNumber(r, tcl, modp);
					}
				}
			}
		}

		return false;
	}


    /**
     * add another numeric value to this numeric value (this must be isScalarNumber())
     * @param v2		value to add (must be isScalarNumber())
     * @param neg		substract value v2
     */
    void CmplVal::numAdd(const CmplVal& v2, bool neg)
    {
        // all values !isScalarNumber() are treated as numeric 0
        if (v2.isScalarNumber()) {
            if (!isScalarNumber() || (isNumNull() && t != TP_REAL)) {
                dispUnset();
                t = v2.t;
                if (v2.useInt())
                    v.i = (neg ? -v2.v.i : v2.v.i);
                else if (v2.useReal())
                    v.r = (neg ? -v2.v.r : v2.v.r);
            }
            else if (t == TP_REAL) {
                if (v2.t == TP_REAL)
                    v.r += (neg ? -v2.v.r : v2.v.r);
                else if (v2.t != TP_INFINITE)
                    v.r += (realType)(neg ? -v2.v.i : v2.v.i);
                else
                    set(TP_INFINITE, (neg ? -v2.v.i : v2.v.i));
            }
            else if (t != TP_INFINITE) {
                if (v2.t == TP_REAL) {
                    t = TP_REAL;
                    v.r = (realType)v.i + (neg ? -v2.v.r : v2.v.r);
                }
                else if (v2.t != TP_INFINITE) {
                    t = TP_INT;
                    v.i += (neg ? -v2.v.i : v2.v.i);
                }
                else
                    set(TP_INFINITE, (neg ? -v2.v.i : v2.v.i));
            }
            else if (v2.t == TP_INFINITE && v.i != v2.v.i) {
                t = TP_REAL;
                v.r = 0;
            }
        }
    }

    /**
     * multiply another numeric value to this numeric value (this must be isScalarNumber())
     * @param v2		value to multiply (must be isScalarNumber())
     */
    void CmplVal::numMult(const CmplVal& v2)
    {
        // all values !isScalarNumber() are treated as numeric 1
        if (v2.isScalarNumber() && !isNumNull()) {
            if (!isScalarNumber() || isNumOne()) {
                copyFrom(v2);
            }
            else if (v2.isNumNull()) {
                if (t == TP_REAL)
                    v.r = 0;
                else
                    set(TP_INT, 0);
            }
            else if (t == TP_REAL) {
                if (v2.t == TP_REAL)
                    v.r *= v2.v.r;
                else if (v2.t != TP_INFINITE)
                    v.r *= (realType)(v2.v.i);
                else
                    set(TP_INFINITE, ((v.r < 0) == (v2.v.i < 0) ? 1 : -1));
            }
            else if (t != TP_INFINITE) {
                if (v2.t == TP_REAL) {
                    t = TP_REAL;
                    v.r = (realType)v.i * v2.v.r;
                }
                else if (v2.t != TP_INFINITE) {
                    realType rr = (realType)(v.i) * (realType)(v2.v.i);
                    if (rr > (realType)CMPLINT_MIN && rr < (realType)CMPLINT_MAX) {
                        t = TP_INT;
                        v.i *= v2.v.i;
                    }
                    else {
                        set(TP_REAL, rr);
                    }
                }
                else {
                    set(TP_INFINITE, ((v.i < 0) == (v2.v.i < 0) ? 1 : -1));
                }
            }
            else if ((v2.t == TP_REAL && v.r < 0) || (v2.t != TP_REAL && v.i < 0)) {
                v.i = -v.i;
            }
        }
    }


    /**
     * compare numerical values
     * @param v1        first value (must be TP_INT, TP_BIN, TP_REAL or TP_INFINITE)
     * @param v2        second value (must be TP_INT, TP_BIN, TP_REAL or TP_INFINITE)
     * @return          -1 if first value lesser second value / 0 if both are equal / 1 if first value greater second value
     */
    int CmplVal::numCmp(const CmplVal &v1, const CmplVal &v2)
    {
        if (v1.t == TP_INFINITE || v2.t == TP_INFINITE) {
            if (v1.t == TP_INFINITE && v2.t == TP_INFINITE)
                return (v1.v.i == v2.v.i ? 0 : v1.v.i);
            else
                return (v1.t == TP_INFINITE ? v1.v.i : v2.v.i);
        }

        else if (v1.t == TP_REAL || v2.t == TP_REAL) {
            realType r1 = v1.numAsReal();
            realType r2 = v2.numAsReal();
            return (r1 == r2 ? 0 : (r1 < r2 ? -1 : 1));
        }

        else {
            intType i1 = v1.numAsInt();
            intType i2 = v2.numAsInt();
            return (i1 == i2 ? 0 : (i1 < i2 ? -1 : 1));
        }
    }


	/**
	 * try to get this value as a string value
	 * @param r			return of string value (gets type TP_STR or TP_STRINGP)
	 * @param tcl		conversion level
	 * @param modp		calling module
	 * @param disp		return true, if the caller has to dispose r after use
	 * @return			true if conversion is successful
	 */
    bool CmplVal::toString(CmplVal& r, TypeConversionLevel tcl, ModuleBase *modp, bool &disp) const
	{
        if (t == TP_STR || t == TP_STRINGP) {
            r.t = t;
            disp = false;
            if (t == TP_STR)
                r.v.i = v.i;
            else
                r.v.s = v.s;
            return true;
        }
        else if (tcl >= typeConversionIdent) {
            const CmplVal *src = (t == TP_TUPLE && tuple()->rank() == 1 ? tuple()->at(0) : this);
            if (tcl == typeConversionIdent && (src->t == TP_STR || src->t == TP_STRINGP || src->t == TP_ITUPLE_1STR)) {
                r.t = (src->t == TP_ITUPLE_1STR ? TP_STR : src->t);
                disp = false;
                if (t == TP_STR)
                    r.v.i = v.i;
                else
                    r.v.s = v.s;
                return true;
            }
            else if (tcl >= typeConversionAll ||
                     (tcl >= typeConversionExact && (src->t == TP_INT || src->t == TP_BIN)) ||
                     (tcl >= typeConversionValue && src->t == TP_REAL && (tcl >= typeConversionLoseAcc || round(src->v.r) == src->v.r))) {
                ostringstream o;
                write(o, modp);

                r.t = TP_STRINGP;
                r.v.s = new string(o.str());

                disp = true;
                return true;
            }
        }

        return false;
	}


	/**
	 * return string value as char pointer
	 * @param modp		calling module
	 * @return			char pointer to string / NULL: no string
	 */
    const char *CmplVal::stringAsCharP(ModuleBase *modp) const
	{
		if (useStringP())
			return v.s->c_str();
		else if ((t==TP_STR || t==TP_ITUPLE_1STR || t==TP_SET_1STR) && modp)
			return modp->data()->globStrings()->at(v.i);
		else
			return NULL;
	}


	/**
	 * if this is TP_STRINGP, then change it to TP_STR
     * @param modp		calling module
     * @param disp		free pointer to string
     */
    void CmplVal::stringPToStr(ModuleBase *modp, bool disp)
	{
		if (useStringP()) {
			unsigned i = modp->data()->globStrings()->storeInd(*(v.s));

            if (disp)
                dispose();

			set(TP_STR, (intType)i);
		}
	}


    /**
     * get numeric value as string suitable for output, this must be TP_REAL, TP_INT or TP_BIN
     * @param rf		format spezifier for real number output or NULL for standard format
     * @param ms		max length of output string
     * @return 			numeric value as string
     */
    string CmplVal::outStringNum(const char *rf, unsigned ms) const
    {
        char vstr[101];

        unsigned sz;
        if (useInt())
            sz = snprintf(vstr, 100, CMPLINT_STDFORMAT, v.i);
        else if (useReal())
            sz = snprintf(vstr, 100, (rf ?: CMPLREAL_STDFORMAT), v.r);
        else {
            *vstr = '\0';
            sz = 0;
        }

        if (sz > ms)
            throw OutputValueToLongException();

        return string(vstr);
    }


    /**
     * try to get this value as an index value
     * @param r			return of index value (gets type TP_INT, TP_STR or TP_ITUPLE)
     * @param tcl		conversion level
     * @param modp		calling module (if NULL, then no conversion from stored string)
     * @return			true if conversion is successful
     */
    bool CmplVal::toIndexVal(CmplVal& r, TypeConversionLevel tcl, ModuleBase *modp)
    {
        if (t == TP_INT || t == TP_STR || t == TP_ITUPLE || t == TP_ITUPLE_NULL) {
            r.copyFrom(this);
            return true;
        }

        if (t == TP_ITUPLE_1INT || t == TP_ITUPLE_1STR) {
            r.t = (tcl == typeConversionNoConv ? t : (t == TP_ITUPLE_1INT ? TP_INT : TP_STR));
            r.v.i = v.i;
            return true;
        }

        if (isTuple()) {
            //TODO
            //  kann hier gar nicht behandelt werden!
            //  Stattdessen vor Aufruf sicherstellen, dass Tuple als TP_ITUPLE gekennzeichnet ist, wenn es eines ist
            return false;
        }

        if (t == TP_STRINGP) {
            if (!modp)
                return false;

            stringPToStr(modp);
            r.t = t;
            r.v.i = v.i;
            return true;
        }

        intType i;
        if (!toInt(i, tcl, modp))
            return false;

        r.t = TP_INT;
        r.v.i = i;
        return true;
    }


    /**
     * try to get this value as a set
     * @param r			return of set (gets type TP_SET_*)
     * @param tcl		conversion level
     * @param modp		calling module (if NULL, then no conversion from stored string)
     * @return			true if conversion is successful
     */
    bool CmplVal::toSet(CmplVal& r, TypeConversionLevel tcl, ModuleBase *modp)
    {
        if (isSet()) {
            r.copyFrom(this);
            return true;
        }
        else if (t == TP_EMPTY || t == TP_NULL || t == TP_BLANK) {
            if (tcl < typeConversionExact)
                return false;

            r.set((t == TP_EMPTY ? TP_SET_EMPTY : (t == TP_NULL ? TP_SET_NULL : TP_SET_ALL)));
            return true;
        }
        else if ((isTuple() && !isITuple()) || isInterval()) {
            // cannot convert, because source type is not known here
            //TODO: exception?
            return false;
        }

        CmplVal iv;
        if (!toIndexVal(iv, tcl, modp))
            return false;

        if (iv.isScalarIndex()) {
            r.set((iv.t == TP_STR ? TP_SET_1STR : TP_SET_1INT), iv.v.i);
        }
        else if (iv.t == TP_ITUPLE_NULL) {
            r.set(TP_SET_NULL);
        }
        else {
            Tuple *tp = iv.tuple();
            if (tp->rank() == 0)
                r.set(TP_SET_NULL);
            else if (tp->rank() == 1)
                r.set((tp->at(0)->t == TP_STR ? TP_SET_1STR : TP_SET_1INT), tp->at(0)->v.i);
            else
                r.set(TP_SET_REC_MULT, new SetRecMult(tp, 0, true));
        }

        return true;
    }


    /**
     * get base data type for type
     * @param tp        given type
     * @return          base type
     */
    tp_e CmplVal::getBaseType(tp_e tp)
    {
        tp_e res;

        if (TP_IS_EMPTY(tp))
            res = TP_EMPTY;
        else if (TP_IS_SCALAR_REAL(tp))
            res = TP_REAL;
        else if (TP_IS_SCALAR_STRING(tp))
            res = TP_STR;
        else if (TP_IS_INTERVAL(tp))
            res = TP_INTERVAL;
        else if (TP_IS_TUPLE(tp))
            res = TP_TUPLE;
        else if (TP_IS_SET(tp))
            res = TP_SET_INF_SET;   // despite its name also used as base type of all sets
        else
            res = tp;

        return res;
    }

}

