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

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Lockable.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QWriteLocker>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Lockable::Lockable()
{
  this->mLockSets.push_back(Locks());
  this->mLockSetHandles["all"] = 0;
}

cedar::aux::Lockable::~Lockable()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const cedar::aux::Lockable::Locks& cedar::aux::Lockable::getLocks(LockSetHandle lockSet) const
{
  CEDAR_ASSERT(lockSet < this->mLockSets.size());
  return this->mLockSets[lockSet];
}

size_t cedar::aux::Lockable::getLockCount() const
{
  return this->mLockSets[this->getLockSetHandle("all")].size();
}

//!@brief Defines a lock set.
cedar::aux::Lockable::LockSetHandle cedar::aux::Lockable::defineLockSet(const std::string& lockSet)
{
  QWriteLocker locker(&mLocksLock);

  cedar::aux::Lockable::LockSetHandle new_handle = this->mLockSets.size();
  this->mLockSets.push_back(Locks());
  this->mLockSetHandles[lockSet] = new_handle;

  return new_handle;
}

//!@brief Retrieves the handle for a given lock set name.
cedar::aux::Lockable::LockSetHandle cedar::aux::Lockable::getLockSetHandle(const std::string& lockSet) const
{
  QReadLocker locker(&mLocksLock);

  std::map<std::string, unsigned int>::const_iterator iter = this->mLockSetHandles.find(lockSet);
  if (iter == this->mLockSetHandles.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "Cannot find a lock set by the name \"" + lockSet + "\".");
  }

  return iter->second;
}

void cedar::aux::Lockable::lockAll(cedar::aux::LOCK_TYPE lockType, LockSetHandle lockSet) const
{
  QReadLocker locker(&this->mLocksLock);

  CEDAR_ASSERT(lockSet < this->mLockSets.size());

  QReadWriteLock* p_last = NULL;
  for (Locks::const_iterator iter = this->mLockSets[lockSet].begin(); iter != this->mLockSets[lockSet].end(); ++iter)
  {
    QReadWriteLock* p_lock = iter->first;

    if (p_lock != p_last)
    {
      p_last = p_lock;
      cedar::aux::Lockable::applyLockType(p_lock, lockType);
    }
  }
}

void cedar::aux::Lockable::lockAll(LockSetHandle lockSet) const
{
  QReadLocker locker(&this->mLocksLock);

  CEDAR_ASSERT(lockSet < this->mLockSets.size());

  QReadWriteLock* p_last = NULL;
  for (Locks::const_iterator iter = this->mLockSets[lockSet].begin(); iter != this->mLockSets[lockSet].end(); ++iter)
  {
    QReadWriteLock* p_lock = iter->first;

    if (p_lock != p_last)
    {
      p_last = p_lock;
      this->applyLockType(p_lock, iter->second);
    }
  }
}

void cedar::aux::Lockable::unlockAll(LockSetHandle lockSet) const
{
  QReadLocker locker(&this->mLocksLock);

  CEDAR_ASSERT(lockSet < this->mLockSets.size());

  QReadWriteLock* p_last = NULL;
  for (Locks::const_iterator iter = this->mLockSets[lockSet].begin(); iter != this->mLockSets[lockSet].end(); ++iter)
  {
    QReadWriteLock* p_lock = iter->first;

    if (p_lock != p_last && iter->second != cedar::aux::LOCK_TYPE_DONT_LOCK)
    {
      p_last = p_lock;
      p_lock->unlock();
    }
  }
}

void cedar::aux::Lockable::addLock(QReadWriteLock* pLock, cedar::aux::LOCK_TYPE lockType, LockSetHandle lockSet)
{
  QWriteLocker locker(&this->mLocksLock);

  CEDAR_ASSERT(lockSet < this->mLockSets.size());

  this->mLockSets[lockSet].insert(std::make_pair(pLock, lockType));

  if (lockSet != 0)
  {
    locker.unlock();
    this->addLock(pLock, lockType, 0);
  }
}

void cedar::aux::Lockable::removeLock(QReadWriteLock* pLock, cedar::aux::LOCK_TYPE lockType, LockSetHandle lockSet)
{
  QWriteLocker locker(&this->mLocksLock);

  CEDAR_ASSERT(lockSet < this->mLockSets.size());

  Locks& lock_set = this->mLockSets[lockSet];

  Locks::iterator iter = lock_set.find(std::make_pair(pLock, lockType));
  if(iter == lock_set.end())
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "The given data object was not found in this lockable.");
  }
  lock_set.erase(iter);

  // remove the automatically added locks from the "all" set.
  if (lockSet != 0)
  {
    locker.unlock();
    this->removeLock(pLock, lockType, 0);
  }
}
