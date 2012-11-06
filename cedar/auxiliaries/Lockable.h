/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Lockable.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 06 08

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOCKABLE_H
#define CEDAR_AUX_LOCKABLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/threadingUtilities.h"
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <set>

/*!@brief Base class for strutures that can be locked in their entierety.
 *
 *        Locking is done in a special order that prevents deadlocks, therefore you should always use this function to
 *        lock the Connectable's data.
 *
 *        This is intended as the base for classes that have multiple locks that must be locked in certain cases.
 *
 * @see   cedar::aux::lock for a description on the deadlock-free locking mechanism.
 */
class cedar::aux::Lockable
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! A handle to a lock set.
  typedef unsigned int LockSetHandle;

  //! Storage for locks in this class.
  typedef std::multiset<std::pair<QReadWriteLock*, cedar::aux::LOCK_TYPE> > Locks;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  Lockable();

  //!@brief Destructor
  virtual ~Lockable();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Locks all locks associated with this object.
  void lockAll(LockSetHandle lockSet = 0) const;

  //!@brief Unlocks all locks associated with this object.
  void unlockAll(LockSetHandle lockSet = 0) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Add a lock to the set of locks of the lockable.
  void addLock(QReadWriteLock* pLock, cedar::aux::LOCK_TYPE lockType, LockSetHandle lockSet = 0);

  //!@brief Removes the given lock from the lock set.
  void removeLock(QReadWriteLock* pLock, cedar::aux::LOCK_TYPE lockType, LockSetHandle lockSet = 0);

  //!@brief Defines a lock set.
  LockSetHandle defineLockSet(const std::string& lockSet);

  //!@brief Retrieves the handle for a given lock set name.
  LockSetHandle getLockSetHandle(const std::string& lockSet) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Lock used for locking the lock set.
  mutable QReadWriteLock mLocksLock;

  //! Association between handles and their names.
  std::map<std::string, unsigned int> mLockSetHandles;

  //! Storage of the lock sets. mLockSets[0] contains all locks.
  mutable std::vector<Locks> mLockSets;

}; // class cedar::aux::Lockable

#endif // CEDAR_AUX_LOCKABLE_H

