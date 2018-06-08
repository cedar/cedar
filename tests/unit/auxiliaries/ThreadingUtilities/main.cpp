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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 12 01

    Description: 

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/threadingUtilities.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <iostream>

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  cedar::aux::LockSet locks;
  cedar::aux::append(locks, new QReadWriteLock(), cedar::aux::LOCK_TYPE_READ);
  cedar::aux::append(locks, new QReadWriteLock(), cedar::aux::LOCK_TYPE_WRITE);
  cedar::aux::append(locks, new QReadWriteLock(), cedar::aux::LOCK_TYPE_READ);
  cedar::aux::append(locks, new QReadWriteLock(), cedar::aux::LOCK_TYPE_READ);
  cedar::aux::append(locks, new QReadWriteLock(), cedar::aux::LOCK_TYPE_WRITE);
  cedar::aux::append(locks, new QReadWriteLock(), cedar::aux::LOCK_TYPE_WRITE);
  cedar::aux::append(locks, new QReadWriteLock(), cedar::aux::LOCK_TYPE_READ);
  cedar::aux::append(locks, new QReadWriteLock(), cedar::aux::LOCK_TYPE_READ);
  cedar::aux::append(locks, new QReadWriteLock(), cedar::aux::LOCK_TYPE_WRITE);
  cedar::aux::append(locks, new QReadWriteLock(), cedar::aux::LOCK_TYPE_READ);

  cedar::aux::lock(locks);

  for (cedar::aux::LockSet::iterator iter = locks.begin(); iter != locks.end(); ++iter)
  {
    if (iter->first->tryLockForWrite())
    {
      // if the attempt to lock succeeds, something must have gone wrong.
      std::cout << "Error: Lock " << iter->first << " could be locked even though it should not be possible."
          << std::endl;
      iter->first->unlock();
      ++errors;
    }
  }

  cedar::aux::unlock(locks);

  for (cedar::aux::LockSet::iterator iter = locks.begin(); iter != locks.end(); ++iter)
  {
    if (!iter->first->tryLockForWrite())
    {
      // if the attempt to lock succeeds, something must have gone wrong.
      std::cout << "Error: Lock " << iter->first << " could not be locked even though it should be possible."
          << std::endl;
      ++errors;
    }
    else
    {
      iter->first->unlock();
    }
  }

  std::cout << "Done. There were " << errors << " errors." << std::endl;

  // cleanup
  for (cedar::aux::LockSet::iterator iter = locks.begin(); iter != locks.end(); ++iter)
  {
    delete iter->first;
  }

  return errors;
}
