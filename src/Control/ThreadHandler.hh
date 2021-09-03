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


#ifndef THREADHANDLER_HH
#define THREADHANDLER_HH

#include <mutex>
#include <condition_variable>
#include <chrono>

#include "ModuleBase.hh"


using namespace std;


namespace cmpl
{
	/**
	 * class for limiting the number of concurrently running threads.
	 * utility class for using in modules.
	 */
	class ThreadHandler
	{
	private:
		unsigned int _maxThreads;			///< max number of worker threads (0: no threading)
        volatile unsigned int _curThreads;  ///< number of currently started worker threads
		
		mutex _accMtx;						///< mutex for synchronizing access to this objekt
		condition_variable _waitCv;			///< condition variable for waiting that a thread has ended

    public:
        /**
         * constructor
         */
        inline ThreadHandler(): _maxThreads(0), _curThreads(0)      { }

		/**
		 * get whether threading is used
		 */
		inline bool hasThreading() const		{ return (_maxThreads > 0); }

		/**
		 * get whether locks are needed for accessing values
		 */
		inline bool needLocks() const			{ return (_maxThreads > 1); }

        /**
         * get max number of worker threads (0: no threading)
         */
        inline unsigned maxThreads() const      { return _maxThreads; }

        /**
         * set max number of worker threads
         * @param n     max number of worker threads (0: no threading)
         */
        inline void setMaxThreads(unsigned n)   { lock_guard<mutex> lck(_accMtx); _maxThreads = n; _curThreads = (n ? 1 : 0); }

		/**
		 * increases count of running worker threads. if the max count of threads already reached, then wait until another thread is ended.
         * @param cf            pointer to flag for cancel waiting / NULL: no such flag
         * @return              false if canceled by <code>cf</code>
         */
        bool waitIncThread(volatile bool *cf = NULL);

		/**
		 * decreases count of running worker threads
		 */
		void decThread();
	};


    /**
     * class like std::lock_guard, but with an additional condition whether really use lock
     */
    template<typename mtxT> class LockGuard
    {
    private:
        mtxT *_mtx;

    public:
        /**
         * constructor
         * @param b         condition if mutex should be locked
         * @param mtx		mutex for locking
         */
        inline LockGuard(bool b, mtxT *mtx): _mtx(b ? mtx : NULL)       { if (_mtx) _mtx->lock(); }

        /**
         * constructor
         * @param b         condition if mutex should be locked
         * @param mtx		mutex for locking
         */
        inline LockGuard(bool b, mtxT& mtx): _mtx(b ? &mtx : NULL)      { if (_mtx) _mtx->lock(); }

        /**
         * destructor
         */
        inline ~LockGuard()                                             { if (_mtx) _mtx->unlock(); }


        LockGuard(const LockGuard&) = delete;	// no copy
        LockGuard(LockGuard&&) = delete;		// no move
    };


    /**
     * class for global lock
     */
    class LockGlobalGuard
    {
    public:
        /**
         * type of global locks
         */
        enum lckType {
            coutLock,                       ///< lock for output to streams cout and cerr
            errLock,                        ///< lock for error handling
                // add other types here
        };

    private:
        recursive_mutex* _mtx;                  ///< used mutex for locking

        static recursive_mutex _coutLockMtx;    ///< global mutex for <code>coutLock</code>
        static recursive_mutex _errLockMtx;     ///< global mutex for <code>errLock</code>
                // add mutexes for other types here

        /**
         * get mutex for type
         * @param tp        type of global lock
         * @return          mutex for the lock type
         */
        inline static recursive_mutex *mutexForType(lckType tp) {
            switch (tp) {
                case coutLock: return &_coutLockMtx;
                case errLock: return &_errLockMtx;
                    // add other types here
                default: return NULL;
            }
        }

    public:
        /**
         * constructor
         * @param b         condition if mutex should be locked
         * @param mtx		type of global lock
         */
        inline LockGlobalGuard(bool b, lckType tp): _mtx(NULL)      { if (b) { _mtx = mutexForType(tp); _mtx->lock(); } }

        /**
         * destructor
         */
        inline ~LockGlobalGuard()                                   { if (_mtx) _mtx->unlock(); }


        LockGlobalGuard(const LockGlobalGuard&) = delete;	// no copy
        LockGlobalGuard(LockGlobalGuard&&) = delete;		// no move
    };


#if 0
	/**
	 * class for read lock (non-exclusive) and write lock (exclusive).
	 * utility class for using in modules.
	 * to use for value access when a module runs in more than one thread.
	 */
	class AccessLock
	{
	private:
		int _lockLvl;						///< 0: no lock / -1: exclusive lock / >0: count of non-exclusive locks

		mutex _accMtx;						///< mutex for synchronizing access to this objekt
		condition_variable _waitCv;			///< condition variable for waiting that a lock is released

		/**
		 * check if lock can be set
		 * @param excl		exclusive or non-exclusive lock
		 * @return 			lock can be set
		 */
		inline bool canLock(bool excl)		{ return (!_lockLvl || (!excl && _lockLvl > 0)); }

		/**
		 * set lock
		 * @param excl		exclusive or non-exclusive lock
		 */
		inline void setLock(bool excl)		{ if (excl) _lockLvl = -1; else _lockLvl++; }

		/**
		 * unset lock
		 */
		inline void unsetLock()				{ if (_lockLvl > 0) _lockLvl--; else _lockLvl = 0; }

	public:
		/**
		 * constructor
		 */
		inline AccessLock(): _lockLvl(0)	{ }

		/**
		 * get whether lock is set
		 * @return			0: no lock / -1: exclusive lock / >0: count of non-exclusive locks
		 */
		inline int lockLvl() const			{ return _lockLvl; }


		/**
		 * test if lock is available and set it
		 * @param excl		exclusive or non-exclusive lock
		 * @return			true: lock is set / false: lock is not available
		 */
		bool testSetLock(bool excl);

		/**
		 * wait for the lock and set it
		 * @param excl		exclusive or non-exclusive lock
		 */
		void waitSetLock(bool excl);

		/**
		 * release lock
		 */
		void releaseLock();
	};

	/**
	 * class for handle a lock of class <code>Lock</code>.
	 * with the constructor the lock is set, necessarily waiting for it.
	 * with the destructor the lock is released.
	 */
	class Locking
	{
	private:
		AccessLock *_lck;		///< lock object

	public:
		/**
		 * constructor
		 * @param lck		lock object
		 * @param useLock	really use lock
		 * @param excl		exclusive or non-exclusive lock
		 */
		inline Locking(AccessLock *lck, bool useLock, bool excl)		{ if (lck && useLock) { _lck = lck; _lck->waitSetLock(excl); } else { _lck = NULL; } }

		/**
		 * destructor
		 */
		inline ~Locking()							{ if (_lck) _lck->releaseLock(); }

		Locking(const Locking&) = delete;		// no copy
		Locking(Locking&&) = delete;			// no move
	};

	/**
	 * class for handle a read lock (non-exclusive lock)
	 */
	class ReadLocking : Locking
	{
	public:
		/**
		 * constructor
		 * @param lck		lock object
		 * @param useLock	really use lock
		 */
        inline ReadLocking(AccessLock *lck, bool useLock = true): Locking(lck, useLock, false)      { }

		/**
		 * constructor
		 * @param lck		lock object
		 * @param useLock	really use lock
		 */
        inline ReadLocking(AccessLock& lck, bool useLock = true): Locking(&lck, useLock, false)     { }

        /**
         * constructor
         * @param lck		lock object
         * @param mp        module
         */
        inline ReadLocking(AccessLock *lck, ModuleBase *mp): Locking(lck, mp->needLocks(), false)	{ }

        /**
         * constructor
         * @param lck		lock object
         * @param mp        module
         */
        inline ReadLocking(AccessLock& lck, ModuleBase *mp): Locking(&lck, mp->needLocks(), false)	{ }
    };

	/**
	 * class for handle a write lock (exclusive lock)
	 */
	class WriteLocking : Locking
	{
	public:
		/**
		 * constructor
		 * @param lck		lock object
		 * @param useLock	really use lock
		 */
        inline WriteLocking(AccessLock *lck, bool useLock = true): Locking(lck, useLock, true)      { }

		/**
		 * constructor
		 * @param lck		lock object
		 * @param useLock	really use lock
		 */
        inline WriteLocking(AccessLock& lck, bool useLock = true): Locking(&lck, useLock, true)     { }

        /**
         * constructor
         * @param lck		lock object
         * @param mp        module
         */
        inline WriteLocking(AccessLock *lck, ModuleBase *mp): Locking(lck, mp->needLocks(), true)	{ }

        /**
         * constructor
         * @param lck		lock object
         * @param mp        module
         */
        inline WriteLocking(AccessLock& lck, ModuleBase *mp): Locking(&lck, mp->needLocks(), true)	{ }
    };
#endif //0
}

//#define READ_LOCK(a)    ReadLocking lck_(a, modp())
//#define WRITE_LOCK(a)   WriteLocking lck_(a, modp())

#endif // !THREADHANDLER_HH

