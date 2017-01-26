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

    File:        Lockable.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 09 21

    Description: Implements all unit tests for the @em cedar::aux::Lockable class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/defines.h"
#include "cedar/auxiliaries/Lockable.h"
#include "cedar/auxiliaries/DataTemplate.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//!@todo Check that the locks of the correct type are acquired, i.e., that readlocks are locked for reading and not writing etc.
//!@todo Check that lock set names cannot be duplicated/overridden

typedef cedar::aux::DataTemplate<double> DoubleData;
CEDAR_GENERATE_POINTER_TYPES(DoubleData);

class LockableTest : public cedar::aux::Lockable
{
  public:
    LockableTest()
    :
    mData1(new DoubleData()),
    mData2(new DoubleData()),
    mData3(new DoubleData())
    {
      std::cout << "Defining lock sets." << std::endl;
      mData1Handle = this->defineLockSet("data1");
      mData2Handle = this->defineLockSet("data2");
      mData3Handle = this->defineLockSet("data3");

      std::cout << "Adding data to lock sets." << std::endl;
      this->addLock(&mData1->getLock(), cedar::aux::LOCK_TYPE_READ, mData1Handle);
      this->addLock(&mData2->getLock(), cedar::aux::LOCK_TYPE_WRITE, mData2Handle);
      this->addLock(&mData3->getLock(), cedar::aux::LOCK_TYPE_WRITE, mData3Handle);

      std::cout << "Object constructed." << std::endl;
    }

    int testLockSetNameRetrieval()
    {
      int errors = 0;

      std::cout << "Testing lock set retrieval by name ..." << std::endl;
      if (mData1Handle != this->getLockSetHandle("data1"))
      {
        std::cout << "ERROR: handle for data1 was different." << std::endl;
        ++errors;
      }
      if (mData2Handle != this->getLockSetHandle("data2"))
      {
        std::cout << "ERROR: handle for data2 was different." << std::endl;
        ++errors;
      }
      if (mData3Handle != this->getLockSetHandle("data3"))
      {
        std::cout << "ERROR: handle for data3 was different." << std::endl;
        ++errors;
      }

      std::cout << "Lock set name retrieval test revealed " << errors << " error(s)." << std::endl;
      return errors;
    }

    int testLockGroups()
    {
      int errors = 0;
      std::cout << "Testing lock groups ... " << std::endl;

      std::cout << " Locking all data." << std::endl;
      this->lockAll();
      errors += testLocks(true, true, true);

      std::cout << " Unlocking all data." << std::endl;
      this->unlockAll();
      errors += testLocks(false, false, false);

      std::cout << " Locking all data in group 1." << std::endl;
      this->lockAll(mData1Handle);
      errors += testLocks(true, false, false);

      std::cout << " Unlocking all data in group 1." << std::endl;
      this->unlockAll(mData1Handle);
      errors += testLocks(false, false, false);

      std::cout << " Locking all data in group 2." << std::endl;
      this->lockAll(mData2Handle);
      errors += testLocks(false, true, false);

      std::cout << " Unlocking all data in group 2." << std::endl;
      this->unlockAll(mData2Handle);
      errors += testLocks(false, false, false);

      std::cout << " Locking all data in group 3." << std::endl;
      this->lockAll(mData3Handle);
      errors += testLocks(false, false, true);

      std::cout << " Unlocking all data in group 3." << std::endl;
      this->unlockAll(mData3Handle);
      errors += testLocks(false, false, false);

      std::cout << "Lock group testing revealed " << errors << " error(s)." << std::endl;
      return errors;
    }

    int testLocks(bool data1locked, bool data2locked, bool data3locked)
    {
      int errors = 0;
      errors += checkLockedness(mData1, data1locked, "data1");
      errors += checkLockedness(mData2, data2locked, "data2");
      errors += checkLockedness(mData3, data3locked, "data3");
      return errors;
    }

    int testMultiLock()
    {
      int errors = 0;
      std::cout << "Testing multiple locks ..." << std::endl;

      std::cout << " Adding data2 lock to group 1" << std::endl;
      this->addLock(&mData2->getLock(), cedar::aux::LOCK_TYPE_READ, mData1Handle);

      std::cout << " Locking all data." << std::endl;
      this->lockAll();
      errors += testLocks(true, true, true);

      std::cout << " Unlocking all data." << std::endl;
      this->unlockAll();
      errors += testLocks(false, false, false);

      std::cout << " Locking all data in group 1." << std::endl;
      this->lockAll(mData1Handle);
      errors += testLocks(true, true, false);

      std::cout << " Unlocking all data in group 1." << std::endl;
      this->unlockAll(mData1Handle);
      errors += testLocks(false, false, false);

      std::cout << " Removing data2 from group 1." << std::endl;
      this->removeLock(&mData2->getLock(), cedar::aux::LOCK_TYPE_READ, mData1Handle);

      // everything should now be back to normal
      errors += this->testLockGroups();

      std::cout << " Removing data2 from group 2." << std::endl;
      this->removeLock(&mData2->getLock(), cedar::aux::LOCK_TYPE_WRITE, mData2Handle);

      std::cout << " Locking all data." << std::endl;
      this->lockAll();
      errors += testLocks(true, false, true);

      std::cout << " Unlocking all data." << std::endl;
      this->unlockAll();
      errors += testLocks(false, false, false);

      std::cout << " Adding data2 to group 2 twice." << std::endl;
      this->addLock(&mData2->getLock(), cedar::aux::LOCK_TYPE_WRITE, mData2Handle);
      this->addLock(&mData2->getLock(), cedar::aux::LOCK_TYPE_WRITE, mData2Handle);

      // everything should now be same as normal
      errors += this->testLockGroups();

      std::cout << " Removing data2 from group 2 once." << std::endl;
      this->removeLock(&mData2->getLock(), cedar::aux::LOCK_TYPE_WRITE, mData2Handle);

      // everything should still be same as normal
      errors += this->testLockGroups();

      std::cout << " Removing data2 from group 2 for the second time." << std::endl;
      this->removeLock(&mData2->getLock(), cedar::aux::LOCK_TYPE_WRITE, mData2Handle);

      std::cout << " Locking all data." << std::endl;
      this->lockAll();
      errors += testLocks(true, false, true);

      std::cout << " Unlocking all data." << std::endl;
      this->unlockAll();
      errors += testLocks(false, false, false);

      std::cout << "Multilocking revealed " << errors << " error(s)." << std::endl;
      return errors;
    }

    int checkLockedness(cedar::aux::DataPtr pData, bool shouldBeLocked, const std::string& name)
    {
      bool is_locked = this->isLocked(pData->getLock());
      if ((is_locked && shouldBeLocked) || (!is_locked && !shouldBeLocked))
      {
        return 0;
      }
      else
      {
        if (shouldBeLocked)
        {
          std::cout << "  ERROR: lock \"" << name << "\" is not locked even though it should be." << std::endl;
        }
        else
        {
          std::cout << "  ERROR: lock \"" << name << "\" is locked even though it should not be." << std::endl;
        }

        return 1;
      }
    }

    bool isLocked(QReadWriteLock& lock)
    {
       if (lock.tryLockForWrite(5))
       {
         lock.unlock();
         return false;
       }
       else
       {
         return true;
       }
    }

    int testAll()
    {
      int errors = 0;
      errors += testLockSetNameRetrieval();
      errors += testLockGroups();
      errors += testMultiLock();
      return errors;
    }

  private:
    DoubleDataPtr mData1;
    DoubleDataPtr mData2;
    DoubleDataPtr mData3;

    LockSetHandle mData1Handle;
    LockSetHandle mData2Handle;
    LockSetHandle mData3Handle;
};

int main()
{
  int errors = 0;

  LockableTest lockable;
  errors += lockable.testAll();

  std::cout << "Test finished with " << errors << " error(s)." << std::endl;
  return errors;
}
