
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


#ifndef VALCONTAINER_HH
#define VALCONTAINER_HH

#include <map>

#include "../../CommonData/CmplVal.hh"
#include "../../CommonData/IntCode.hh"
#include "ValueStore.hh"



using namespace std;



namespace cmpl
{
    class Interpreter;


	/**
     * container class for subsymbol values
	 */
	class ValContainer : public CmplObjBase
	{
    private:
        /**
         * symbol entry for one subsymbol.
         * in rare cases there maybe more than one entry, for different validity ranges.
         */
        struct SubElem {
            SymbolValue symVal;                     ///< symbol entry

            unsigned validFrom;                     ///< validity start position in the intermediary code of the symbol entry
            unsigned validTo;                       ///< validity to position in the intermediary code of the symbol entry
            unsigned defPos;                        ///< position within intermediary code of the definition of the subsymbol / 0:predefined
            bool defNew;                            ///< definition of the subsymbol with "new" modificator

            SubElem *next;                          ///< pointer to next SubElem (with other validity range: next->validFrom < validFrom) / NULL: no next SubElem

            /**
             * constructor
             */
            SubElem(unsigned vf, unsigned vt, unsigned dp, bool dn): validFrom(vf), validTo(vt), defPos(dp), defNew(dn), next(NULL)  { }

            /**
             * destructor
             */
            ~SubElem()                              { symVal.unset(); if (next) delete next; }

            /**
             * check if symbol entry is accessible on a position within intermediary code
             */
            inline bool checkPos(unsigned pos) const  { return (pos >= validFrom && (validTo == 0 || pos <= validTo)); }

            /**
             * get syntax element id of definition of the subsymbol / 0: predefined
             */
            inline unsigned syntaxElem(IntCode::IcElem *cb) const  { return (defPos ? cb[defPos].se : 0); }

            /**
             * create new SubElem as copy of this
             */
            SubElem *copy(bool ref)                 { SubElem *s = new SubElem(validFrom, validTo, defPos, defNew); s->symVal.copyAllFrom(&symVal, ref); if (next) { s->next = next->copy(ref); } return s; }
        };

    private:
        map<unsigned, SubElem*> _symMap;            ///< symbol map; key is the subsymbol number
        bool _finalized;                            ///< container is finalized, i.e. no subsymbols cannot be added anymore
        mutex _accMtx;                              ///< mutex for access lock

    public:
        /**
         * constructor
         */
        ValContainer(bool fin = false): _finalized(fin)     { }

        /**
         * destructor
         */
        ~ValContainer();

        /**
         * get mutex for access lock
         */
        inline mutex& accMtx()                              { return _accMtx; }

    public:

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override		{ ostr << "<container (" << _symMap.size() << " entries)>"; }

        /**
         * set container as finalized
         */
        void setFinalized()                                 { _finalized = true; }

        /**
         * get symbol value for subsymbol fetch, insert new symbol value if necessary
         * @param ctx       execution context
         * @param cd        intermediary code command for subsymbol fetch
         * @return          symbol value pointer
         */
        SymbolValue *getInsertSymbol(ExecContext *ctx, IntCode::IcElem *cd);

        /**
         * get symbol value for subsymbol
         * @param symnr     symbol number
         * @param pos       position in intermediary code / NULL: don't pay attention to validity range
         * @return          symbol value pointer / NULL: not found
         */
        SymbolValue *getSimple(unsigned symnr, unsigned pos = 0);

        /**
         * insert new symbol value object for predefined subsymbol
         * @param modp      interpreter module
         * @param symnr     symbol number
         * @return          symbol value pointer
         */
        SymbolValue *insertPredef(Interpreter *modp, unsigned symnr);

        /**
         * create a new container as a copy of this
         * @param ctx       execution context
         * @param res		store for result value
         * @param ref       ref assignment for all elements in the container
         */
        void copy(ExecContext *ctx, CmplVal &res, bool ref);
	};

}

#endif // VALCONTAINER_HH

