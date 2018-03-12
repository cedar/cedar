/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        threadingUtilities.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_THREADING_UTILITIES_H
#define CEDAR_AUX_THREADING_UTILITIES_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/LockType.h"
#include "cedar/auxiliaries/LockerBase.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <utility>
#include <set>

namespace cedar
{
  namespace aux
  {
    //!@brief A set of pairs of QReadWriteLocks and their corresponding lock role from cedar::aux::LOCK_TYPE.
    typedef std::set<std::pair<QReadWriteLock*, LOCK_TYPE> > LockSet;

    //!@brief Appends a new lock to the lock set.
    inline void append(LockSet& lockSet, QReadWriteLock* pLock, LOCK_TYPE lockType)
    {
      lockSet.insert(std::make_pair(pLock, lockType));
    }

    /*! @brief Locks the given locks in a deadlock-free manner.
     *
     *         This method locks all the locks passed in a specific manner, such that, if no other locks are acquired
     *         via other methods between this lock call and the unlock call of the same set, no deadlocks can occur.
     *
     *         This works by always locking the locks in a canonical order. Consider this example: Assume there are two
     *         threads t1 and t2 that both want to acquire locks l1 and l2. This will lead to a deadlock if t1 acquires
     *         l1 first, while t2 acquires l2 at the same time. In this case, t1 will wait for l2, while t2 will wait
     *         for l1 till infinity.
     *
     *         If this lock-function is used, it will always lock in the order l1, l2. Thus, both threads will attempt
     *         to acquire l1 first, and one of the threads will win. This principle can be extended to arbitrary sets
     *         of locks.
     *
     *         The important thing when using this method is to never lock multiple locks in any other order, i.e.,
     *         if your thread needs more than one lock, always lock using this method and never lock anything until
     *         all locks you have acquired thus far are completely unlocked.
     *
     *         Here's some example code of how to use this method:
     *
     *         @code
     *         cedar::aux::LockSet locks;
     *         locks.insert(std::make_pair(lock_ptr_1, cedar::aux::LOCK_TYPE_READ));
     *         locks.insert(std::make_pair(lock_ptr_2, cedar::aux::LOCK_TYPE_WRITE));
     *         // ...
     *         cedar::aux::lock(locks);
     *         // do stuff. Don't call any more lock functions here!
     *         cedar::aux::unlock(locks);
     *         // hooray! If there were no more lock functions between the lines above, you should be deadlock free!
     *         @endcode
     */
    inline void lock(LockSet& locks)
    {
      // The implicit ordering of the set is used to impose the canonical lock order here.
      for (const auto& lock_type_pair : locks)
      {
        // switch based on the lock type
        switch (lock_type_pair.second)
        {
          case cedar::aux::LOCK_TYPE_READ:
            lock_type_pair.first->lockForRead();
            break;

          case cedar::aux::LOCK_TYPE_WRITE:
            lock_type_pair.first->lockForWrite();
            break;

          case cedar::aux::LOCK_TYPE_DONT_LOCK:
            // ignored
            break;
        }
      }
    }

    /*! @brief Unlocks a set of locks.
     *
     *  @see   cedar::aux::lock.
     */
    inline void unlock(LockSet& locks)
    {
      // The implicit ordering of the set is used to impose the canonical lock order here.
      for (const auto& lock_type_pair : locks)
      {
        if (lock_type_pair.second != cedar::aux::LOCK_TYPE_DONT_LOCK)
        {
          lock_type_pair.first->unlock();
        }
      }
    }

    /*!@brief Alternative to the cedar::aux::lock(cedar::aux::LockSet&) method.
     */
    inline void lock(const std::set<QReadWriteLock*>& locks, cedar::aux::LOCK_TYPE type)
    {
      if (type == cedar::aux::LOCK_TYPE_DONT_LOCK)
      {
        return;
      }

      for (const auto& lock : locks)
      {
        // switch based on the lock type
        switch (type)
        {
          case cedar::aux::LOCK_TYPE_READ:
            lock->lockForRead();
            break;

          case cedar::aux::LOCK_TYPE_WRITE:
            lock->lockForWrite();
            break;

          case cedar::aux::LOCK_TYPE_DONT_LOCK:
            // ignored
            break;
        }
      }
    }

    /*!@brief Alternative to the cedar::aux::unlock(cedar::aux::LockSet&) method.
     */
    inline void unlock(const std::set<QReadWriteLock*>& locks, cedar::aux::LOCK_TYPE type)
    {
      if (type == cedar::aux::LOCK_TYPE_DONT_LOCK)
      {
        return;
      }

      for (const auto& lock : locks)
      {
        lock->unlock();
      }
    }

    /*! A RAII-based locker for cedar::aux::LockSet.
     * @see cedar::aux::LockerBase.
     */
    class LockSetLocker : public cedar::aux::LockerBase
    {
      public:
        //! Constructor.
		  CEDAR_AUX_LIB_EXPORT LockSetLocker(const cedar::aux::LockSet& lockSet);

        //! Destructor.
		  CEDAR_AUX_LIB_EXPORT ~LockSetLocker();

      private:
        void applyLock();

        void applyUnlock();

      private:
        cedar::aux::LockSet mLockSet;
    };
  }
}

#endif // CEDAR_AUX_THREADING_UTILITIES_H
