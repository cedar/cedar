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
#include "cedar/auxiliaries/LockType.h"
#include "cedar/auxiliaries/LockerBase.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Lockable.fwd.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <set>
#include <map>
#include <utility>
#include <vector>

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

public:
  /*! @brief A RAII-based locker for lockables. Will automatically unlock when the locker is destroyed.
   *
   * @see cedar::aux::LockerBase
   */
  class Locker : public cedar::aux::LockerBase
  {
    public:
      //! Constructor. Takes a lockable and the lock set to be locked.
      Locker(LockablePtr lockable, LockSetHandle lockSet = 0)
      :
      cedar::aux::LockerBase
      (
        boost::bind(&cedar::aux::Lockable::lockAll, lockable, lockSet),
        boost::bind(&cedar::aux::Lockable::unlockAll, lockable, lockSet)
      )
      {
      }

      //! Constructor. Takes a lockable and the lock set to be locked.
      Locker(Lockable* lockable, LockSetHandle lockSet = 0)
      :
      cedar::aux::LockerBase
      (
        boost::bind(&cedar::aux::Lockable::lockAll, lockable, lockSet),
        boost::bind(&cedar::aux::Lockable::unlockAll, lockable, lockSet)
      )
      {
      }

      //! Constructor. Takes a lockable and the lock set to be locked, as well as the type of locking to be done.
      Locker(Lockable* lockable, cedar::aux::LOCK_TYPE lockType, LockSetHandle lockSet = 0)
      :
      cedar::aux::LockerBase
      (
        boost::bind(&cedar::aux::Lockable::lockAll, lockable, lockType, lockSet),
        boost::bind(&cedar::aux::Lockable::unlockAll, lockable, lockSet)
      )
      {
      }

      //! Constructor. Takes a lockable and the lock set to be locked, as well as the type of locking to be done.
      Locker(LockablePtr lockable, cedar::aux::LOCK_TYPE lockType, LockSetHandle lockSet = 0)
      :
      cedar::aux::LockerBase
      (
        boost::bind(&cedar::aux::Lockable::lockAll, lockable, lockType, lockSet),
        boost::bind(&cedar::aux::Lockable::unlockAll, lockable, lockSet)
      )
      {
      }
  };

  CEDAR_GENERATE_POINTER_TYPES(Locker);

public:
  /*! An RAII-based read-locker for cedar::aux::Lockables.
   *
   * @see cedar::aux::LockerBase
   */
  class ReadLocker : public Locker
  {
  public:
    //! Constructor.
    ReadLocker(LockablePtr lockable, LockSetHandle lockSet = 0)
    :
    Locker(lockable, cedar::aux::LOCK_TYPE_READ, lockSet)
    {
    }

    //! Constructor.
    ReadLocker(Lockable* lockable, LockSetHandle lockSet = 0)
    :
    Locker(lockable, cedar::aux::LOCK_TYPE_READ, lockSet)
    {
    }
  };

  CEDAR_GENERATE_POINTER_TYPES(ReadLocker);

  /*! An RAII-based write-locker for cedar::aux::Lockables.
   *
   * @see cedar::aux::LockerBase
   */
  class WriteLocker : public Locker
  {
  public:
    //! Constructor.
    WriteLocker(LockablePtr lockable, LockSetHandle lockSet = 0)
    :
    Locker(lockable, cedar::aux::LOCK_TYPE_WRITE, lockSet)
    {
    }

    //! Constructor.
    WriteLocker(Lockable* lockable, LockSetHandle lockSet = 0)
    :
    Locker(lockable, cedar::aux::LOCK_TYPE_WRITE, lockSet)
    {
    }
  };

  CEDAR_GENERATE_POINTER_TYPES(WriteLocker);

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

  //!@brief Locks all locks associated with this object with the given lock type (reading, writing).
  void lockAll(cedar::aux::LOCK_TYPE lockType, LockSetHandle lockSet) const;

  //!@brief Unlocks all locks associated with this object.
  void unlockAll(LockSetHandle lockSet = 0) const;

  //!@brief Returns the number of locks.
  size_t getLockCount() const;

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

  //! Returns the set of locks stored for the given lock set handle.
  const Locks& getLocks(LockSetHandle lockSet = 0) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  inline static void applyLockType(QReadWriteLock* pLock, cedar::aux::LOCK_TYPE lockType)
  {
    switch (lockType)
    {
      case cedar::aux::LOCK_TYPE_READ:
        pLock->lockForRead();
        break;

      case cedar::aux::LOCK_TYPE_WRITE:
        pLock->lockForWrite();
        break;

      case cedar::aux::LOCK_TYPE_DONT_LOCK:
        // do nothing
        break;
    }
  }

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

