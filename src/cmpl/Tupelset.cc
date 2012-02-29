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
#include "Tupelset.hh"


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
		if (rank >= 1)
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
		// memory allocation
		tupel_t res;
		if (glmem)
			res = (tupel_t) glvar->c_getmem((rank+1) * sizeof(long));
		else
			res = (tupel_t) calloc(rank+1, sizeof(long));

		res[0] = (glmem ? 0x100 : 0) + rank;

		// store tupel values
		storeInd(glvar, res+1, arr, cnt);
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

		for (i=0, vp=arr, dp=dest; i < cnt; vp++)
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
				//TODO: test that vp->v.tps->isTupel(), else error
				r = vp->v.tps->rank();
				if (r >= 1)
				{
					for (j=0; j<r; j++)
						*(dp++) = valToTupelval(vp->v.tps->ind(j));
				}

				if (!vp->v.tps->globAlloc())
				{
					Tupelset::freeTupelset(glvar, vp->v.tps);
					vp->t = TPP_EMPTY;
				}
			}
			else
			{
				//TODO: in TP_INT oder TP_ISTR wandeln
				//TODO: test *vp->v.i whether is valid for tupel
				*(dp++) = valToTupelval(vp);
			}
		}
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
				return NULL;		//TODO: spezielles globales Nulltupel
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
		int i, j, r;
		val_str *vp;
		val_str *dp;
		tupel_t tp;
		Tupelset *tps;
		SetUtil setUtil(glvar);

		bool hasRank = true;
		bool isEmpty = false;
		bool isInfinite = false;

		_len = 1;

		for (i=0, vp=arr, dp=_ind; i < cnt; vp++)
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
					for (j=0; j < tps->_cnt; j++)
						*(dp++) = tps->_ind[j];

					if (!tps->hasRank())
						hasRank = false;

					_fix += tps->_fix;
					_rank += tps->_rank;
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
				//TODO: in TP_INT oder TP_ISTR wandeln
				//TODO: test *vp->v.i whether is valid for tupel
				*(dp++) = *vp;
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

		// TODO: wenn mindestens 2 TupelsetEnum mit jeweils verschiedenrangigen Tupeln enthalten, dann kann _len falsch sein; z.B. { [1], [1,1] } { [1], [1,1] } -> { [1,1], [1,1,1], [1,1,1,1] }
		// 	(aber Fall doch nicht extra berücksichtigen, Tupel existieren dann wirklich doppelt, aber nur erstes zugreifbar da zweites nicht adressierbar)
	}

	/**
	 * constructs a tupelset as a val_str value from an enumeration of tupels.
	 * @param glvar     CmplCore object of CMPL
	 * @param arr       array with tupels
	 * @param cnt       length of arr
  	 * @param glmem     global allocation of the memory
	 * @param tup       pointer to store the constructed tupelset in
	 */
	void Tupelset::constructValEnum(CmplCore *glvar, val_str *arr, long cnt, bool glmem, val_str *tup)
	{
		Tupelset *tps = Tupelset::allocTupelset(glvar, 1, glmem);

		TupelsetEnum *tpe = TupelsetEnum::construct(glvar, arr, cnt, glmem);
		tps->_ind[0].v.p = tpe;
		tps->_ind[0].t = (tpe->_rank >= 0 ? TPK_TUPELSET_N(tpe->_rank) : TPK_TUPELSET_N(1));

		tps->_len = tpe->_len;
		tps->_rank = tpe->_rank;
		tps->_hasEnum = 1;

		tup->v.p = tps;
		tup->t = tps->_ind[0].t;
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
				glvar->ins_mem(set);
				set->is_gl_mem = true;
			}
		}
	}


	/**
	 * counts the tupels and tupelsets which are to copy to an enumeration tupelset.
	 * @param cnttps	return count tupelsets to copy
	 * @return			count of tupels to copy
	 */
	int Tupelset::countTupelToEnum(int *cnttps)
	{
		bool freemem = false;
		return ccTupelToEnum(NULL, NULL, NULL, 0, cnttps, &freemem);
	}

	/**
	 * @param tupels		dest array of tupels
	 * @param tupelsets		dest array of tupelsets
	 * @param tpsOrder		dest array of order position numbers for tupelsets
	 * @param curTupel		current index position for tupels, back the position after copy
	 * @param curTps		current index position for tupelsets, back the position after copy
	 * @param freemem		free memory within this tupelset if it is not globally allocated, returns whether the tupelset is to free
	 */
	void Tupelset::copyTupelToEnum(tupel_t *tupels, Tupelset **tupelsets, int *tpsOrder, int *curTupel, int *curTps, bool *freemem)
	{
		int cntTupel, cntTps;
		cntTupel = ccTupelToEnum(tupels + *curTupel, tupelsets + *curTps, tpsOrder + *curTps, *curTupel, &cntTps, freemem);
		*curTupel += cntTupel;
		*curTps += cntTps;
	}

	/**
	 * counts and copies the tupels from this tupelset to the arrays of a <code>TupelsetEnum</code>.
	 * @param atup		dest start position in the array of tupels to copy to / NULL: don't copy, only count
	 * @param atps		dest start position in the array of tupelsets to copy to / NULL: don't copy, only count
	 * @param aord		dest start position in the array of order numbers for tupelsets to copy to / NULL: don't copy, only count
	 * @param ord		start order number for tupelsets
	 * @param cnttps	return count of counted or copied tupelsets
	 * @param freemem	free memory within this tupelset if it is not globally allocated, returns whether the tupelset is to free
	 * @return			count of counted or copied tupels / 0: tupelset is empty / -1: tupelset is infinite
	 */
	int Tupelset::ccTupelToEnum(tupel_t *atup, Tupelset **atps, int *aord, int ord, int *cnttps, bool *freemem)
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
				//TODO
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
	 * @param glmem     global allocation of the memory
	 * @return			TupelsetEnum object
	 */
	TupelsetEnum* TupelsetEnum::construct(CmplCore *glvar, val_str *arr, long cnt, bool glmem)
	{
		// Anzahl Elemente für TupelsetEnum bestimmen
		long i;
		int tupcnt = 0, tpscnt = 0, tpscnttmp;
		int rank = -2, maxrank = -2;
		enum etp tp;
		Tupelset *tps;

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

				if (RANK_TP_TUPEL(arr[i].t) > maxrank)
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

		// allocate and fill the TupelsetEnum
		TupelsetEnum *tpe = TupelsetEnum::allocTupelsetEnum(glvar, tupcnt, tpscnt, rank, maxrank, glmem);
		tpe->initEnum(glvar, arr, cnt);		

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
	 */
	void TupelsetEnum::initEnum(CmplCore *glvar, val_str *arr, long cnt)
	{
		// Tupel eintragen
		long i;
		int tupcnt = 0, tpscnt = 0;
		enum etp tp;
		Tupelset *tps;
		bool freetps;

		_len = 0;

		for (i=0; i<cnt; i++)
		{
			tp = arr[i].t;

			if (TP_IS_TUPEL(tp))
			{
				_tupels[tupcnt++] = TupelUtil::tupelFromVal(glvar, arr+i, _globAlloc);
				_len++;
			}
			else if (TP_IS_TUPELSET(tp))
			{
				tps = arr[i].v.tps;

				if (! tps->isEmpty() && ! tps->isInfinite())
				{
					freetps = !tps->_globAlloc;
					tps->copyTupelToEnum(_tupels, _tupelsets, _tpsOrder, &tupcnt, &tpscnt, &freetps);
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
}

