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

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Lockable::~Lockable()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::Lockable::lockAll()
{
  cedar::aux::lock(this->mLocks);
}

void cedar::aux::Lockable::unlockAll()
{
  cedar::aux::unlock(this->mLocks);
}

void cedar::aux::Lockable::addLock(QReadWriteLock* pLock, cedar::aux::LOCK_TYPE lockType)
{
  cedar::aux::append(this->mLocks, pLock, lockType);
}

void cedar::aux::Lockable::removeLock(QReadWriteLock* pLock, cedar::aux::LOCK_TYPE lockType)
{
  cedar::aux::LockSet::iterator iter = this->mLocks.find(std::make_pair(pLock, lockType));

  CEDAR_DEBUG_ASSERT(iter != this->mLocks.end());
  this->mLocks.erase(iter);
}