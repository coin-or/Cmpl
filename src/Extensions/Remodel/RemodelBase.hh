
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


#ifndef REMODELBASE_HH
#define REMODELBASE_HH


#include <unordered_map>
#include <condition_variable>
#include <chrono>

#include "../../Control/ExtensionBase.hh"
#include "../../CommonData/ExtensionSteps.hh"
#include "../../Control/ThreadHandler.hh"
#include "../../CommonData/CmplVal.hh"


using namespace std;



namespace cmpl
{
    class Interpreter;
    class OptModel;
    class OptVar;
    class OptCon;
    class RemodelBaseMod;


    /*********** command line options delivered to the extension by run() **********/

    #define OPTION_EXT_REMODEL_ENABLE   20
    #define OPTION_EXT_THREADS          25
    #define OPTION_EXT_REMODEL_BIGM     26
    #define OPTION_EXT_NAMESEP          30
    #define OPTION_EXT_NAMEPREF         31



    /**
     * the <code>RemodelBase</code> class is the base class for
     * extension classes for linearization or other remodeling of constraints and objectives
     */
	class RemodelBase : public ExtensionBase
	{
    protected:
        bool _remodelEnabled;           ///< remodeling with this extension is enabled
        unsigned _maxThreads;           ///< max number of worker threads (0: no threading)
        unsigned _namePref;             ///< prefix string for names of generated constraints or variables / 0: no separator
        unsigned _nameSep;              ///< separator string between original constraint or variable name and postfix / 0: no separator
        realType _bigM;                 ///< huge number used for linearization

        mutex _conMtx;                  ///< mutex for inserting new constraint or objective
        mutex _varMtx;                  ///< mutex for inserting new optimization variable

	public:
        /**
         * constructor
         * @param mod			module creating this extension object
         */
        RemodelBase(RemodelBaseMod *mod);


		/************** overwritten methods of <code>ExtensionBase</code> **********/

	protected:
        /**
         * run the extension function for processing a command line option
         * @param mod			module calling the extension
         * @param step			execution step within the module
         * @param id			additional identificator
         * @param ref           reference number of option registration, should be used for discriminate the options
         * @param prio          priority value of option
         * @param opt           command line option
         * @param par			additional parameter
         * @return              true if option is used by the extension
         */
        bool run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par) override;

        /**
		 * run the extension function
		 * @param mod			module calling the extension
		 * @param step			execution step within the module
		 * @param id			additional identificator
		 * @param par			additional parameter
		 */
        void run(ModuleBase *mod, int step, int id, void *par) override;

		/**
		 * called after running the module, before deleting the module object
		 * @param mod			module calling the extension
		 * @param id			additional identificator
		 */
        void moduleEnd(ModuleBase *mod, int id) override        { }

    public:
        /**
		 * get count of sub data objects
		 */
        int subCnt() override                                   { return 0; }


        /************** remodeling **********/
    protected:
        /**
         * get whether remodeling is enabled
         */
        virtual bool remodelEnabled()                           { return _remodelEnabled; }

        /**
         * initialization before remodeling
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         */
        virtual void remodelInit(Interpreter *modp, OptModel *om)   { }

        /**
         * remodel all constraints and objectives
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param size      count of potential work steps (usually count of matrix rows)
         */
        virtual void remodelAll(Interpreter *modp, OptModel *om, unsigned long size);

        /**
         * remodel all constraints and objectives, for running in parallel worker threads
         * @param rme       remodel extension object
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param rowcnt    row count at start of running this remodel extension
         * @param rc        thread safe row counter
         * @param res       thread safe return of result count
         */
        static void remodelAllThdr(RemodelBase *rme, Interpreter *modp, OptModel *om, unsigned long rowcnt, atomic<unsigned long> *rc, atomic<unsigned long> *res);

        /**
         * return next potential workstep for remodeling (usually pointer to matrix row)
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param rowno     row count of workstep
         * @return          pointer to workstep / NULL: no workstep
         */
        virtual void *getWorkStep(Interpreter *modp, OptModel *om, unsigned long rowno);

        /**
         * execute one potential workstep for remodeling
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param ws        pointer to workstep (already checked for correct type)
         */
        virtual unsigned remodelWorkStep(Interpreter *modp, OptModel *om, void *ws);

        /**
         * remodel one constraint and objective
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        constraint and objective
         * @return          count of executed remodelations
         */
        virtual unsigned remodelCon(Interpreter *modp, OptModel *om, OptCon *oc)        { return 0; }

        /**
         * generate new optimization variable
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param se        syntax element
         * @param dt        data type for optimization variable
         * @param lb        lower bound for optimization variable / NULL: no lower bound
         * @param ub        upper bound for optimization variable / NULL: no lower bound
         * @param nm        name for new optimization variable / empty or NULL: don't set name
         * @param tpl       index tuple for new optimization variable (only used if nm is given)
         * @return          new optimization variable (has counted reference within OptModel)
         */
        OptVar *newOptVar(Interpreter *modp, OptModel *om, unsigned se, CmplVal& dt, CmplVal *lb, CmplVal *ub, string *nm = NULL, CmplVal *tpl = NULL);

        /**
         * generate new optimization constraint
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        original constraint / NULL: no one
         * @param se        syntax element
         * @param f         formula for new constraint
         * @param nm        name for new constraint / empty: don't set name / NULL: replace oc
         * @param tpl       index tuple for new constraint (only used if nm is given)
         * @return          constraint (has counted reference within OptModel)
         */
        OptCon *newOptCon(Interpreter *modp, OptModel *om, OptCon *oc, unsigned se, CmplVal& f, string *nm = NULL, CmplVal *tpl = NULL);

        /**
         * add prefix string to name and store result name in global string store
         * @param modp      intepreter module calling the extension
         * @param nm        name without prefix / NULL: no one
         * @param cur       current number to use if no name
         * @param lc        true: constraint / false: variable
         * @param f         formula for new constraint: if given then add prefix string only if formula is auto generated
         * @return          index number of result name / 0: no one
         */
        unsigned addStorePrefixString(Interpreter *modp, string *nm, unsigned long cur, bool lc, CmplVal *f = NULL);

        /**
         * get base name and tuple of a source variable or constraint
         * @param modp      intepreter module calling the extension
         * @param src       source variable or constraint
         * @param tpl       return of tuple
         * @param nameSep   add name separator if given
         * @param attach    attach postfix string (return empty if no postfix string)
         * @param pfs       postfix string
         * @return          name
         */
        string getBaseNameTuple(Interpreter *modp, OptVarConBase *src, CmplVal& tpl, bool nameSep, bool attach = false, unsigned pfs = 0);

        /**
         * get name and tuple based on two source variables or constraints
         * @param modp      intepreter module calling the extension
         * @param src1      first source variable or constraint
         * @param src2      second source variable or constraint
         * @param tpl       return of tuple
         * @param nameSep   concat names by name separator if given
         * @return          name
         */
        string getBaseNameTuple2(Interpreter *modp, OptVarConBase *src1, OptVarConBase *src2, CmplVal& tpl, bool nameSep = true);

        /**
         * attach postfix string to name
         * @param modp      intepreter module calling the extension
         * @param base      base name
         * @param nameSep   add name separator if given
         * @param pfs       postfix string
         * @return          resulting name / empty if no attach
         */
        string attachName(Interpreter *modp, string& base, bool nameSep, unsigned pfs);

        /**
         * get whether lock are needed
         */
        inline bool needLock()                                  { return (_maxThreads > 1); }

        /**
         * output error message
         * @param modp      intepreter module calling the extension
         * @param ws        workstep (matrix row)
         * @param msg       error message
         * @param errLvl    error level
         */
        virtual void outError(Interpreter *modp, void *ws, const char *msg, int errLvl);
    };


    class RemodelCacheGuard;

    /**
     * Class for a cache of remodelations.
     * (CacheMap::mapped_type must have a conversion to bool, so that the default constructed value converts to false)
     * For all access the class RemodelCacheGuard is used.
     */
    template<class CacheMap>
    class RemodelCache
    {
        friend class RemodelCacheGuard;

    private:
        CacheMap _map;                                  ///< mapping for stored remodelations
        mutex _mtx;                                     ///< mutex for accessing _map
        condition_variable _cv;                         ///< trigger for next checking of _map

    private:
        /**
         * remove a key in the cache, only used for error recovery
         * @param key       key (inserting of this key must be prepared with checkPrepCache() before)
         */
        void release(const typename CacheMap::key_type& key)
        {
            try {
                unique_lock<mutex> lck(_mtx);
                _map.erase(key);

                _cv.notify_all();
            }
            catch (...) {
                // ignore errors because already in error recovery
            }
        }
    };

    /**
     * class for multi threaded access to a RemodelCache
     */
    class RemodelCacheGuard
    {
    private:
        /**
         * base class for info about a guarded cache entry
         */
        class GuardInfoBase
        {
            friend class RemodelCacheGuard;

            virtual ~GuardInfoBase()        { }
            virtual void release()          { }
        };

        /**
         * templated class for info about a guarded cache entry
         */
        template<class CacheMap>
        class GuardInfo: public GuardInfoBase
        {
            friend class RemodelCacheGuard;

            virtual void release()          { _mapp->release(_key); }

        private:
            RemodelCache<CacheMap> *_mapp;
            const typename CacheMap::key_type& _key;

            GuardInfo(RemodelCache<CacheMap> *m, const typename CacheMap::key_type& k): _mapp(m), _key(k)     { }
        };

    private:
        GuardInfoBase *_info;               ///< info about a guarded cache entry

    public:
        /**
         * constructor
         */
        RemodelCacheGuard(): _info(NULL)    { }

        /**
         * destructor, release guarded cache entry if not regulary ended
         */
        ~RemodelCacheGuard()                { if (_info) { _info->release(); delete _info; } }

        /**
         * check cache for a given key, and if not found then prepare inserting of the key
         * @param cache     cache
         * @param key       key to search for
         * @return          found or inserted value (converted to bool: true if found, false if inserted)
         */
        template<class CacheMap> typename CacheMap::mapped_type& checkPrep(RemodelCache<CacheMap>& cache, const typename CacheMap::key_type& key)
        {
            unique_lock<mutex> lck(cache._mtx);

            while (true) {
                // check for key
                bool found = (cache._map.count(key) != 0);
                typename CacheMap::mapped_type& val = cache._map[key];

                if (!found || (bool)val) {
                    if (!found)
                        _info = new GuardInfo<CacheMap>(&cache, key);

                    return val;
                }

                // another thread is working for key, wait until it is ready
                cache._cv.wait_for(lck, chrono::milliseconds(100));
            }
        }

        /**
         * insert value to the cache (key are already known by calling checkPrep())
         * @param cache     cache
         * @param val       value
         */
        template<class CacheMap> void insert(RemodelCache<CacheMap>& cache, typename CacheMap::mapped_type& val)
        {
            if (_info) {
                GuardInfo<CacheMap> *info = dynamic_cast<GuardInfo<CacheMap> *>(_info);
                if (info) {
                    unique_lock<mutex> lck(cache._mtx);

                    cache._map[info->_key] = val;
                    _info = NULL;
                    cache._cv.notify_all();

                    delete info;
                    return;
                }
            }

            throw logic_error("RemodelCacheGuard::checkPrep() must be called before RemodelCacheGuard::insert()");
        }

        /**
         * release key in the cache reserved with checkPrep()
         * @param cache     cache
         * @param err       error message if no reserved key
         */
        template<class CacheMap> void release(RemodelCache<CacheMap>& cache, bool err = true)
        {
            if (_info) {
                GuardInfo<CacheMap> *info = dynamic_cast<GuardInfo<CacheMap> *>(_info);
                if (info) {
                    unique_lock<mutex> lck(cache._mtx);

                    cache._map.erase(info->_key);
                    _info = NULL;
                    cache._cv.notify_all();

                    delete info;
                    return;
                }
            }
            else if (err) {
                throw logic_error("RemodelCacheGuard::checkPrep() must be called before RemodelCacheGuard::release()");
            }
        }

        /**
         * check whether key is set by calling checkPrep()
         * @return
         */
        bool hasKey()
        {
            return (_info != NULL);
        }
    };


    /**
     * type for using a formula value as the key for a remodel cache
     */
    class FormulaCacheKey
    {
        friend class FormulaCacheKeyHash;

    private:
        CmplValAuto _frm;            ///< formula (can only be TP_FORMULA)

    public:
        FormulaCacheKey(CmplVal& v)                             { if (v.t == TP_FORMULA) _frm.copyFrom(v); }
        FormulaCacheKey(ValFormula *f): _frm(TP_FORMULA, (CmplObjBase *)f)      { }

        bool operator == (const FormulaCacheKey& p2) const;
        explicit operator bool() const                          { return (bool)_frm; }
    };

    /**
     * hash for use with FormulaCacheKey
     */
    struct FormulaCacheKeyHash
    {
        size_t operator() (FormulaCacheKey const& p) const noexcept;
    };
}

#endif // REMODELBASE_HH

