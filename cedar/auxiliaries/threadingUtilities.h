/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        cedar/auxiliaries/threadingUtilities.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_THREADING_UTILITIES_H
#define CEDAR_AUX_THREADING_UTILITIES_H

#include <set>
#include <QReadWriteLock>

namespace cedar
{
  namespace aux
  {
    enum LOCK_TYPE
    {
      //! The lock should be locked for reading
      LOCK_TYPE_READ,
      //! The lock should be locked for writing
      LOCK_TYPE_WRITE
    };

    typedef std::set<std::pair<QReadWriteLock*, LOCK_TYPE> > LockSet;

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
      for (LockSet::iterator iter = locks.begin(); iter != locks.end(); ++iter)
      {
        // switch based on the lock type
        switch (iter->second)
        {
          case cedar::aux::LOCK_TYPE_READ:
            iter->first->lockForRead();
            break;

          case cedar::aux::LOCK_TYPE_WRITE:
            iter->first->lockForWrite();
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
      for (LockSet::iterator iter = locks.begin(); iter != locks.end(); ++iter)
      {
        iter->first->unlock();
      }
    }
  }
}


#endif // CEDAR_AUX_THREADING_UTILITIES_H
