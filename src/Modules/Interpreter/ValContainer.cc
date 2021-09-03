
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


#include "ValContainer.hh"
#include "ExecContext.hh"
#include "ValueStore.hh"
#include "Interpreter.hh"
#include "../../Control/MainControl.hh"



namespace cmpl
{

    /*********** ValContainer **********/

    /**
     * destructor
     */
    ValContainer::~ValContainer()
    {
        for (map<unsigned, SubElem*>::iterator it1 = _symMap.begin(); it1 != _symMap.end(); it1++) {
            SubElem *sub = it1->second;
            delete sub;
        }
    }


    /**
     * get symbol value for subsymbol fetch, insert new symbol value if necessary
     * @param ctx       execution context
     * @param cd        intermediary code command for subsymbol fetch
     * @return          symbol value pointer
     */
    SymbolValue *ValContainer::getInsertSymbol(ExecContext *ctx, IntCode::IcElem *cd)
    {
        unsigned pos = cd - ctx->codeBase();
        unsigned symnr = cd[1].v.n.n1;

        bool subName = (_symMap.count(symnr) != 0);
        SubElem *sub;

        if (subName) {
            sub = _symMap[symnr];
            while (sub && !sub->checkPos(pos))
                sub = sub->next;
        }
        else {
            sub = NULL;
        }

        if (!(cd->v.c.par & ICPAR_FETCH_LVALUE)) {
            if (!sub) {
                const char *errmsg = ctx->modp()->ctrl()->printBuffer("subsymbol '%s' not %s", ctx->modp()->symbolInfo(symnr)->name(), (subName ? "accessible" : "defined"));
                const SyntaxElement *sep = ctx->modp()->syntaxElement(cd->se);
                ctx->modp()->ctrl()->errHandler().error(ERROR_LVL_NORMAL, errmsg, sep->loc());
            }
        }

        else if (!sub || sub->defPos != pos) {
            bool insNew = (cd->v.c.par & (ICPAR_FETCH_LVAL_NEW | ICPAR_FETCH_LVAL_PUBLIC | ICPAR_FETCH_LVAL_PRIVATE));
            if (insNew || !sub) {
                if (_finalized) {
                    const char *errmsg = ctx->modp()->ctrl()->printBuffer("subsymbol '%s' cannot be inserted in a finalized container", ctx->modp()->symbolInfo(symnr)->name());
                    const SyntaxElement *sep = ctx->modp()->syntaxElement(cd->se);
                    ctx->modp()->ctrl()->errHandler().error(ERROR_LVL_NORMAL, errmsg, sep->loc());
                }
                else {
                    // get validity range
                    unsigned vf = 0, vt = 0;
                    if (cd[1].v.n.n2) {
                        IntCode::IcElem *v = ctx->codeBase() + cd[1].v.n.n2;
                        vf = v->v.n.n1;
                        vt = v->v.n.n2;
                    }

                    SubElem *subErr = NULL;
                    if (!(cd->v.c.par & ICPAR_FETCH_LVAL_NEW)) {
                        if (sub) {
                            subErr = sub;
                        }
                        else if (subName) {
                            // check if validity of new subsymbol would overlap another
                            SubElem *s = _symMap[symnr];
                            while (s && !subErr) {
                                if (!(s->defNew) && (!vt || s->validFrom < vt) && (!(s->validTo) || s->validTo > vf))
                                    subErr = s;
                                s = s->next;
                            }
                        }
                    }

                    if (!subErr) {
                        sub = new SubElem(vf, vt, pos, (cd->v.c.par & ICPAR_FETCH_LVAL_NEW));
                        sub->symVal.init(ctx->modp()->symbolInfo(symnr), true, false);

                        if (subName) {
                            // insert in list of subelems in desc order of validFrom
                            SubElem *s = _symMap[symnr];
                            if (s->validFrom <= vf) {
                                sub->next = s;
                                _symMap[symnr] = sub;
                            }
                            else {
                                while (true) {
                                    SubElem *s2 = s->next;
                                    if (!s2 || s2->validFrom <= vf) {
                                        sub->next = s2;
                                        s->next = sub;
                                        break;
                                    }
                                    s = s2;
                                }
                            }
                        }
                        else {
                            _symMap[symnr] = sub;
                        }
                    }
                    else {
                        const SyntaxElement *sep1 = ctx->modp()->syntaxElement(cd->se);
                        unsigned se = subErr->syntaxElem(ctx->codeBase());
                        if (se) {
                            const SyntaxElement *sep2 = ctx->modp()->syntaxElement(se);
                            const char *errmsg = ctx->modp()->ctrl()->printBuffer("subsymbol '%s'", ctx->modp()->symbolInfo(symnr)->name());
                            ctx->modp()->ctrl()->errHandler().error(ERROR_LVL_NORMAL, errmsg, sep1->loc(), "is already defined", sep2->loc());
                        }
                        else {
                            const char *errmsg = ctx->modp()->ctrl()->printBuffer("subsymbol '%s' is a predefined symbol", ctx->modp()->symbolInfo(symnr)->name());
                            ctx->modp()->ctrl()->errHandler().error(ERROR_LVL_NORMAL, errmsg, sep1->loc());
                        }

                        sub = NULL;
                    }
                }
            }
        }

        return (sub ? &(sub->symVal) : NULL);
    }

    /**
     * get symbol value for subsymbol
     * @param symnr     symbol number
     * @param pos       position in intermediary code / NULL: don't pay attention to validity range
     * @return          symbol value pointer / NULL: not found
     */
    SymbolValue *ValContainer::getSimple(unsigned symnr, unsigned pos)
    {
        if (_symMap.count(symnr) == 0)
            return NULL;

        SubElem *sub = _symMap[symnr];
        if (pos) {
            while (sub && !sub->checkPos(pos))
                sub = sub->next;
        }

        return (sub ? &(sub->symVal) : NULL);
    }

    /**
     * insert new symbol value object for predefined subsymbol
     * @param modp      interpreter module
     * @param symId     symbol number
     * @return          symbol value pointer
     */
    SymbolValue *ValContainer::insertPredef(Interpreter *modp, unsigned symnr)
    {
        if (_symMap.count(symnr) != 0)
            modp->ctrl()->errHandler().internalError(modp->ctrl()->printBuffer("double initialization for predefined subsymbol symnr = %d", symnr));

        SubElem *sub = new SubElem(0, 0, 0, false);
        sub->symVal.init(modp->symbolInfo(symnr), true, false);
        _symMap[symnr] = sub;

        return &(sub->symVal);
    }


    /**
     * create a new container as a copy of this
     * @param ctx       execution context
     * @param res		store for result value
     * @param ref       ref assignment for all elements in the container
     * @return          new container
     */
    void ValContainer::copy(ExecContext *ctx, CmplVal& res, bool ref)
    {
        res.set(TP_CONTAINER, new ValContainer());

        ValContainer *c = res.valContainer();
        c->_finalized = _finalized;

        LockGuard<mutex> lckC(ctx->needLock(), _accMtx);

        for (map<unsigned, SubElem*>::iterator it1 = _symMap.begin(); it1 != _symMap.end(); it1++) {
            c->_symMap[it1->first] = it1->second->copy(ref);
        }
    }

}

