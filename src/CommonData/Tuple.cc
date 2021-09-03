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


#include "Tuple.hh"
#include "SetIterator.hh"

using namespace std;


namespace cmpl
{
	/**
	 * constructor
	 * @param rank			initial rank of the tuple
	 * @param cap			capacity for the tuple / 0: same as rank
	 * @param ttp			tuple type
	 */
	Tuple::Tuple(unsigned rank, unsigned cap, TupleType ttp)
	{
		_rank = rank;
		_cap = (cap > rank ? cap : rank);
		_tupleType = ttp;
		_iterSet.unset();

		if (_cap <= TUPLE_MIN_CAPACITY)
			_array = _arrMin;
		else
			_array = new CmplVal[_cap];

		// initializing of the tuple elements is only necessary, when it can be disposable cmpl objects
		if (ttp != tupleIndexOnly) {
			CmplVal *vp = _array;
			for (unsigned i = 0; i < _cap; i++, vp++)
				vp->unset();
		}
	}

	/**
	 * constructor with iteration set, only called from SetIterator
	 * @param set			iteration set
	 * @param rank			initial rank of the tuple
	 * @param cap			capacity for the tuple / 0: same as rank
	 */
    Tuple::Tuple(const CmplVal &set, unsigned rank, unsigned cap)
	{
		_rank = rank;
		_cap = (cap > rank ? cap : rank);
		_tupleType = tupleIndexOnly;

        _iterSet.copyFrom(set, true, false);

		if (_cap <= TUPLE_MIN_CAPACITY)
			_array = _arrMin;
		else
			_array = new CmplVal[_cap];
	}

	/**
	 * destructor
	 */
	Tuple::~Tuple()
	{
		if (_tupleType != tupleIndexOnly) {
			CmplVal *vp = _array;
			for (unsigned i = 0; i < _rank; i++, vp++)
				vp->dispose();
		}

		if (_cap > TUPLE_MIN_CAPACITY)
			delete _array;

		if (_iterSet)
			_iterSet.dispose(); 
	}


	/**
	 * set rank
	 * @param rank		new rank for the tuple
	 * @param disp		dispose values of removed elements (not used for marked index tuples)
	 */
	void Tuple::setRank(unsigned rank, bool disp)
	{
		if (rank > _cap) {
			throw out_of_range("tuple rank");
		}

		if (_tupleType != tupleIndexOnly && disp) {
			CmplVal *vp = _array;
			unsigned i;

			if (rank < _rank) {
				for (i = rank; i < _rank; i++, vp++)
					vp->dispose();
			}
			else if (rank > _rank) {
				for (i = _rank; i < rank; i++, vp++)
					vp->unset();
			}
		}
		
		_rank = rank;
	}


    /**
     * count tuple parts with given type
     * @param t         type
     * @return          count of tuple parts with given type
     */
    unsigned Tuple::countPartType(tp_e t) const
    {
        unsigned res = 0;

        CmplVal *vp = _array;
        for (unsigned i = 0; i < _rank; i++, vp++) {
            if (vp->t == t)
                res++;
        }

        return res;
    }


    /**
     * get part of this tuple
     * @param res           return result
     * @param f             first element of part
     * @param r             rank of part
     */
    void Tuple::partTuple(CmplVal& res, unsigned f, unsigned r) const
    {
        if (r == 0 || f >= _rank) {
            res.set(TP_ITUPLE_NULL);
        }
        else {
            if (f + r > _rank)
                r = _rank - f;

            if (r == 1) {
                res.copyFrom(_array + f, true, false);
            }
            else {
                Tuple *tp = new Tuple(r, 0, _tupleType);
                res.set((_tupleType==tupleIndexOnly ? TP_ITUPLE : TP_TUPLE), tp);

                CmplVal *p = _array + f;
                for (unsigned i = 0; i < r; i++)
                    tp->at(i)->copyFrom(p + i, true, false);
            }
        }
    }


    /**
     * construct new tuple by concatenation of two tuples
     * @param res           return result
     * @param fi            first source tuple or value
     * @param sc            second source tuple or value
     */
    void Tuple::constructTuple(CmplVal& res, const CmplVal& fi, const CmplVal& sc)
    {
        unsigned rfi = (fi.isTuple() ? Tuple::rank(fi) : (fi ? 1 : 0));
        unsigned rsc = (sc.isTuple() ? Tuple::rank(sc) : (sc ? 1 : 0));
        unsigned rr = rfi + rsc;

        if (rr == 0) {
            res.set(TP_ITUPLE_NULL);
            return;
        }
        else if (rr == 1) {
            const CmplVal& rt = (rfi == 1 ? fi : sc);
            if (rt.isTuple() || rt.isScalarIndex()) {
                if (rt.isTuple())
                    res.copyFrom(rt);
                else
                    res.set(TP_INDEX_VAL_TUPLE(rt.t), rt.v.i);

                return;
            }
        }

        Tuple *tpl = new Tuple(rr);
        res.set(TP_TUPLE, tpl);

        bool itpl = true;
        for (unsigned r = 0; r < rr; r++) {
            const CmplVal *v = (r < rfi ? Tuple::at(fi, r) : Tuple::at(sc, r - rfi));
            tpl->at(r)->copyFrom(v, true, false);
            if (!v->isScalarIndex())
                itpl = false;
        }

        if (itpl) {
            res.t = TP_ITUPLE;
        }
    }

    /**
     * construct new tuple by concatenation of tuples
     * @param res           return result
     * @param src           source tuples
     */
    void Tuple::constructTuple(CmplVal& res, const vector<CmplValAuto>& src)
    {
        unsigned rr = 0;
        for (const CmplValAuto& t : src)
            rr += (t.isTuple() ? Tuple::rank(t) : (t ? 1 : 0));

        if (rr == 0) {
            res.set(TP_ITUPLE_NULL);
            return;
        }
        else if (rr == 1) {
            for (const CmplValAuto& t : src) {
                if (t && (!t.isTuple() || Tuple::rank(t) > 0)) {
                    if (t.isTuple() || t.isScalarIndex()) {
                        if (t.isTuple())
                            res.copyFrom(t);
                        else
                            res.set(TP_INDEX_VAL_TUPLE(t.t), t.v.i);

                        return;
                    }
                }
            }
        }

        Tuple *tpl = new Tuple(rr);
        res.set(TP_TUPLE, tpl);

        bool itpl = true;
        unsigned r = 0;
        for (const CmplValAuto& t : src) {
            unsigned sr = (t.isTuple() ? Tuple::rank(t) : (t ? 1 : 0));
            if (sr) {
                if (sr == 1) {
                    tpl->at(r)->copyFrom(t, true, false);
                    r++;
                }
                else {
                    Tuple *st = t.tuple();
                    for (unsigned i = 0; i < sr; i++, r++)
                        tpl->at(r)->copyFrom(st->at(i), true, false);
                }

                if (!t.isScalarIndex() && !t.isITuple())
                    itpl = false;
            }
        }

        if (itpl)
            res.t = TP_ITUPLE;
    }


    /**
     * concat all current tuples of given set iterators to this tuple (must have enough capacity)
     * @param vsi           vector of set iterators
     * @param ssi           start copy of tuple parts with this set iterator
     */
    void Tuple::concatIter(vector<SetIterator>& vsi, unsigned ssi)
    {
        unsigned r = 0;
        for (unsigned i = 0; i < vsi.size(); i++)
        {
            const CmplVal& ct = vsi[i].curTuple();
            unsigned tr = (ct.isTuple() ? Tuple::rank(ct) : (ct.isScalarIndex() ? 1 : 0));
            if (tr)
            {
                if (r + tr > _cap) {
                    if (_rank < r)
                        _rank = r;
                    throw out_of_range("tuple element index");
                }

                if (i >= ssi) {
                    if (ct.useValP()) {
                        Tuple *s = ct.tuple();
                        for (unsigned j = 0; j < tr; j++)
                            _array[r++].copyFrom(s->at(j), true, true);
                    }
                    else {
                        _array[r++].set(TP_INDEX_VAL_BASE(ct.t), ct.v.i);
                    }
                }
                else {
                    r += tr;
                }
            }
        }

        if (r < _rank) {
            for (unsigned j = r; j < _rank; j++)
                _array[j].dispUnset();
        }

        _rank = r;
    }


	/**
	 * write contents of the object to a stream
	 * @param modp			calling module
	 * @param mode			mode for output: 0=direct; 1=part of other value
	 */
	void Tuple::write(ostream& ostr, ModuleBase *modp, int mode) const
	{
		if (_rank == 0) {
			ostr << "[null]";
		}
		else {
			ostr << '[';

			CmplVal *vp = _array;
			for (unsigned i = 0; i < _rank; i++, vp++) {
				if (i > 0)
					ostr << ", ";

				vp->write(ostr, modp, 1);
			}

			ostr << ']';
		}
	}


	/**
	 * change an index tuple value to an equivalent special value if possible
	 * @param v			value to change to special value
	 * @param disp		dispose cmpl value object
	 * @return			value is changed
	 */
	bool Tuple::toSpecialValTuple(CmplVal& v, bool disp)
	{
		if (v.t != TP_ITUPLE || v.tuple()->rank() > 1)
			return false;

		Tuple *tp = v.tuple();

		if (tp->rank() == 0)
			v.set(TP_ITUPLE_NULL);
		else if (tp->at(0)->t == TP_INT)
			v.set((tp->at(0)->t == TP_INT ? TP_ITUPLE_1INT : TP_ITUPLE_1STR), tp->at(0)->v.i);
		else
			return false;

		if (disp && tp->decRef())
			delete tp;

		return true;
	}
}

