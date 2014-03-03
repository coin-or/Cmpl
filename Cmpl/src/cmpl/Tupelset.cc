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


#include "CmplCore.hh"
#include "IntFuncBase.hh"
#include "IntFuncOper.hh"
#include "Tupelset.hh"
#include "gencode.h"


namespace cmpl
{
	//********************************************************
	// TupelUtil
	//********************************************************
	
	/**
	 * constructs a tupel as a val_str value from an array of indizes.
	 * @param glvar		CmplCore object of CMPL
	 * @param rank      rank for the tupel
	 * @param arr       array with indizes or tupels, must match rank
	 * @param cnt       length of arr
	 * @param glmem		global allocation of the memory
	 * @param tup       pointer to store the constructed tupel in
	 */
	void TupelUtil::constructVal(CmplCore *glvar, int rank, val_str *arr, long cnt, bool glmem, val_str *tup)
	{
		tup->t = TPK_TUPEL_N(rank);
		if (rank == 1)
			storeInd(glvar, &(tup->v.i), arr, cnt);
		else if (rank > 1)
			tup->v.tp = construct(glvar, rank, arr, cnt, glmem);
	}

	/**
	 * constructs a tupel as tupel_t from an array of indizes.
	 * @param glvar		CmplCore object of CMPL
	 * @param rank      rank for the tupel
	 * @param arr       array with indizes or tupels, must match rank
	 * @param cnt       length of arr
	 * @param glmem		global allocation of the memory
	 * @return          constructed tupel
	 */
	tupel_t TupelUtil::construct(CmplCore *glvar, int rank, val_str *arr, long cnt, bool glmem)
	{
		// if rank == 0 use global null tupel
		if (rank == 0 && glvar->gl_nulltupel)
			return glvar->gl_nulltupel;

		// memory allocation
        tupel_t res;
		if (glmem)
			res = (tupel_t) glvar->c_getmem((rank+1) * sizeof(long));
		else
			res = (tupel_t) calloc(rank+1, sizeof(long));

		res[0] = (glmem ? 0x100 : 0) + rank;

		// store tupel values
		storeInd(glvar, res+1, arr, cnt);

        return res;
	}

	/**
	 * store index values for the tupel.
	 * @param glvar		CmplCore object of CMPL
	 * @param dest      pointer to the start for storing the index values
	 * @param arr       array with indizes or tupels
	 * @param cnt       length of arr
	 */
	void TupelUtil::storeInd(CmplCore *glvar, long *dest, val_str *arr, long cnt)
	{
		int i, j, r;
		val_str *vp;
		long *dp;
		tupel_t tp;

                for (i=0, vp=arr, dp=dest; i < cnt; i++, vp++)
		{
			if (TP_IS_TUPEL(vp->t))
			{
				r = RANK_TP_TUPEL(vp->t);
				if (r == 1)
				{
					*(dp++) = vp->v.i;
				}
				else if (r > 1)
				{
					// copy tupel indizes to the new tupel
					tp = vp->v.tp;
					for (j=1; j<=r; j++)
						*(dp++) = tp[j];

					if (!globAlloc(tp))
					{
						free(tp);
						vp->t = TPP_EMPTY;
					}
				}
			}
			else if (TP_IS_TUPELSET(vp->t))
			{
				r = vp->v.tps->rank();
				if (r >= 1)
				{
					if (vp->v.tps->isTupel())
					{
						for (j=0; j<r; j++)
							*(dp++) = valToTupelval(vp->v.tps->ind(j));
					}
					else
					{
						FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "tupelset cannot be part of a tupel");	//TODO
					}
				}

				if (!vp->v.tps->globAlloc())
				{
					Tupelset::freeTupelset(glvar, vp->v.tps);
					vp->t = TPP_EMPTY;
				}
			}
			else
			{
				*(dp++) = valToTupelvalChecked(glvar, vp);
			}
		}
	}

	/**
	 * constructs a tupel as a val_str value as copy of another tupel
	 * @param glvar     CmplCore object of CMPL
	 * @param src       tupel to copy
	 * @param r1val     if tupel has rank 1, then return as scalar value
	 * @param dst       pointer to store the constructed tupel in
	 * @param glmem     global allocation of the memory
	 */
	void TupelUtil::copyVal(CmplCore *glvar, val_str *src, bool r1val, val_str *dst, bool glmem)
	{
		// for rank 0 or 1 no copy is needed, because they have no memory allocation
		int r = RANK_TP_TUPEL(src->t);
		if (r <= 1)
		{
			if (r==1 && r1val)
				tupelvalToVal(src->v.i, dst);
			else
				*dst = *src;

			return;
		}

		// if the tupel is already global allocated, no copy is needed
		if (TupelUtil::globAlloc(src->v.tp))
		{
			*dst = *src;
			return;
		}

		// if the copy should be global allocated, then don't copy but change allocation to global
		if (glmem)
		{
			TupelUtil::setToGlobAlloc(glvar, src->v.tp);
			*dst = *src;
			return;
		}

		// copy the tupel
		tupel_t res = (tupel_t) calloc(r+1, sizeof(long));
		memcpy(res, src->v.tp, ((r+1)*sizeof(long)));

		dst->t = TPK_TUPEL_N(r);
		dst->v.tp = res;
	}

	/**
	 * gets the tupel that is represented by a val_str value.
	 * @param glvar		CmplCore object of CMPL
	 * @param vp	    value
	 * @param glmem     global allocation of the memory if neccessary
	 * @return          tupel
	 */
	tupel_t TupelUtil::tupelFromVal(CmplCore *glvar, val_str *vp, bool glmem)
	{
		if (TP_IS_TUPEL(vp->t))
		{
			// Für Rang 0 und 1 ist die Speicherung als val_str abweichend
			int rank = RANK_TP_TUPEL(vp->t);
			if (rank == 0)
			{
				return glvar->gl_nulltupel;
			}
			else if (rank == 1)
			{
				tupel_t res;
				if (glmem)
					res = (tupel_t) glvar->c_getmem(2 * sizeof(long));
				else
					res = (tupel_t) calloc(2, sizeof(long));

				res[0] = (glmem ? 0x100 : 0) + 1;
				res[1] = vp->v.i;
				return res;
			}
			else
			{
				return vp->v.tp;
			}
		}
		else
		{
			// Einfacher Wert, daraus Tupel Rang 1 erstellen
			return construct(glvar, 1, vp, 1, glmem);
		}
	}

	/**
	 * sets a val_str value to represent the tupel.
	 * @param tup       tupel
	 * @param vp        value to set
	 */
	void TupelUtil::tupelToVal(tupel_t tup, val_str *vp)
	{
		vp->t = TPK_TUPEL_N(rank(tup));

		if (rank(tup) == 1)
			vp->v.i = tup[1];
		else if (rank(tup) > 1)
			vp->v.tp = tup;
	}

	/**
	 * converts a val_str value for an index (must be convertible to TP_INT or TP_ISTR) to a simple int like used for storing tupels.
	 * (in the simple int the higher negative values are used for string indizes).
	 * @param vp        value as val_str
	 * @return          value for using in <code>tupel</code>
	 */
	long TupelUtil::valToTupelvalChecked(CmplCore *glvar, val_str *vp)
	{
		long res;

		// Wert muss als TP_INT oder TP_ISTR darstellbar sein
		if (vp->t != TP_INT && vp->t != TP_DBL && vp->t != TP_BIN && vp->t != TP_ISTR && vp->t != TP_STR)
		{
			SPRINT_ERR_2(ERROR_NOT_AN_INDEX, "(val)");  //write_val_to_temp_buf(vp));	//TODO
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			res = 1;
		}
		else
		{
			val_str v;
			if (vp->t == TP_STR)
			{
				if (!glvar->string_to_istr(&v, vp->v.s, false))
					free(vp->v.s);
			}
			else
			{
				v.v.i = (vp->t == TP_DBL ? RUND(vp->v.f) : vp->v.i);
				v.t = (vp->t == TP_DBL || vp->t == TP_BIN ? TP_INT : vp->t);
			}

			res = valToTupelval(&v);
		}

		return res;
	}

	/**
	 * compares two tupels
	 * @param t1        tupel
	 * @param t2        tupel
	 * @param r1val		a scalar value can substitute a rank 1 tupel
	 * @return          true if the two tupels are equal
	 */
	bool TupelUtil::cmpEqual(val_str *t1, val_str *t2, bool r1val)
	{
		// both values must be tupels
		if (!TP_IS_TUPEL(t1->t) || !TP_IS_TUPEL(t2->t)) {
			if (r1val) {
				if ((TP_IS_TUPEL(t1->t) && RANK_TP_TUPEL(t1->t) != 1) && (TP_IS_TUPEL(t2->t) && RANK_TP_TUPEL(t2->t)))
					return false;
				else
					return ((TP_IS_TUPEL(t1->t) ? t1->v.i : valToTupelval(t1)) == (TP_IS_TUPEL(t2->t) ? t2->v.i : valToTupelval(t2)));
			}

			return false;
		}

		// both tupels must have the same rank
		int r1 = RANK_TP_TUPEL(t1->t);
		int r2 = RANK_TP_TUPEL(t2->t);
		if (r1 != r2)
			return false;

		if (r1 <= 1) {
			return (r1 == 0 || t1->v.i == t2->v.i);
		}
		else {
			tupel_t tp1 = t1->v.tp;
			tupel_t tp2 = t2->v.tp;
			for (int i = 1; i <= r1; i++) {
				if (tp1[i] != tp2[i])
					return false;
			}
			return true;
		}
	}

	/**
	 * compares a tupel with the tupels in an array
	 * @param t1        tupel
	 * @param arr       array of tupels
	 * @param cnt       length of array
	 * @param r1val		a scalar value can substitute a rank 1 tupel
	 * @return          index of equal tupel in arr, or -1 if no tupel is equal to <code>t1</code>
	 */
	int TupelUtil::cmpEqualArr(val_str *t1, val_str *arr, int cnt, bool r1val)
	{
		int i;
		for (i = cnt-1; i >= 0; i--) {
			if (TupelUtil::cmpEqual(t1, arr+i, r1val))
				return i;
		}
		return -1;
	}

	/**
	 * creates a tupel with a reduced rank.
	 * @param glvar     CmplCore object of CMPL
	 * @param mode      reduce mode (one of RANK_REDUCE_*, but not RANK_REDUCE_TPS)
	 * @param pat       reduce pattern array, length must match rank of tupel <code>src</code>, only used for RANK_REDUCE_PAT
	 * @param dstrank	rank of destination tupel or -1 if not known, only used for RANK_REDUCE_PAT
	 * @param src       source tupel
	 * @param r1val     if <code>dstrank</code> is 1, then return as scalar value
	 * @param dst       result tupel
	 * @param glmem     global allocation of the memory
	 */
	void TupelUtil::reduceRank(CmplCore *glvar, int mode, bool *pat, int dstrank, val_str *src, bool r1val, val_str *dst, bool glmem)
	{
		// calculate dstrank if necessary
		int i, r;
		if (mode == RANK_REDUCE_PAT) {
			if (!TP_IS_TUPEL(src->t)) {
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "value must be a tupel");
				mode = RANK_REDUCE_ALL;
			}
			else {
				r = RANK_TP_TUPEL(src->t);

				if (dstrank < 0) {
					dstrank = 0;
					for (i = 0; i < r; i++) {
                        if (!pat[i])
							dstrank++;
					}
				}
				if (dstrank >= r)
					mode = RANK_REDUCE_NO;
				else if (dstrank == 0)
					mode = RANK_REDUCE_ALL;
			}
		}
		
		// make the dest tupel
		if (mode == RANK_REDUCE_PAT) {
			dst->t = TPK_TUPEL_N(dstrank);

			if (dstrank == 1) {
				for (i = 0; i < r; i++) {
                    if (!pat[i]) {
						if (r1val)
							tupelvalToVal(src->v.tp[i+1], dst);
						else
							dst->v.i = src->v.tp[i+1];

						break;
					}
				}
			}
			else {
				tupel_t res;
				if (glmem)
					res = (tupel_t) glvar->c_getmem((dstrank+1) * sizeof(long));
				else
					res = (tupel_t) calloc(dstrank+1, sizeof(long));

				res[0] = (glmem ? 0x100 : 0) + dstrank;

				int d = 1;
				for (i = 0; i < r; i++) {
                    if (!pat[i])
						res[d++] = src->v.tp[i+1];
				}

				dst->v.tp = res;
			}
		}
		else if (mode == RANK_REDUCE_NO) {
			if (RANK_TP_TUPEL(src->t) == 1 && r1val)
				tupelvalToVal(src->v.i, dst);
			else
				*dst = *src;
		}
		else {
			if (mode != RANK_REDUCE_ALL)
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "wrong mode for TupelUtil::reduceRank");
			dst->t = TPK_TUPEL_N(0);
		}
	}

	/**
	 * changes the tupel to global memory allocation.
	 * @param glvar     CmplCore object of CMPL
	 * @param tup       tupel
	 */
	void TupelUtil::setToGlobAlloc(CmplCore *glvar, tupel_t tup)
	{
		if (!TupelUtil::globAlloc(tup))
		{
			glvar->ins_mem(tup);
			*tup |= 0x100;
		}
	}

	/**
	 * writes a textual form of a tupel or tupelset in a string.
	 * @param glvar     CmplCore object of CMPL
	 * @param dest      result buffer
	 * @param vp        tupel or tupelset
	 * @param maxlen    size of the result buffer
	 * @return result buffer
	 */
	char *TupelUtil::asString(CmplCore *glvar, char *dest, val_str *vp, int maxlen)
	{
		if (maxlen <= 0)
			return dest;

		int rank = RANK_TP_TUPEL(vp->t);
		if (rank == 0)
		{
			snprintf(dest, maxlen, "[null]");
		}
		else
		{
			long *ip, i;
			int p;
			long l;
			enum etp tp;

			if (rank == 1)
				ip = &(vp->v.i);
			else
				ip = ((long *)(vp->v.tp)) + 1;

			p = snprintf(dest, maxlen, "[");

			for (i = 0; i < rank && p < maxlen; i++)
			{
				l = tupelvalToIntTp(ip[i], &tp);
				if (tp == TP_INT)
					p += snprintf(dest + p, maxlen - p, "%s%ld", (i==0 ? "" : ", "), l);
				else
					p += snprintf(dest + p, maxlen - p, "%s\"%s\"", (i==0 ? "" : ", "), glvar->strings.strings_ind[l]);
			}

			if (p < maxlen)
				snprintf(dest + p, maxlen - p, "]");
		}

		dest[maxlen-1] = '\0';
		return dest;
	}
	
	
	//********************************************************
	// Tupelset
	//********************************************************
	
	/**
	 * constructs a tupelset as a val_str value from an array of indizes.
	 * @param glvar		CmplCore object of CMPL
	 * @param indLen	count for the array of indizes in the tupelset
	 * @param arr       array with indizes or tupels, must match indLen
	 * @param cnt       length of arr
	 * @param glmem		global allocation of the memory
	 * @param tup       pointer to store the constructed tupelset in
	 */
	void Tupelset::constructValTupel(CmplCore *glvar, int indLen, val_str *arr, long cnt, bool glmem, val_str *tup)
	{
		PROTO_OUT_3("\n        Tupelset::constructValTupel %d %d ", indLen, (int)cnt);

		tup->v.tps = constructTupel(glvar, indLen, arr, cnt, glmem);
		if (tup->v.tps->hasRank())
			tup->t = TPK_TUPELSET_N(tup->v.tps->rank());
		else
			tup->t = TPK_TUPELSET_N(1);		// Ersatzweise hier 1 verwenden (Rang wird bei der Ausführung des Zwischencodes sowieso immer aus dem Tupelset selbst geholt und nicht aus dem Typ)
	}

	/**
	 * constructs a tupelset object from an array of indizes.
	 * @param glvar		CmplCore object of CMPL
	 * @param indLen	count for the array of indizes in the tupelset
	 * @param arr       array with indizes or tupels, must match indLen
	 * @param cnt       length of arr
	 * @param glmem		global allocation of the memory
	 * @return			tupelset object
	 */
	Tupelset* Tupelset::constructTupel(CmplCore *glvar, int indLen, val_str *arr, long cnt, bool glmem)
	{
		// check for null tupelset
		if (indLen == 0 && glvar->gl_nulltupelset)
			return glvar->gl_nulltupelset;

		// check for empty tupelset
		if (indLen == 1 && cnt == 1 && arr->t == TP_SET_EMPTY)
		{
			if (glvar->gl_emptytupelset)
				return glvar->gl_emptytupelset;

			Tupelset *tps = allocTupelset(glvar, 0, glmem);
			tps->_rank = 1;
			return tps;
		}

		// construct new tupelset
		Tupelset *tps = allocTupelset(glvar, indLen, glmem);
		tps->initTupel(glvar, arr, cnt);

		if (glmem)
			tps->setToGlobAlloc(glvar);

		return tps;
	}

	/**
	 * initialize a tupelset from an array of indizes.
	 * @param glvar		CmplCore object of CMPL
	 * @param arr       array with indizes or tupels, must match indLen
	 * @param cnt       length of arr
	 */
	void Tupelset::initTupel(CmplCore *glvar, val_str *arr, long cnt)
	{
		int i, j, r, d;
		val_str *vp;
		val_str *dp, *dp2;
		tupel_t tp;
		Tupelset *tps;
		SetUtil setUtil(glvar);

		bool hasRank = true;
		bool isEmpty = false;
		bool isInfinite = false;

		_len = 1;

        for (i=0, vp=arr, dp=_ind, d=0; i < cnt; i++, vp++)
		{
			if (TP_IS_TUPEL(vp->t))
			{
				r = RANK_TP_TUPEL(vp->t);
				if (r == 1)
				{
					TupelUtil::tupelvalToVal(vp->v.i, (dp++));
				}
				else if (r > 1)
				{
					// copy tupel indizes to tupelset
					tp = vp->v.tp;
					for (j=1; j<=r; j++)
						TupelUtil::tupelvalToVal(tp[j], (dp++));

					if (!TupelUtil::globAlloc(tp))
					{
						free(tp);
						vp->t = TPP_EMPTY;
					}
				}
				_fix += r;
				_rank += r;
			}
			else if (TP_IS_TUPELSET(vp->t))
			{
				tps = vp->v.tps;

				if (tps->isEmpty())
				{
					isEmpty = true;
				}
				else if (tps->rank() != 0)
				{
					// if a tupelset index has _freeIndRev, then set _freeIndRev also in the resulting tupelset
					if (tps->_freeIndRev && !_freeIndRev && !tps->_hasEnum && !_hasEnum && !isEmpty)
					{
						_freeIndRev = (bool *) calloc(_cnt, sizeof(bool));
						for (d=0; d<_cnt; d++)
							_freeIndRev[d] = false;
					}

					// copy indices from tupelset
					for (j=0, d=dp-_ind; j < tps->_cnt; j++)
					{
						*(dp++) = tps->_ind[j];
						if (_freeIndRev && tps->_freeIndRev)
							_freeIndRev[d++] = tps->_freeIndRev[j];
					}

					if (!tps->hasRank())
						hasRank = false;

					_fix += tps->_fix;
					_rank += tps->_rank;
                    if (tps->_hasEnum)
                        _hasEnum = true;

					if (tps->isInfinite())
						isInfinite = true;
					else
						_len *= tps->len();

				}

				if (!tps->_globAlloc)
				{
					freeTupelset(glvar, tps);
					vp->t = TPP_EMPTY;
				}
			}
			else if (TYPECLASS(vp->t) == TPK_SET)
			{
				*(dp++) = *vp;
				_rank++;

				if (SET_IS_EMPTY(vp->t))
					isEmpty = true;
				else if (SET_IS_INFINITE(vp->t))
					isInfinite = true;
				else
					_len *= setUtil.set_len(vp);
			}
			else
			{
				// vp noetigenfalls in ein fuer Tupel geeigneten Typ konvertieren
				if (vp->t == TP_INT || vp->t == TP_ISTR)
					*(dp++) = *vp;
				else
					TupelUtil::tupelvalToVal(TupelUtil::valToTupelvalChecked(glvar, vp), dp++);

				_fix++;
				_rank++;
			}
		}

		if (isEmpty)
		{
			_len = 0;
			_fix = 0;
			_cnt = 0;
			_rank = 1;
			_hasEnum = false;
		}
		else
		{
			if (isInfinite)
				_len = -1;
			if (!hasRank)
				_rank = -1;
		}

		if (_freeIndRev)
		{
			if (isEmpty || _hasEnum)
			{
				free(_freeIndRev);
				_freeIndRev = NULL;
			}
			else if (_globAlloc)
			{
				glvar->ins_mem(_freeIndRev);
			}
		}

		// TODO: wenn mindestens 2 TupelsetEnum mit jeweils verschiedenrangigen Tupeln enthalten, dann kann _len falsch sein; z.B. { [1], [1,1] } { [1], [1,1] } -> { [1,1], [1,1,1], [1,1,1,1] }
		// 	(aber Fall doch nicht extra berücksichtigen, Tupel existieren dann wirklich doppelt, aber nur erstes zugreifbar da zweites nicht adressierbar)
	}

	/**
	 * constructs a tupelset as a val_str value from an enumeration of tupels.
	 * @param glvar     CmplCore object of CMPL
	 * @param arr       array with tupels
	 * @param cnt       length of arr
	 * @param indRank   if greater 0 then <code>arr</code> contains no tupels but indices which are to combine to tupels of this rank
  	 * @param glmem     global allocation of the memory
	 * @param tup       pointer to store the constructed tupelset in
	 */
	void Tupelset::constructValEnum(CmplCore *glvar, val_str *arr, long cnt, int indRank, bool glmem, val_str *tup)
	{
		PROTO_OUT_3("\n        Tupelset::constructValEnum %d %d ", indRank, (int)cnt);

		Tupelset *tps = constructEnum(glvar, arr, cnt, indRank, glmem);
		tup->v.p = tps;
		tup->t = TPK_TUPELSET_N(tps->_rank >= 0 ? tps->_rank : 1);
    }

	/**
	 * constructs a tupelset from an enumeration of tupels.
	 * @param glvar     CmplCore object of CMPL
	 * @param arr       array with tupels
	 * @param cnt       length of arr
	 * @param indRank   if greater 0 then <code>arr</code> contains no tupels but indices which are to combine to tupels of this rank
  	 * @param glmem     global allocation of the memory
	 * @return			tupelset object
	 */
	Tupelset* Tupelset::constructEnum(CmplCore *glvar, val_str *arr, long cnt, int indRank, bool glmem)
	{
		// use global empty tupelset if there is no element
		if (cnt == 0 && glvar->gl_emptytupelset)
			return glvar->gl_emptytupelset;

		// use global null tupelset if the only tupel is the null tupel
		if (cnt == 1 && indRank <= 0 && RANK_TP_TUPEL(arr->t) == 0 && glvar->gl_nulltupelset)
			return glvar->gl_nulltupelset;

		// if only one tupel element then construct as tupel
		if (cnt == 1 && indRank <= 0 && TP_IS_TUPEL(arr->t))
			return constructTupel(glvar, RANK_TP_TUPEL(arr->t), arr, 1, glmem);

		// construct as enumeration
		TupelsetEnum *tpe = TupelsetEnum::construct(glvar, arr, cnt, indRank, glmem);
		return constructForEnum(glvar, tpe, glmem);
	}

	/**
	 * constructs a tupelset for a TupelsetEnum
	 * @param glvar     CmplCore object of CMPL
	 * @param tpe		TupelsetEnum object
  	 * @param glmem     global allocation of the memory
	 * @return			tupelset object
	 */
	Tupelset* Tupelset::constructForEnum(CmplCore *glvar, TupelsetEnum *tpe, bool glmem)
	{
		Tupelset *tps = Tupelset::allocTupelset(glvar, 1, glmem);

		tps->_ind[0].v.p = tpe;
		tps->_ind[0].t = (tpe->_rank >= 0 ? TPK_TUPELSET_N(tpe->_rank) : TPK_TUPELSET_N(1));
		tps->_len = tpe->_len;
		tps->_rank = tpe->_rank;
		tps->_hasEnum = 1;

		return tps;
	}

	/**
	 * constructs a tupelset as a val_str value for one index value with setting for <code>_freeIndRev</code>
	 * @param glvar     CmplCore object of CMPL
	 * @param vp        value from which the tupelset is constructed
	 * @param free      true: treat scalar value as free / false: treat set value as non-free
	 * @param glmem     global allocation of the memory
	 * @param tup		pointer to store the constructed tupelset in
	 */
	void Tupelset::constructForFreeIndRev(CmplCore *glvar, val_str *vp, bool free, bool glmem, val_str *tup)
	{
		Tupelset *tps;
		int indLen, i;

		// if vp is already the correct tupelset then nothing is to do
		if (TP_IS_TUPELSET(vp->t))
		{
			tps = vp->v.tps;
			if (tps->_freeIndRev)
			{
				for (i=0; i<tps->_cnt; i++)
				{
					if (TYPECLASS(tps->_ind[i].t) == TPK_VALUE)
					{
						if (tps->_freeIndRev[i] != free)
							break;
					}
					else
					{
						if (tps->_freeIndRev[i] == free)
							break;
					}
				}
				if (i == tps->_cnt)
				{
					if (glmem && !tps->_globAlloc)
						tps->setToGlobAlloc(glvar);

					*tup = *vp;
					return;
				}
			}
		}

		// construct tupelset
		if (TP_IS_TUPELSET(vp->t))
			indLen = vp->v.tps->cnt();
		else if (TP_IS_TUPEL(vp->t))
			indLen = RANK_TP_TUPEL(vp->t);
		else
			indLen = 1;

		constructValTupel(glvar, indLen, vp, 1, glmem, tup);
		tps = tup->v.tps;

		if (tps->hasEnum())
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "operation is not allowed for an enumeration tupelset");
			return;
		}

		if (tps->isEmpty())
			return;

		// alloc _freeIndRev
		if (!tps->_freeIndRev)
		{
			if (glmem)
				tps->_freeIndRev = (bool *) glvar->c_getmem(indLen * sizeof(bool));
			else
				tps->_freeIndRev = (bool *) calloc(indLen, sizeof(bool));
		}

		// set _freeIndRev
		for (i=0; i<indLen; i++)
		{
			if (TYPECLASS(tps->_ind[i].t) == TPK_VALUE)
				tps->_freeIndRev[i] = free;
			else
				tps->_freeIndRev[i] = !free;
		}
	}

	/**
	 * constructs a tupelset from an intersection of two tupelsets
	 * @param glvar     CmplCore object of CMPL
	 * @param funcNumOp	object for callback of fset_op()
	 * @param vp1       first operand of the intersection
	 * @param vp2       second operand of the intersection
     * @param mode      reduce mode (one of RANK_REDUCE_*)
	 * @param pat       reduce pattern array, length must match rank of tupel <code>src</code>, only used for RANK_REDUCE_PAT
	 * @param r1val		if dest tupelset has rank 1, then return as set or scalar value
	 * @param glmem     global allocation of the memory
	 * @param tup       pointer to store the constructed tupelset in
	 */
	void Tupelset::constructFromIntersection(CmplCore *glvar, IntFuncNumOp *funcNumOp, val_str *vp1, val_str *vp2, int mode, bool *pat, bool r1val, bool glmem, val_str *tup)
	{
		PROTO_OUT_2("\n    constructFromIntersection (rrMode %d) ", mode);

		// both operands must be tupelsets or tupels
		enum etp t1 = vp1->t;
		enum etp t2 = vp2->t;
		if ((!TP_IS_TUPEL(t1) && !TP_IS_TUPELSET(t1)) || (!TP_IS_TUPEL(t2) && !TP_IS_TUPELSET(t2)))
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "operand for tupelset intersection is neither a tupel or a tupelset");
			tup->t = TPK_TUPELSET_N(1);
			tup->v.tps = glvar->gl_emptytupelset;
			return;
		}

		// if one operand is an empty tupelset, then the result is the empty tupelset
		if ((TP_IS_TUPELSET(t1) && vp1->v.tps->isEmpty()) || (TP_IS_TUPELSET(t2) && vp2->v.tps->isEmpty()))
		{
			tup->t = TPK_TUPELSET_N(1);
			tup->v.tps = glvar->gl_emptytupelset;
			return;
		}

		// both operands must have the same rank
		int r1 = (TP_IS_TUPELSET(t1) ? vp1->v.tps->rank() : RANK_TP_TUPEL(t1));
		int r2 = (TP_IS_TUPELSET(t2) ? vp2->v.tps->rank() : RANK_TP_TUPEL(t2));
		if (r1 != r2 || r1 < 0)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "operands for tupelset intersection don't have the same rank");
			tup->t = TPK_TUPELSET_N(1);
			tup->v.tps = glvar->gl_emptytupelset;
			return;
		}

		// if rank is 0 then both operands are the null tupel
		if (r1 == 0)
		{
			tup->t = TPK_TUPELSET_N(0);
			tup->v.tps = glvar->gl_nulltupelset;
			return;
		}

		// get the reduce rank mode and pattern
		int rrMode, rrDestRank = r2;
		bool *rrPat = NULL, rrAlloc = false;

		if (mode == RANK_REDUCE_TPS) {
			if (TP_IS_TUPELSET(t2)) {
				rrMode = reduceRankMode(glvar, mode, &rrPat, vp2->v.tps, &rrDestRank, &rrAlloc);
			}
			else {
				rrMode = RANK_REDUCE_ALL;
				rrDestRank = 0;
			}
		}
		else if (mode == RANK_REDUCE_PAT) {
			rrPat = pat;
			rrMode = reduceRankMode(glvar, mode, &rrPat, NULL, &rrDestRank, &rrAlloc);
		}
        else {
            rrMode = mode;
            rrDestRank = (mode == RANK_REDUCE_NO ? r2 : 0);
        }

		bool r1valDst = ((rrMode == RANK_REDUCE_NO && r1 == 1) || rrDestRank == 1);

		// if one or both operands are tupel, then the result is this tupel if it is element of the other operand, else the result is the empty tupelset
		if (TP_IS_TUPEL(t1) || TP_IS_TUPEL(t2)) {
			PROTO_OUT_1("-> Tupel ");

			bool b;
			if (TP_IS_TUPEL(t1) && TP_IS_TUPEL(t2))
				b = TupelUtil::cmpEqual(vp1, vp2, false);
			else if (TP_IS_TUPEL(t1))
				b = vp2->v.tps->testElement(glvar, vp1);
			else
				b = vp1->v.tps->testElement(glvar, vp2);

			if (b) {
				if (rrMode == RANK_REDUCE_NO)
					TupelUtil::copyVal(glvar, (TP_IS_TUPEL(t1) ? vp1 : vp2), r1val, tup, glmem);
				else
					TupelUtil::reduceRank(glvar, rrMode, rrPat, rrDestRank, (TP_IS_TUPEL(t1) ? vp1 : vp2), r1val, tup, glmem);
			}
			else {
				tup->t = TPK_TUPELSET_N(1);
				tup->v.tps = glvar->gl_emptytupelset;
			}

			if (rrAlloc)
				free(rrPat);

			return;
		}

		// both operands are tupelsets
		Tupelset *tps1 = vp1->v.tps;
		Tupelset *tps2 = vp2->v.tps;
		val_str *arr;

		// if one operand is the infinite tupelset with all tupels of the rank, then the other operand is the result
		if (tps1->isAllTupelsetOfRank()) {
			reduceRank(glvar, rrMode, rrPat, rrDestRank, vp2, r1val, tup, glmem);
		}
		else if (tps2->isAllTupelsetOfRank()) {
			reduceRank(glvar, rrMode, rrPat, rrDestRank, vp1, r1val, tup, glmem);
		}
		
		// if one or both operands are enumeration tupelsets, then calculate the intersection by enumeration
		else if (tps1->hasEnum() || tps2->hasEnum()) {
			PROTO_OUT_1("-> Enum ");

			// check which tupelset should enumerated
			Tupelset *tpsE, *tpsT;
			if (tps1->hasEnum() && tps2->hasEnum() && !tps1->isInfinite() && !tps2->isInfinite()) {
				tpsE = (tps1->len() <= tps2->len() ? tps1 : tps2);
				tpsT = (tps1->len() <= tps2->len() ? tps2 : tps1);
			}
			else if (!tps1->isInfinite() && (tps1->hasEnum() || tps2->isInfinite())) {
				tpsE = tps1;
				tpsT = tps2;
			}
			else if (!tps2->isInfinite() && (tps2->hasEnum() || tps1->isInfinite())) {
				tpsE = tps2;
				tpsT = tps1;
			}
			else {
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "tupelset intersection not allowed for both infinite enumeration tupelsets");
				tup->t = TPK_TUPELSET_N(1);
				tup->v.tps = glvar->gl_emptytupelset;
				return;
			}

			// enumeration of the one tupelset
			TupelsetIteratorBase *iter = tpsE->newIterContext(glvar, true);
			val_str curtup;
			bool end;
            long cnt = 0;

			arr = (val_str *) calloc(tpsE->len(), sizeof(val_str));

			for (end = iter->start_iter(&curtup); !end; end = iter->set_next(&curtup)) {
				if (tpsT->testElement(glvar, &curtup)) {
					// because curtup is freed after iteration make a copy here
					if (rrMode == RANK_REDUCE_NO) {
						TupelUtil::copyVal(glvar, &curtup, r1valDst, arr+cnt, glmem);
						cnt++;
					}
					else {
						TupelUtil::reduceRank(glvar, rrMode, rrPat, rrDestRank, &curtup, r1valDst, arr+cnt, glmem);
						if (TupelUtil::cmpEqualArr(arr+cnt, arr, cnt, r1valDst) < 0)
							cnt++;
					}
				}
			}

			TupelsetIteratorBase::delIterContext(iter);

			// construct the result tupelset
			if (r1valDst) {
				val_str set;
				SetUtil::constructSetEnum(glvar, arr, cnt, glmem, &set);

				if (r1val)
					*tup = set;
				else
					Tupelset::constructValTupel(glvar, 1, &set, 1, glmem, tup);
			}
			else
				Tupelset::constructValEnum(glvar, arr, cnt, 0, glmem, tup);

			free(arr);
		}
		else {
			PROTO_OUT_1("-> non-Enum ");

			// both tupelsets are non-enum, so calculate the intersection per _ind element
			arr = (val_str *) calloc(tps1->_cnt, sizeof(val_str));
			bool empty = false;
			int i, r;

			for (i=0, r=0; i<tps1->_cnt && !empty; i++) {
				t1 = tps1->_ind[i].t;
				t2 = tps2->_ind[i].t;

				if (TYPECLASS(t1) == TPK_SET) {
					if (TYPECLASS(t2) == TPK_SET) {
						funcNumOp->fset_op(MULT, tps1->_ind[i], tps2->_ind[i]);
						funcNumOp->pop_val(arr+r);

						if (SET_IS_EMPTY(arr[r].t))
							empty = true;
						else if (SET_USE_POINTER(arr[r].t) && !(arr[r].v.set->is_gl_mem) && rrMode != RANK_REDUCE_ALL && (rrMode != RANK_REDUCE_PAT || rrPat[i])) {
							// if the result set the same as one operand set, then change memory allocation to global
							if ((arr[r].t == t1 && arr[r].v.set == tps1->_ind[i].v.set) || (arr[r].t == t2 && arr[r].v.set == tps2->_ind[i].v.set)) {
								glvar->ins_mem(arr[r].v.set);
								arr[r].v.set->is_gl_mem = true;
							}
						}
					}
					else {
						if (SET_TEST_ELEM(tps1->_ind+i, tps2->_ind+i))
							arr[r] = tps2->_ind[i];
						else
							empty = true;
					}
				}
				else {
					if ((TYPECLASS(t2) == TPK_SET && SET_TEST_ELEM(tps2->_ind+i, tps1->_ind+i)) || (t1 == t2 && tps1->_ind[i].v.i == tps2->_ind[i].v.i))
						arr[r] = tps1->_ind[i];
					else
						empty = true;
				}

                if (rrMode != RANK_REDUCE_ALL && (rrMode != RANK_REDUCE_PAT || !rrPat[i]))
					r++;
			}

			if (r1valDst) {
				if (empty)
					tup->t = TP_SET_EMPTY;
				else
					*tup = arr[0];
			}
			else {
				if (empty) {
					tup->t = TPK_TUPELSET_N(1);
					tup->v.tps = glvar->gl_emptytupelset;
				}
				else {
					// construct the result tupelset
					Tupelset::constructValTupel(glvar, r, arr, r, glmem, tup);
				}
			}

			free(arr);
		}

		if (rrAlloc)
			free(rrPat);
	}

	/**
	 * gives the mode for reduced rank, as needed for <code>reduceRank()</code>.
	 * @param glvar     CmplCore object of CMPL
	 * @param modeSrc   reduce mode (one of RANK_REDUCE_*)
	 * @param pat       reduce pattern array if modeSrc==RANK_REDUCE_PAT, else return of the reduce pattern array
	 * @param tps       tupelset which is used as reduce pattern, if modeSrc==RANK_REDUCE_TPS
	 * @param rank   	input of rank of source tupel, return of rank of destination tupel
	 * @param alloc     returns if new memory is allocated for <code>pat</code>
	 * @return          reduce mode (one of RANK_REDUCE_*, but not RANK_REDUCE_TPS)
	 */
	int Tupelset::reduceRankMode(CmplCore *glvar, int modeSrc, bool **pat, Tupelset *tps, int *rank, bool *alloc)
	{
		*alloc = false;

		if (modeSrc == RANK_REDUCE_PAT) {
			int i, dstrank = 0;
			for (i=0; i < *rank; i++) {
				if (*pat[i])
					dstrank++;
			}

			if (dstrank == 0) {
				*rank = 0;
				return RANK_REDUCE_ALL;
			}
			else if (dstrank == *rank) {
				return RANK_REDUCE_NO;
			}
			else {
				*rank = dstrank;
				return RANK_REDUCE_PAT;
			}
		}
		else if (modeSrc == RANK_REDUCE_TPS) {
			if (tps->_rank < 0 || tps->_hasEnum) {
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "enumeration tupelset cannot be a reduce rank pattern");
				return RANK_REDUCE_NO;
			}

			if (tps->_cnt == 0) {
				*rank = 0;
				return RANK_REDUCE_NO;
			}

			int mode = (TYPECLASS(tps->_ind->t) == TPK_SET ? RANK_REDUCE_NO : RANK_REDUCE_ALL);
			if (tps->_freeIndRev && *tps->_freeIndRev)
				mode = (mode == RANK_REDUCE_NO ? RANK_REDUCE_ALL : RANK_REDUCE_NO);
			
			int r, r2, mode2, dstrank = (mode == RANK_REDUCE_NO ? 1 : 0);
			for (r=1; r < tps->_cnt; r++) {
				mode2 = (TYPECLASS(tps->_ind[r].t) == TPK_SET ? RANK_REDUCE_NO : RANK_REDUCE_ALL);
				if (tps->_freeIndRev && tps->_freeIndRev[r])
					mode2 = (mode2 == RANK_REDUCE_NO ? RANK_REDUCE_ALL : RANK_REDUCE_NO);

				if (mode2 == RANK_REDUCE_NO)
					dstrank++;

				if (mode2 != mode) {
					if (mode != RANK_REDUCE_PAT) {
						*pat = (bool *) calloc(tps->_cnt, sizeof(bool));
						*alloc = true;

						for (r2=0; r2 < r; r2++)
                            (*pat)[r2] = (mode==RANK_REDUCE_ALL);

						mode = RANK_REDUCE_PAT;
					}

                    (*pat)[r] = (mode2==RANK_REDUCE_ALL);
				}
			}

			*rank = dstrank;
			return mode;
		}
		else {
			if (modeSrc == RANK_REDUCE_ALL)
				*rank = 0;
			return modeSrc;
		}
	}

	/**
	 * creates a tupelset with a reduced rank.
	 * @param glvar     CmplCore object of CMPL
	 * @param mode      reduce mode (one of RANK_REDUCE_*, but not RANK_REDUCE_TPS)
	 * @param pat       reduce pattern array, length must match rank of tupelset <code>src</code>, only used for RANK_REDUCE_PAT
	 * @param dstrank   rank of destination tupelset or -1 if not known, only used for RANK_REDUCE_PAT
	 * @param src       source tupelset
	 * @param r1val		if dest tupelset has rank 1, then return as set or scalar value
	 * @param dst       result tupelset
	 * @param glmem     global allocation of the memory
	 */
	void Tupelset::reduceRank(CmplCore *glvar, int mode, bool *pat, int dstrank, val_str *src, bool r1val, val_str *dst, bool glmem)
	{
		Tupelset *tps = src->v.tps;
		int i, r;

		PROTO_OUT_4("\n      reduceRank %d %d -> %d", mode, tps->rank(), dstrank);

		// calculate dstrank if necessary
		if (mode == RANK_REDUCE_PAT) {
			if (!TP_IS_TUPELSET(src->t) || tps->rank() < 0) {
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "value must be a tupelset with a known rank");
				mode = RANK_REDUCE_ALL;
			}
			else {
				r = tps->rank();

				if (dstrank < 0) {
					dstrank = 0;
					for (i = 0; i < r; i++) {
                        if (!pat[i])
							dstrank++;
					}
				}
				if (dstrank >= r)
					mode = RANK_REDUCE_NO;
				else if (dstrank == 0)
					mode = RANK_REDUCE_ALL;
			}
		}

		// make the dest tupelset
		if (mode == RANK_REDUCE_PAT || (mode == RANK_REDUCE_NO && tps->hasEnum() && tps->rank() == 1)) {
			val_str *arr;

			if (tps->hasEnum()) {
				// reduce rank by enumeration
				PROTO_OUT_2(" (enum %d)", (int)(tps->len()));

				TupelsetIteratorBase *iter = tps->newIterContext(glvar, true);
				val_str curtup;
				bool end;

				arr = (val_str *) calloc(tps->len(), sizeof(val_str));
				r = 0;

				for (end = iter->start_iter(&curtup); !end; end = iter->set_next(&curtup)) {
					if (mode == RANK_REDUCE_PAT) {
						TupelUtil::reduceRank(glvar, RANK_REDUCE_PAT, pat, dstrank, &curtup, (dstrank==1), arr+r, glmem);
						if (TupelUtil::cmpEqualArr(arr+r, arr, r, (dstrank==1)) < 0)
							r++;
					}
					else {
						TupelUtil::copyVal(glvar, &curtup, true, arr+r, glmem);
						r++;
					}
				}

				TupelsetIteratorBase::delIterContext(iter);

				if (dstrank == 1) {
					val_str set;
					SetUtil::constructSetEnum(glvar, arr, r, glmem, &set);

					if (r1val)
						*dst = set;
					else
						Tupelset::constructValTupel(glvar, 1, &set, 1, glmem, dst);
				}
				else
					Tupelset::constructValEnum(glvar, arr, r, 0, glmem, dst);
			}
			else {
				arr = (val_str *) calloc(dstrank, sizeof(val_str));

				for (i=0, r=0; i<tps->_cnt; i++) {
                    if (!pat[i]) {
						arr[r] = tps->_ind[i];

						if (TYPECLASS(arr[r].t) == TPK_SET && SET_USE_POINTER(arr[r].t) && !(arr[r].v.set->is_gl_mem)) {
							// change memory allocation to global
							glvar->ins_mem(arr[r].v.set);
							arr[r].v.set->is_gl_mem = true;
						}

						r++;
					}
				}

				if (dstrank == 1 && r1val)
					*dst = *arr;
				else
					Tupelset::constructValTupel(glvar, dstrank, arr, dstrank, glmem, dst);
			}

			free(arr);
		}
		else if (mode == RANK_REDUCE_NO) {
			if (tps->rank() == 1 && r1val) {
				if (tps->hasEnum()) {
					PROTO_OUT_2(" (r1val enum %d)", (int)(tps->len()));

					TupelsetIteratorBase *iter = tps->newIterContext(glvar, true);
					val_str curtup;
					bool end;

					val_str *arr = (val_str *) calloc(tps->len(), sizeof(val_str));

					for (end = iter->start_iter(&curtup), r=0; !end; end = iter->set_next(&curtup), r++) {
						TupelUtil::tupelvalToVal(curtup.v.i, arr+r);
					}

					SetUtil::constructSetEnum(glvar, arr, r, glmem, dst);

					TupelsetIteratorBase::delIterContext(iter);
					free(arr);
				}
				else {
					*dst = tps->_ind[0];
				}
			}
			else if (tps->_freeIndRev) {
				// _freeIndRev is not valid after this operation
				if (tps->globAlloc()) {
					Tupelset::constructValTupel(glvar, tps->_cnt, tps->_ind, tps->_cnt, glmem, dst);
				}
				else {
					free(tps->_freeIndRev);
					tps->_freeIndRev = NULL;
					*dst = *src;
				}
			}
			else {
				*dst = *src;
			}
		}
		else {
			if (mode != RANK_REDUCE_ALL)
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "wrong mode for Tupelset::reduceRank");

			dst->t = TPK_TUPELSET_N(0);
			dst->v.tps = glvar->gl_nulltupelset;
		}
	}

	/**
	 * allocates memory for a tupelset object and initialize it.
	 * @param glvar		CmplCore object of CMPL
	 * @param indLen	count for the array of indizes in the tupelset
	 * @param glmem		global allocation of the memory
	 * @return			tupelset object
	 */
	Tupelset* Tupelset::allocTupelset(CmplCore *glvar, int indLen, bool glmem)
	{
		// memory allocation
		// don't use "new", because all memory allocation is done by calloc()
		Tupelset *tps;
		if (glmem)
			tps = (Tupelset *) glvar->c_getmem(sizeof(Tupelset) + indLen * sizeof(val_str));
		else
			tps = (Tupelset *) calloc(1, sizeof(Tupelset) + indLen * sizeof(val_str));

		// init tupelset
		tps->_ind = (val_str *)(tps + 1);
		tps->_cnt = indLen;
		tps->_globAlloc = glmem;

		tps->_fix = tps->_len = tps->_rank = 0;
		tps->_hasEnum = false;

		tps->_freeIndRev = NULL;

		return tps;
	}

	/**
	 * frees the memory for the tupelset tps.
	 */
	void Tupelset::freeTupelset(CmplCore *glvar, Tupelset *tps)
	{
		// don't free the memory if it is globally allocated
		if (tps->_globAlloc)
			return;

		// free memory of contained TupelsetEnum's if not globally allocated
		if (tps->_hasEnum)
		{
			TupelsetEnum *tpe;
			for (int i=0; i<tps->_cnt; i++)
			{
				if (TP_IS_TUPELSET(tps->_ind[i].t))
				{
					tpe = (TupelsetEnum *) tps->_ind[i].v.p;
					TupelsetEnum::freeTupelsetEnum(glvar, tpe);
				}
			}
		}

		if (tps->_freeIndRev)
			free(tps->_freeIndRev);
		
		free(tps);
	}

	/**
	 * changes this object to global memory allocation.
	 * @param glvar     CmplCore object of CMPL
	 */
	void Tupelset::setToGlobAlloc(CmplCore *glvar)
	{
		if (!_globAlloc)
		{
			glvar->ins_mem(this);
			_globAlloc = true;

			if (_freeIndRev)
				glvar->ins_mem(_freeIndRev);
		}

		for (int i=0; i<_cnt; i++)
		{
			if (TP_IS_TUPELSET(_ind[i].t))
			{
				TupelsetEnum *tpe = (TupelsetEnum *) _ind[i].v.p;
				if (! tpe->_globAlloc)
					tpe->setToGlobAlloc(glvar);
			}
			else if (TYPECLASS(_ind[i].t) == TPK_SET && SET_USE_POINTER(_ind[i].t))
			{
				struct set_ea_str_s *set = _ind[i].v.set;
				if (!(set->is_gl_mem)) {
					glvar->ins_mem(set);
					set->is_gl_mem = true;
				}
			}
		}
	}

	/**
	 * test wether the tupelset is the infinite set of all tupels of the rank of the tupelset.
	 * @return          true if the tupelset is the infinite set of all tupels of the rank
	 */
	bool Tupelset::isAllTupelsetOfRank()
	{
		if (_len >= 0 || _rank < 0 || _hasEnum)
			return false;

		for (int i=0; i<_cnt; i++)
		{
			if (_ind[i].t != TP_SET_ALL)
				return false;
		}

		return true;
	}

	/**
	 * test wether a tupel is element of the tupelset.
	 * @param glvar     CmplCore object of CMPL
	 * @param tup       searched tupel
	 * @return          true if the tupel is element of this
	 */
	bool Tupelset::testElement(CmplCore *glvar, val_str *tup)
	{
		// tup must be a tupel of same rank as tupelset this
		//TODO: this has no fixed rank
		if (!TP_IS_TUPEL(tup->t) || RANK_TP_TUPEL(tup->t) != _rank)
			return false;

		int i, j;
		val_str v, *vp;
		TupelsetEnum *tpe;

		// First test fixed elements and algorithmic sets
		//TODO: store a test order of descendend estimated search cost in this and use it here
		for (i=0, j=0, vp=_ind; i < _cnt; i++, j++, vp++)
		{
			if (TYPECLASS(vp->t) == TPK_VALUE || (TYPECLASS(vp->t) == TPK_SET && vp->t != TP_SET_ENUM))
			{
				TupelUtil::tupelvalToVal((RANK_TP_TUPEL(tup->t) == 1 ? tup->v.i : tup->v.tp[j+1]), &v);

				if (TYPECLASS(vp->t) == TPK_VALUE)
				{
					if (vp->t != v.t || vp->v.i != v.v.i)
						return false;
				}
				else
				{
					if (!glvar->setUtil.set_test_elem(vp, &v))
						return false;
				}
			}
			else if (TP_IS_TUPELSET(vp->t))
			{
				j += RANK_TP_TUPEL(vp->t) - 1;
			}
		}

		// Second test other sets
		for (i=0, j=0, vp=_ind; i < _cnt; i++, j++, vp++)
		{
			if (TYPECLASS(vp->t) == TPK_SET && vp->t == TP_SET_ENUM)
			{
				TupelUtil::tupelvalToVal((RANK_TP_TUPEL(tup->t) == 1 ? tup->v.i : tup->v.tp[j+1]), &v);
				if (!glvar->setUtil.set_test_elem(vp, &v))
					return false;
			}
			else if (TP_IS_TUPELSET(vp->t))
			{
				j += RANK_TP_TUPEL(vp->t) - 1;
			}
		}

		// Third test enumeration tupelsets
		for (i=0, j=0, vp=_ind; i < _cnt; i++, j++, vp++)
		{
			if (TP_IS_TUPELSET(vp->t))
			{
				tpe = (TupelsetEnum *) _ind[i].v.p;
				if (!tpe->testElement(glvar, tup, j, RANK_TP_TUPEL(vp->t)))
					return false;

				j += RANK_TP_TUPEL(vp->t) - 1;
			}
		}

		return true;
	}

	/**
	 * creates a new iterator object for this tupelset.
	 * @param glvar     CmplCore object of CMPL
	 * @param freeTup	free current tupel after iterating
	 * @return          tupelset iterator object
	 */
	TupelsetIteratorBase *Tupelset::newIterContext(CmplCore *glvar, bool freeTup)
	{
		// TupelsetIterator wird nur hier verwendet und auch wieder freigegeben, deshalb kann hier auch new/delete verwendet werden
		if (isEnum())
			return new TupelsetEnumIterator(glvar, (TupelsetEnum *) _ind->v.p, freeTup);
		else
			return new TupelsetIterator(glvar, this, freeTup);
	}

	/**
	 * writes a textual form of the tupelset in a string.
	 * @param glvar     CmplCore object of CMPL
	 * @param dest      result buffer
	 * @param maxlen    size of the result buffer
	 * @return result buffer
	 */
	char *Tupelset::asString(CmplCore *glvar, char *dest, int maxlen)
	{
		int i, p;
		val_str *vp;

		if (maxlen <= 0)
			return dest;

		if (_hasEnum || _len==0 || _rank==0 || (_fix==_cnt && !_freeIndRev))
		{
			// output by enumeration
			TupelsetIteratorBase *iter = newIterContext(glvar, true);
			val_str curtup;
			bool end;

			p = snprintf(dest, maxlen, "set(");

			for (i=0, end = iter->start_iter(&curtup); !end && p < maxlen; i++, end = iter->set_next(&curtup))
			{
				if (i > 0)
					p += snprintf(dest + p, maxlen - p, ", ");

				TupelUtil::asString(glvar, dest + p, &curtup, maxlen - p);
				p += strlen(dest + p);
			}

			if (p < maxlen)
				p += snprintf(dest + p, maxlen, ")");

			TupelsetIteratorBase::delIterContext(iter);
		}
		else
		{
			// output by indizes
			p = snprintf(dest, maxlen, "[");

			for (i=0, vp=_ind; i<_cnt && p < maxlen; i++, vp++)
			{
				if (i > 0)
					p += snprintf(dest + p, maxlen - p, ",");

				if (TYPECLASS(vp->t) == TPK_VALUE)
				{
					if (_freeIndRev && _freeIndRev[i])
						p += snprintf(dest + p, maxlen - p, "*");

					if (vp->t == TP_INT)
						p += snprintf(dest + p, maxlen - p, "%ld", vp->v.i);
					else
						p += snprintf(dest + p, maxlen - p, "\"%s\"", glvar->strings.strings_ind[vp->v.i]);
				}
				else if (TYPECLASS(vp->t) == TPK_SET)
				{
					if (_freeIndRev && _freeIndRev[i])
						p += snprintf(dest + p, maxlen - p, "/");

					SetUtil::asString(glvar, dest + p, vp, maxlen - p);
					p += strlen(dest + p);
				}
				else
				{
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "invalid index value type in tupelset");
					p += snprintf(dest + p, maxlen - p, "?");
				}
			}

			if (p < maxlen)
				p += snprintf(dest + p, maxlen, "]");
		}

		dest[maxlen-1] = '\0';
		return dest;
	}


	/**
	 * counts the tupels and tupelsets which are to copy to an enumeration tupelset.
	 * @param cnttps	return count tupelsets to copy
	 * @return			count of tupels to copy
	 */
	int Tupelset::countTupelToEnum(int *cnttps)
	{
		bool freemem = false;
		return ccTupelToEnum(NULL, NULL, NULL, NULL, 0, cnttps, &freemem);
	}

	/**
	 * copies the tupels and tupelsets from this tupelset to the arrays of a <code>TupelsetEnum</code>.
	 * @param tupels		dest array of tupels
	 * @param tupelsets		dest array of tupelsets
	 * @param tpsOrder		dest array of order position numbers for tupelsets
	 * @param curTupel		current index position for tupels, back the position after copy
	 * @param curTps		current index position for tupelsets, back the position after copy
	 * @param freemem		free memory within this tupelset if it is not globally allocated, returns whether the tupelset is to free
	 */
	void Tupelset::copyTupelToEnum(CmplCore *glvar, tupel_t *tupels, Tupelset **tupelsets, int *tpsOrder, int *curTupel, int *curTps, bool *freemem)
	{
		int cntTupel, cntTps;
		cntTupel = ccTupelToEnum(glvar, tupels + *curTupel, tupelsets + *curTps, tpsOrder + *curTps, *curTupel, &cntTps, freemem);
		*curTupel += cntTupel;
		*curTps += cntTps;
	}

	/**
	 * counts and copies the tupels from this tupelset to the arrays of a <code>TupelsetEnum</code>.
	 * @param glvar     CmplCore object of CMPL (only used if copy)
	 * @param atup		dest start position in the array of tupels to copy to / NULL: don't copy, only count
	 * @param atps		dest start position in the array of tupelsets to copy to / NULL: don't copy, only count
	 * @param aord		dest start position in the array of order numbers for tupelsets to copy to / NULL: don't copy, only count
	 * @param ord		start order number for tupelsets
	 * @param cnttps	return count of counted or copied tupelsets
	 * @param freemem	free memory within this tupelset if it is not globally allocated, returns whether the tupelset is to free
	 * @return			count of counted or copied tupels / 0: tupelset is empty / -1: tupelset is infinite
	 */
	int Tupelset::ccTupelToEnum(CmplCore *glvar, tupel_t *atup, Tupelset **atps, int *aord, int ord, int *cnttps, bool *freemem)
	{
		// Fuer leeres oder unendliches Tupelset einfach die Länge zurück; es wird nichts kopiert
		if (_len <= 0)
		{
			*cnttps = 0;
			return _len;
		}

		int copyTupel = 0, copyTps = 0;
		if (! _hasEnum && _len >= 10)	// TODO: Beschränkung als #define
		{
			// Tupelset wird direkt als Tupelset übernommen, wenn es keine Enumeration enthält und nicht nur wenige Elemente hat
			copyTps = 1;
			*freemem = false;

			if (atps)
			{
				*atps = this;
				*aord = ord;
			}
		}
		else if (isEnum())
		{
			// Tupelset ist reine Enumeration, direkt die Elemente von dort übernehmen
			TupelsetEnum *tpe = (TupelsetEnum *) _ind->v.p;
			copyTupel = tpe->_cntTupels;
			copyTps = tpe->_cntTupelsets;

			if (atup)
			{
				int i;
				for (i=0; i<copyTupel; i++)
				{
					atup[i] = tpe->_tupels[i];
				}
				for (i=0; i<copyTps; i++)
				{
					atps[i] = tpe->_tupelsets[i];
					aord[i] = tpe->_tpsOrder[i] + ord;
				}

				if (*freemem && !tpe->_globAlloc)
				{
					// Anzahlen auf 0 setzen, damit bei Speicherfreigabe die kopierten Elemente nicht freigegeben werden
					tpe->_cntTupels = 0;
					tpe->_cntTupelsets = 0;
				}
			}
		}
		else
		{
			// Enumeration durch Iteration
			copyTupel = _len;
			if (atup)
			{
				TupelsetIteratorBase *iter = newIterContext(glvar, false);
				val_str curtup;
				int i, rank;

				for (i=0, iter->start_iter(&curtup); i<copyTupel; i++, iter->set_next(&curtup))
					atup[i] = TupelUtil::tupelFromVal(glvar, &curtup, _globAlloc);
				
				TupelsetIteratorBase::delIterContext(iter);
			}
		}

		*cnttps = copyTps;
		return copyTupel;
	}
	


	//********************************************************
	// TupelsetEnum
	//********************************************************
	
	/**
	 * construct a TupelsetEnum object from the value at the value stack
	 * @param glvar     CmplCore object of CMPL
	 * @param arr       array with tupels
	 * @param cnt       length of arr
	 * @param indRank   if greater 0 then <code>arr</code> contains no tupels but indices which are to combine to tupels of this rank
	 * @param glmem     global allocation of the memory
	 * @return			TupelsetEnum object
	 */
	TupelsetEnum* TupelsetEnum::construct(CmplCore *glvar, val_str *arr, long cnt, int indRank, bool glmem)
	{
		// count elements for TupelsetEnum
		long i;
		int tupcnt = 0, tpscnt = 0, tpscnttmp;
		int rank = -2, maxrank = -2;
		enum etp tp;
		Tupelset *tps;

		if (indRank <= 0)
		{
			for (i=0; i<cnt; i++)
			{
				tp = arr[i].t;

				if (TP_IS_TUPEL(tp))
				{
					tupcnt++;

					if (rank == -2)
						rank = RANK_TP_TUPEL(tp);
					else if (rank >= 0 && rank != RANK_TP_TUPEL(tp))
						rank = -1;

					if (RANK_TP_TUPEL(tp) > maxrank)
						maxrank = RANK_TP_TUPEL(tp);
				}
				else if (TP_IS_TUPELSET(tp))
				{
					tps = arr[i].v.tps;

					if (! tps->isEmpty())
					{
						if (tps->isInfinite())
						{
							SPRINT_ERR_2("infinite tupelset %s cannot be used in enumeration tupelsets", "(tupelset)" /*write_val_to_temp_buf(arr+i)*/);    //TODO
							FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
						}
						else
						{
							tupcnt += tps->countTupelToEnum(&tpscnttmp);
							tpscnt += tpscnttmp;

							if (rank == -2)
								rank = tps->_rank;
							else if (rank >= 0 && rank != tps->_rank)
								rank = -1;

							if (tps->_rank > maxrank)
								maxrank = RANK_TP_TUPEL(tp);
						}
					}
				}
				else
				{
					SPRINT_ERR_2("value %s is not a tupel", "(val)" /*write_val_to_temp_buf(arr+i)*/);    //TODO
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
				}
			}
		}
		else
		{
			if (cnt % indRank == 0)
			{
				tupcnt = cnt / indRank;
				rank = maxrank = indRank;
			}
			else
			{
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "count of indices doen't match the tupel rank");	//TODO
			}
		}

		// allocate and fill the TupelsetEnum
		TupelsetEnum *tpe = TupelsetEnum::allocTupelsetEnum(glvar, tupcnt, tpscnt, rank, maxrank, glmem);
		tpe->initEnum(glvar, arr, cnt, indRank);		

		if (glmem)
			tpe->setToGlobAlloc(glvar);

		return tpe;
	}

	/**
	 * allocates memory for a TupelsetEnum object and initialize it.
	 * @param glvar     CmplCore object of CMPL
	 * @param tupcnt    count for the array of tupels
	 * @param tpscnt    count for the array of tupelsets
	 * @param rank      rank of the TupelsetEnum or -1 if it contains tupels of different ranks
	 * @param maxrank   max rank of tupels in the TupelsetEnum
	 * @param glmem     global allocation of the memory
	 * @return          TupelsetEnum object
	 */
	TupelsetEnum* TupelsetEnum::allocTupelsetEnum(CmplCore *glvar, int tupcnt, int tpscnt, int rank, int maxrank, bool glmem)
	{
		// memory allocation
		// don't use "new", because all memory allocation is done by calloc()
		TupelsetEnum *tpe;
		int size = sizeof(TupelsetEnum) + tupcnt * sizeof(tupel_t) + tpscnt * sizeof(Tupelset*) + tpscnt * sizeof(int) + (rank == -1 ? (maxrank+1) * sizeof(int) : 0);

		if (glmem)
			tpe = (TupelsetEnum *) glvar->c_getmem(size);
		else
			tpe = (TupelsetEnum *) calloc(1, size);

		// init TupelsetEnum
		tpe->_tupels = (tupel_t *)(tpe + 1);
		tpe->_tupelsets = (Tupelset **)(tpe->_tupels + tupcnt);
		tpe->_tpsOrder = (int *)(tpe->_tupelsets + tpscnt);
		tpe->_lenRank = (int *)(rank == -1 ? tpe->_tpsOrder + tpscnt : NULL);

		tpe->_cntTupels = tupcnt;
		tpe->_cntTupelsets = tpscnt;
		tpe->_rank = rank;
		tpe->_maxRank = maxrank;
		tpe->_globAlloc = glmem;

		tpe->_len = 0;
		if (rank == -1)
		{
			for (int i=0; i<=maxrank; i++)
				tpe->_lenRank[i] = 0;
		}

		tpe->_cntRankTupels = NULL;
		tpe->_cntRankTupelsets = NULL;
		tpe->_sort = NULL;
		tpe->_sortRank = NULL;
		tpe->_tpsRank = NULL;

		return tpe;
	}

	/**
	 * initialize a TupelsetEnum from an array of tupels.
	 * @param glvar     CmplCore object of CMPL
	 * @param arr       array with tupels
	 * @param cnt       length of arr
	 * @param indRank	if greater 0 then <code>arr</code> contains no tupels but indices which are to combine to tupels of this rank
	 */
	void TupelsetEnum::initEnum(CmplCore *glvar, val_str *arr, long cnt, int indRank)
	{
		// Tupel eintragen
		long i;
		int tupcnt = 0, tpscnt = 0;
		val_str *vp = NULL;
		tupel_t tup;
		Tupelset *tps;
		bool freetps;

		_len = 0;

		for (i=0; i<cnt; i++)
		{
			if (indRank <= 0)
			{
				vp = arr + i;
			}
			else
			{
				tup = TupelUtil::construct(glvar, indRank, arr + i, indRank, _globAlloc);
				i += (indRank - 1);
			}

			if (!vp || TP_IS_TUPEL(vp->t))
			{
				_tupels[tupcnt++] = (vp ? TupelUtil::tupelFromVal(glvar, vp, _globAlloc) : tup);
				_len++;
			}
			else if (TP_IS_TUPELSET(vp->t))
			{
				tps = vp->v.tps;

				if (! tps->isEmpty() && ! tps->isInfinite())
				{
					freetps = !tps->_globAlloc;
					tps->copyTupelToEnum(glvar, _tupels, _tupelsets, _tpsOrder, &tupcnt, &tpscnt, &freetps);
					_len += tps->_len;

					if (freetps)
						Tupelset::freeTupelset(glvar, tps);
				}
			}
		}

		if (_cntTupels != tupcnt || _cntTupelsets != tpscnt)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, "inconsistent data");	//TODO
		}

		if (_rank == -1)
		{
			// Tupel pro Rang zählen
			for (i=0; i<_cntTupels; i++)
				_lenRank[TupelUtil::rank(_tupels[i])]++;

			for (i=0; i<_cntTupelsets; i++)
				_lenRank[_tupelsets[i]->_rank] += _tupelsets[i]->_len;
		}
	}

	/**
	 * frees the memory for the tupelset tps.
	 * @param glvar     CmplCore object of CMPL
	 * @param tpe       tupelset to free
	 */
	void TupelsetEnum::freeTupelsetEnum(CmplCore *glvar, TupelsetEnum *tpe)
	{
		// don't free the memory if it is globally allocated
		if (tpe->_globAlloc)
			return;

		// free the memory for contained tupels and tupelsets
		int i;
		for (i=0; i<tpe->_cntTupels; i++)
		{
			if (! TupelUtil::globAlloc(tpe->_tupels[i]))
				free(tpe->_tupels[i]);
		}

		for (i=0; i<tpe->_cntTupelsets; i++)
		{
			if (! tpe->_tupelsets[i]->_globAlloc)
				Tupelset::freeTupelset(glvar, tpe->_tupelsets[i]);
		}

		// free the memory for the search indizes
		if (tpe->_cntRankTupels)
			free(tpe->_cntRankTupels);
		if (tpe->_cntRankTupelsets)
			free(tpe->_cntRankTupelsets);
		if (tpe->_sort)
			free(tpe->_sort);
		if (tpe->_sortRank)
			free(tpe->_sortRank);
		if (tpe->_tpsRank)
			free(tpe->_tpsRank);

		free(tpe);
	}

	/**
	 * changes this object to global memory allocation.
	 * @param glvar     CmplCore object of CMPL
	 */
	void TupelsetEnum::setToGlobAlloc(CmplCore *glvar)
	{
		if (!_globAlloc)
		{
			glvar->ins_mem(this);
			_globAlloc = true;

			if (_cntRankTupels)
				glvar->ins_mem(_cntRankTupels);
			if (_cntRankTupelsets)
				glvar->ins_mem(_cntRankTupelsets);
			if (_sort)
				glvar->ins_mem(_sort);
			if (_sortRank)
				glvar->ins_mem(_sortRank);
			if (_tpsRank)
				glvar->ins_mem(_tpsRank);
		}

		int i;
		for (i=0; i<_cntTupels; i++)
		{
			if (! TupelUtil::globAlloc(_tupels[i]))
				TupelUtil::setToGlobAlloc(glvar, _tupels[i]);
		}

		for (i=0; i<_cntTupelsets; i++)
		{
			if (! _tupelsets[i]->_globAlloc)
				_tupelsets[i]->setToGlobAlloc(glvar);
		}
	}

	/**
	 * test wether a part of a tupel is element of the tupelset.
	 * @param glvar     CmplCore object of CMPL
	 * @param tup       tupel
	 * @param ind       start index of the searched tupel part
	 * @param len       length of the searched tupel part
	 * @return          true if the part of tupel is element of this
	 */
	bool TupelsetEnum::testElement(CmplCore *glvar, val_str *tup, int ind, int len)
	{
		// tupelset must contain at least one tupel of searched rank
		if ((_rank >= 0 && _rank != len) || len > _maxRank || (_rank < 0 && _lenRank[len] == 0))
			return false;

		//TODO: fill and use _cntRankTupels, _cntRankTupelsets, _sort, _sortRank and _tpsRank

		// search by iterating the tupelset
		//TODO: implement a more performant search
		TupelsetIteratorBase *iter = newIterContext(glvar, true);
		val_str curtup;
		bool stop, tst;
		int i;
		long v1, v2;

		for (stop = iter->start_iter(&curtup); !stop; stop = iter->set_next(&curtup))
		{
			if (RANK_TP_TUPEL(curtup.t) == len)
			{
				for (i=0, tst=true; i<len && tst; i++)
				{
					v1 = (RANK_TP_TUPEL(tup->t) == 1 ? tup->v.i : tup->v.tp[ind+i+1]);
					v2 = (RANK_TP_TUPEL(curtup.t) == 1 ? curtup.v.i : curtup.v.tp[i+1]);
					tst = (v1 == v2);
				}

				if (tst)
					return true;
			}
		}

		return false;
	}

	/**
	 * creates a new iterator object for this TupelsetEnum.
	 * @param glvar     CmplCore object of CMPL
	 * @param freeTup	free current tupel after iterating
	 * @return          tupelset iterator object
	 */
	TupelsetIteratorBase *TupelsetEnum::newIterContext(CmplCore *glvar, bool freeTup)
	{
		// TupelsetEnumIterator wird nur hier verwendet und auch wieder freigegeben, deshalb kann hier auch new/delete verwendet werden
		return new TupelsetEnumIterator(glvar, this, freeTup);
	}

	

	//********************************************************
	// TupelsetIterator
	//********************************************************
	
	/**
	 * constructor
	 * @param m     	CmplCore object of CMPL
	 * @param tps   	Tupelset object to iterate
	 * @param freeTup	free current tupel after iterating
	 */
	TupelsetIterator::TupelsetIterator(CmplCore *m, Tupelset *tps, bool freeTup)
	{
		glvar = m;
		_tps = tps;
		_freeTup = freeTup;
		_curTupel.t = TPP_EMPTY;
		_firstInd = -1;

		if (_tps->_ind > 0 && _tps->_len != 0 && _tps->_fix < _tps->_cnt)
		{
			_iter = new SetIterator*[_tps->_cnt];

			for (int i = 0; i < _tps->_cnt; i++)
			{
				val_str *vp = _tps->_ind + i;
				if (TP_IS_TUPELSET(vp->t) || TYPECLASS(vp->t) == TPK_SET)
				{
					_iter[i] = new SetIterator(glvar, vp, true);
					if (_firstInd == -1)
						_firstInd = i;
				}
				else
					_iter[i] = NULL;
			}
		}
		else
		{
			_iter = NULL;
		}
	}

	/**
	 * destructor
	 */
	TupelsetIterator::~TupelsetIterator()
	{
		if (TP_IS_TUPEL(_curTupel.t) && RANK_TP_TUPEL(_curTupel.t) >= 2 && TupelUtil::canRelease(_curTupel.v.tp))
		{
			free(_curTupel.v.tp);
			_curTupel.t = TPP_EMPTY;
		}

		if (_iter && _firstInd >= 0)
		{
			for (int i = _firstInd; i < _tps->_cnt; i++)
			{
				if (_iter[i])
				{
					_iter[i]->release();
					delete _iter[i];
				}
			}

			delete _iter;
		}
	}

	/**
	 * starts the iteration.
	 * @param curtup    returns the first tupel of the tupelset
	 * @return          true if the iteration ends (then <code>curtup</code> is not set)
	 */
	bool TupelsetIterator::start_iter(val_str *curtup)
	{
		if (_iter && _firstInd >= 0)
		{
			for (int i = _firstInd; i < _tps->_cnt; i++)
			{
				if (_iter[i])
					_iter[i]->iter_set_start(true);
			}
		}
		
		return setCurElem(curtup, true);
	}

	/**
	 * iterates to the next element of the tupelset
	 * @param curtup    returns the next tupel of the tupelset iteration
	 * @return          true if the iteration ends (then <code>curtup</code> is not set)
	 */
	bool TupelsetIterator::set_next(val_str *curtup)
	{
		if (_iter)
		{
			for (int i = _tps->_cnt - 1; i >= _firstInd; i--)
			{
				if (_iter[i])
				{
					// Wenn diese Iteration noch nicht endet, dann fertig
					if (_iter[i]->iter_set_next())
						break;
					
					// Diese Iteration neu starten, darunterliegende iterieren (nur wenn es eine darunter gibt)
					if (i > _firstInd)
						_iter[i]->iter_set_restart();
				}
			}
		}

		return setCurElem(curtup, false);
	}

	/**
	 * sets <code>_curTupel</code> to the current tupel of the iteration.
	 * @param curtup	returns the current tupel of the iteration.
	 * @param start		iteration just started
	 * @return			true if the iteration ends (then <code>curtup</code> is not set)
	 */
	bool TupelsetIterator::setCurElem(val_str *curtup, bool start)
	{
		// Test ob Iteration endet
		bool stop = false;
		if (_iter && _firstInd >= 0)
		{
			// Ende wenn mindestens ein Iterationsset beendet ist
			for (int i = _firstInd; i < _tps->_cnt; i++)
			{
				if (_iter[i] && _iter[i]->isEnd())
				{
					stop = true;
					break;
				}
			}
		}
		else
		{
			// Ende wenn gar kein Element (ersatzweise auch bei unendlich), sonst Ende wenn nicht Start (da sonst Tupelset nur ein Tupel enthalten kann)
			stop = (_tps->_len <= 0 || !start);
		}

		// Test ob bisheriges Tupel freigebbar
		bool isAlloc = (TP_IS_TUPEL(_curTupel.t) && RANK_TP_TUPEL(_curTupel.t) >= 2);
		bool freeTup = (_freeTup && isAlloc && !TupelUtil::globAlloc(_curTupel.v.tp));

		// Rang fuer neues aktuelles Tupel bestimmen
		int rank = (stop ? -1 : _tps->_rank);
		if (rank < 0 && !stop)
		{
			rank = _tps->_cnt;
			for (int i = 0; i < _tps->_cnt; i++)
			{
				if (TP_IS_TUPELSET(_tps->_ind[i].t))
					rank += RANK_TP_TUPEL(_iter[i]->curVal()->t) - 1;
			}
		}

		// Bisheriges Tupel gegebenenfalls freigeben (aber bei gleichem Rang wiederverwenden)
		if (stop || (TP_IS_TUPEL(_curTupel.t) && (!freeTup || RANK_TP_TUPEL(_curTupel.t) != rank)))
		{
			if (freeTup)
				free(_curTupel.v.tp);
			else if (isAlloc)
				TupelUtil::markNoRelease(_curTupel.v.tp, false);

			_curTupel.t = TPP_EMPTY;
		}

		// Neues Tupel setzen
		if (!stop)
		{
			if (_curTupel.t == TPP_EMPTY && rank >= 2)
			{
				_curTupel.v.tp = (tupel_t) calloc(rank+1, sizeof(long));

				// Tupel erst hier bei der naechsten Iteration freigeben (bzw. gar nicht), deshalb Markierung setzen, damit nicht anderswo freigegeben
				TupelUtil::markNoRelease(_curTupel.v.tp, true);
			}
			_curTupel.t = TPK_TUPEL_N(rank);


			if (rank >= 1)
			{
				val_str *vp;
				bool isTup;
                long tpi;
                int t = 1;

				for (int i = 0; i < _tps->_cnt; i++)
				{
					// Wert fuer die Position holen; wenn kein Tupel, dann schon in Index fuer Tupel wandeln
					if (_iter && _iter[i])
					{
						vp = _iter[i]->curVal();
						isTup = TP_IS_TUPEL(vp->t);

						if (isTup)
						{
							if (RANK_TP_TUPEL(vp->t) == 0)
								continue;
							else if (RANK_TP_TUPEL(vp->t) == 1)
							{
								tpi = vp->v.i;
								isTup = false;
							}
						}	
						else
						{
							tpi = TupelUtil::valToTupelval(vp);
						}
					}
					else
					{
                        isTup = false;
						tpi = TupelUtil::valToTupelval(_tps->_ind + i);
					}

					if (rank == 1)
					{
						// Da nur ein Wert kann es kein Tupel sein; und nach diesem Wert fertig
						_curTupel.v.i = tpi;
						break;
					}
					else
					{
						if (isTup)
						{
							for (int j = 1; j <= RANK_TP_TUPEL(vp->t); j++)
                                _curTupel.v.tp[t++] = vp->v.tp[j];
						}
						else
							_curTupel.v.tp[t++] = tpi;
					}
				}
			}
		}

		*curtup = _curTupel;
		return stop;
	}

	
	//********************************************************
	// TupelsetEnumIterator
	//********************************************************
	
	/**
	 * constructor
	 * @param m     	CmplCore object of CMPL
	 * @param tps   	TupelsetEnum object to iterate
	 * @param freeTup	free current tupel after iterating
	 */
	TupelsetEnumIterator::TupelsetEnumIterator(CmplCore *m, TupelsetEnum *tps, bool freeTup)
	{
		glvar = m;
		_tps = tps;
		_freeTup = freeTup;
		_curTupel.t = TPP_EMPTY;
		_iterTps = NULL;
		_tupelReleasable = false;
	}

	/**
	 * destructor
	 */
	TupelsetEnumIterator::~TupelsetEnumIterator()
	{
		if (_tupelReleasable)
			TupelUtil::markNoRelease(_curTupel.v.tp, false);

		if (_iterTps)
			delete _iterTps;
	}

	/**
	 * starts the iteration.
	 * @param curtup    returns the first tupel of the tupelset
	 * @return          true if the iteration ends (then <code>curtup</code> is not set)
	 */
	bool TupelsetEnumIterator::start_iter(val_str *curtup)
	{
		_curTupel.t = TPP_EMPTY;
		_curTup = -1;
		_curTps = 0;
		_iterTps = NULL;
		_tupelReleasable = false;

		return set_next(curtup);
	}

	/**
	 * iterates to the next element of the tupelset
	 * @param curtup    returns the next tupel of the tupelset iteration
	 * @return          true if the iteration ends (then <code>curtup</code> is not set)
	 */
	bool TupelsetEnumIterator::set_next(val_str *curtup)
	{
		// Gegebenenfalls bisheriges Tupel wieder auf freigebar setzen
		if (_tupelReleasable)
		{
			TupelUtil::markNoRelease(_curTupel.v.tp, false);
			_tupelReleasable = false;
		}

		if (_iterTps)
		{
			// Zum naechsten Element des aktuellen Tupelsets
			if (!_iterTps->set_next(&_curTupel))
			{
				*curtup = _curTupel;
				return false;
			}

			// Aktuelles Tupelset beendet
			delete _iterTps;
			_iterTps = NULL;

			_curTps++;
		}
		else
		{
			// Zum naechsten Element
			_curTup++;
		}

		// Wenn als naechstes Tupelset, dann das durchgehen
		while (_curTps < _tps->_cntTupelsets && _curTup == _tps->_tpsOrder[_curTps])
		{
			// Zum ersten Element des Tupelsets
			_iterTps = new TupelsetIterator(glvar, _tps->_tupelsets[_curTps], _freeTup);
			if (!_iterTps->start_iter(&_curTupel))
			{
				*curtup = _curTupel;
				return false;
			}
			
			// Wenn gar kein Element, dann weiter
			delete _iterTps;
			_iterTps = NULL;
			_curTps++;
		}

		// Einzelnes Tupel
		if (_curTup < _tps->_cntTupels)
		{
			TupelUtil::tupelToVal(_tps->_tupels[_curTup], &_curTupel);

			// Tupel nicht anderswo freigeben
			if (RANK_TP_TUPEL(_curTupel.t) >= 2 && !TupelUtil::canRelease(_curTupel.v.tp))
			{
				_tupelReleasable = true;
				TupelUtil::markNoRelease(_curTupel.v.tp, true);
			}

			*curtup = _curTupel;
			return false;
		}

		return true;
	}
}

