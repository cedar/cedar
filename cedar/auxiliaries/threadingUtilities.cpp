/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        threadingUtilities.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 07 29

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/threadingUtilities.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::LockSetLocker::LockSetLocker(const cedar::aux::LockSet& lockSet)
:
cedar::aux::LockerBase
(
  boost::bind(&cedar::aux::LockSetLocker::applyLock, this),
  boost::bind(&cedar::aux::LockSetLocker::applyUnlock, this),
  false // because we need to access mLockSet (which will not be initialized if relock and thus applyLock is
        // called here), we do not let the superclass constructor call relock but rather do it below
),
mLockSet(lockSet)
{
  this->relock();
}

cedar::aux::LockSetLocker::~LockSetLocker()
{
  this->preDestruct();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::LockSetLocker::applyLock()
{
  cedar::aux::lock(this->mLockSet);
}

void cedar::aux::LockSetLocker::applyUnlock()
{
  cedar::aux::unlock(this->mLockSet);
}
