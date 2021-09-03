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


#include "ThreadHandler.hh"


namespace cmpl
{

	/* **************** ThreadHandler ************* */

	/**
	 * increases count of running worker threads. if the max count of threads already reached, then wait until another thread is ended.
     * @param cf            pointer to flag for cancel waiting / NULL: no such flag
     * @return              false if canceled by <code>cf</code>
	 */
    bool ThreadHandler::waitIncThread(volatile bool *cf)
	{
		if (!hasThreading())
            return true;

		unique_lock<mutex> lck(_accMtx);
		while (true) {
			if (_curThreads < _maxThreads) {
				_curThreads++;
                return true;
			}

            if (cf && *cf)
                break;

            _waitCv.wait_for(lck, chrono::milliseconds(10));
		}

        return false;
	}

	/**
	 * decreases count of running worker threads
	 */
	void ThreadHandler::decThread()
	{
		if (!hasThreading())
			return;

		unique_lock<mutex> lck(_accMtx);
		if (_curThreads > 0)
			_curThreads--;

		_waitCv.notify_one();
	}



    /* **************** global mutexes ************* */

    recursive_mutex LockGlobalGuard::_coutLockMtx;
    recursive_mutex LockGlobalGuard::_errLockMtx;



#if 0
	/* **************** AccessLock ************* */

	/**
	 * test if lock is available and set it
	 * @param excl		exclusive or non-exclusive lock
	 * @return			true: lock is set / false: lock is not available
	 */
	bool AccessLock::testSetLock(bool excl)
	{
		lock_guard<mutex> lck(_accMtx);

		if (canLock(excl)) {
			setLock(excl);
			return true;
		}

		return false;
	}

	/**
	 * wait for the lock and set it
	 * @param excl		exclusive or non-exclusive lock
	 */
	void AccessLock::waitSetLock(bool excl)
	{
		unique_lock<mutex> lck(_accMtx);

		while (true) {
			if (canLock(excl)) {
				setLock(excl);
				return;
			}

            _waitCv.wait_for(lck, chrono::milliseconds(20));
		}
	}

	/**
	 * release lock
	 */
	void AccessLock::releaseLock()
	{
		unique_lock<mutex> lck(_accMtx);
		unsetLock();
		_waitCv.notify_one();
	}
#endif //0
}

