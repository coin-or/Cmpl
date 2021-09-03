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


#include "ValueTree.hh"


using namespace std;


namespace cmpl
{
    /****** ValueTreeElem ****/

    /**
     * get root of value tree, to which this belongs
     * @return      root of value tree, or NULL if not in a value tree
     */
    ValueTree *ValueTreeElem::getRoot() const
    {
        return (_level >= 0 ? (_level == 0 ? (ValueTree*)this : _parent->getRoot()) : NULL);
    }

    /**
     * set or move element within the value tree
     * @param vtr	root of value tree, to which this belongs
     * @param fi	first index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE), or TP_EMPTY then <code>sc</code> holds only the remaining tuple parts up from <code>sub</code>
     * @param sc	second index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
     * @param sub	subtree for the element if known
     * @return		0:ok; 1:place already occupied
     */
    int ValueTreeElem::setInValueTree(ValueTreeRoot *vtr, const CmplVal &fi, const CmplVal &sc, ValueTree *sub)
    {
        //TODO: fi/sc is TP_LIST_TUPLE (oder generell vorher in normales ITUPEL wandeln?)
        unsigned rfi = (fi.isTuple() ? Tuple::rank(fi) : (fi ? 1 : 0));
        unsigned rsc = (sc.isTuple() ? Tuple::rank(sc) : (sc ? 1 : 0));
        unsigned rb = (sub ? sub->_level : 0);
        unsigned re = (sub && !fi ? (rb + rsc) : (rfi + rsc));

        if (!sub)
            sub = &(vtr->_tree);

        for (unsigned r = rb; r < re; r++) {
            const CmplVal *v = (fi && r < rfi ? Tuple::at(fi, r) : Tuple::at(sc, r - (fi ? rfi : rb)));
            bool ii = (v->t != TP_STR && v->t != TP_ITUPLE_1STR);

            if (r < re - 1) {
                map<intType, ValueTree *>& mvt = (ii ? sub->_subInt : sub->_subStr);
                if (mvt.count(v->v.i))
                    sub = mvt[v->v.i];
                else {
                    sub = new ValueTree(sub, sub->_level + 1, ii, v->v.i);
                    mvt[v->v.i] = sub;
                }
            }
            else {
                map<intType, ValueTreeElem *>& mvt = (ii ? sub->_leafInt : sub->_leafStr);
                if (mvt.count(v->v.i))
                    return 1;
                else {
                    if (_parent != sub || _level != sub->_level + 1 || _indexInt != ii || _parIndex != v->v.i) {
                        // remove from previous place
                        if (_level > 0 && _parent) {
                            map<intType, ValueTreeElem *>& pvt = (_indexInt ? _parent->_leafInt : _parent->_leafStr);
                            pvt.erase(_parIndex);
                        }

                        _parent = sub;
                        _level = sub->_level + 1;
                        _indexInt = ii;
                        _parIndex = v->v.i;
                        mvt[v->v.i] = this;
                    }
                }
            }
        }

        return 0;
    }

    /**
     * take element out of the value tree
     */
    void ValueTreeElem::unsetInValueTree()
    {
        if (isInTree()) {
            map<intType, ValueTreeElem *>& pvt = (_indexInt ? _parent->_leafInt : _parent->_leafStr);
            pvt.erase(_parIndex);

            _parent = NULL;
            _level = -1;
        }
    }

    /**
     * remove element from the value tree, also from store by identy number
     * @param vtr   root of value tree, to which this belongs
     */
    void ValueTreeElem::removeFromValueTree(ValueTreeRoot *vtr)
    {
        unsetInValueTree();
        vtr->_flat[_id - 1] = NULL;

        if (vtr->_hasArrFlat)
            vtr->_hasArrFlat = false;
        if (vtr->_hasArrTree)
            vtr->_hasArrTree = false;
    }


    /**
     * get single index value of a certain level of the element in the tree
     * @param level             level of searched index value
     * @param res               return of result value
     */
    void ValueTreeElem::getIndexOfLevel(unsigned level, CmplVal &res) const
    {
        if ((int)level < _level)
            _parent->getIndexOfLevel(level, res);
        else if ((int)level == _level)
            res.set((_indexInt ? TP_INT : TP_STR), _parIndex);
        else
            res.unset();
    }

    /**
     * get tuple of index values for a given range of level of the element in the tree
     * @param levelFrom         level from of searched index tuple
     * @param levelTo           level to of searched index tuple or 0 for up to the level of this
     * @param res               return of result value
     */
    void ValueTreeElem::getIndexOfLevel(unsigned levelFrom, unsigned levelTo, CmplVal &res) const
    {
        if (!isInTree()) {
            res.unset();
            return;
        }

        if (levelFrom < 1)
            levelFrom = 1;

        if (levelTo == 0 || levelTo > (unsigned)_level)
            levelTo = _level;

        int r = levelTo + 1 - levelFrom;

        if (r <= 0) {
            res.set(TP_ITUPLE_NULL);
        }
        else if (r == 1) {
            getIndexOfLevel(levelFrom, res);
            res.t = TP_INDEX_VAL_TUPLE(res.t);
        }
        else {
            Tuple *tpl = new Tuple(r, 0, Tuple::tupleIndexOnly);
            res.set(TP_ITUPLE, tpl);

            unsigned lvl = levelTo;
            const ValueTreeElem *ve = this;
            while ((int)lvl > ve->_level) {
                ve = ve->_parent;
                lvl--;
            }

            while (lvl >= levelFrom) {
                tpl->at(--r)->set((ve->_indexInt ? TP_INT : TP_STR), ve->_parIndex);
                ve = ve->_parent;
                lvl--;
            }
        }
    }


    /**
     * insert name for usage on formatted outputs in map of names
     * @param sstore			stored string values
     * @param names				map of names, key is the name, value is the id of this ValueTreeElem
     * @param prefStd			prefix for standard name if the element have no name in the value tree / NULL if no standard name
     * @param prefSubs			prefix for substitute name, used if a regular name not exists or is not unique
     * @param cntSubs			counter for substitute name
     * @param quote				quote char for strings, 0 if no one
     * @param rplSp				replacement char for space, 0 for removing spaces, space char for leaving spaces unchanged
     * @param mLen				max length for result name
     * @return					true if the regular name is used, false if the substitute name is used
     */
    bool ValueTreeElem::insOutName(StringStore *sstore, unordered_map<string, unsigned long>& names, const char *prefStd, const char *prefSubs, unsigned& cntSubs, char quote, char rplSp, unsigned mLen)
    {
        if (_level == 1 && !_indexInt) {
            // simple name without indexes: check if directly suitable as result
            string s = StringStore::asName(string(sstore->at(_parIndex)), rplSp, mLen);
            if (names.count(s) == 0) {
                names[s] = _id;
                return true;
            }
        }

        string s;
        if (_level >= 1) {
            ostringstream ostr;
            recOutName(ostr, sstore, quote, true);
            s = StringStore::asName(ostr.str(), rplSp, mLen);
        }
        else if (prefStd) {
            ostringstream ostr;
            ostr << prefStd << _id;
            s = StringStore::asName(ostr.str(), rplSp, mLen);
        }

        bool subst = false;
        while (s.empty() || names.count(s)) {
            ostringstream ostr;
            ostr << prefSubs << ++cntSubs;
            s = ostr.str();
            subst = true;
        }

        names[s] = _id;
        return subst;
    }


    /**
     * recursive output of name parts of this value tree element to stream
     * @param ostr				stream for output
     * @param sstore			stored string values
     * @param quote				quote char for strings, 0 if no one
     * @param leaf				true if this is the leaf element
     */
    void ValueTreeElem::recOutName(ostringstream& ostr, StringStore *sstore, char quote, bool leaf)
    {
        if (_level == 1) {
            // first level counts as name, never quote strings here
            if (_indexInt)
                ostr << _parIndex;
            else
                ostr << sstore->at(_parIndex);
        }
        else if (_level > 1) {
            _parent->recOutName(ostr, sstore, quote, false);
            ostr << (_level == 2 ? '[' : ',');

            if (_indexInt)
                ostr << _parIndex;
            else if (quote)
                ostr << quote << sstore->at(_parIndex) << quote;
            else
                ostr << sstore->at(_parIndex);

            if (leaf)
                ostr << ']';
        }
    }



    /****** ValueTree ****/

    /**
     * destructor
     */
    ValueTree::~ValueTree()
    {
        // delete here only subtree elements, because leaf elements are referenced also in ValueTreeRoot::_flat
        for (unsigned i = 0; i < 2; i++) {
            map<intType, ValueTree *>& st = (i==0 ? _subInt : _subStr);
            for (map<intType, ValueTree *>::iterator it = st.begin(); it != st.end(); it++) {
                ValueTree *vt = it->second;
                delete vt;
            }
        }
    }



    /****** ValueTreeRoot ****/

    /**
	 * destructor
	 */
	ValueTreeRoot::~ValueTreeRoot()
	{
        for (unsigned i = 0; i < _flat.size(); i++) {
            ValueTreeElem *vte = _flat[i];
            if (vte) {
                CmplObjBase *o = dynamic_cast<CmplObjBase *>(vte);
                if (o)
                    CmplObjBase::dispose(o);
                else
                    delete vte;
            }
        }

        if (_arrFlat)
            delete _arrFlat;

        if (_arrTree)
            delete _arrTree;
	}


    /**
     * insert a new element, but only in vector by id, but not in value tree
     * @param e		new element (not used already in a ValueTreeRoot)
     * @return		new element <code>e</code>
     */
    ValueTreeElem *ValueTreeRoot::insertNewElem(ValueTreeElem *e)
    {
        _flat.push_back(e);
        e->_id = _flat.size();
        e->_parent = &_tree;
        e->_level = -1;

        if (_hasArrFlat) {
            //TODO: vielleicht besser Element in _arrFlat anfuegen
            _hasArrFlat = false;
        }

        return e;
    }


    /**
	 * insert a new element
	 * @param e		new element (not used in another ValueTreeRoot)
     * @param fi	first index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE), or TP_EMPTY then <code>sc</code> holds only the remaining tuple parts up from <code>sub</code>
	 * @param sc	second index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
	 * @param sub	subtree for the element if known
	 * @return		new element <code>e</code>
	 */
	ValueTreeElem *ValueTreeRoot::insertNewElem(ValueTreeElem *e, CmplVal &fi, CmplVal &sc, ValueTree *sub)
    {
        insertNewElem(e);
        e->setInValueTree(this, fi, sc, sub);
        _hasArrTree = false;
		return e;
	}


	/**
	 * get the element by index
	 * @param fi	first index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
	 * @param sc	second index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
	 * @return		element
	 */
    ValueTreeElem *ValueTreeRoot::getElem(CmplVal &fi, CmplVal &sc) const
	{
		//TODO
		return NULL;
	}

}

