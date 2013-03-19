/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        Step.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 07

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <iostream>

class TestStep : public cedar::proc::Step
{
  public:
    TestStep()
    :
    mData(new cedar::aux::MatData())
    {
      this->declareBuffer("buffer", this->mData);
    }

    void switchSlots()
    {
      this->removeBufferSlot("buffer");
      this->declareOutput("output", this->mData);
    }

    bool checkLockSets()
    {
      if (this->getLockCount() != 1)
      {
        std::cout << "Invalid number of locks; should be 1, is " << this->getLockCount() << std::endl;
        return false;
      }

      return true;
    }

  private:
    void compute(const cedar::proc::Arguments&)
    {
      std::cout << "Computing." << std::endl;
    }

    cedar::aux::MatDataPtr mData;
};

CEDAR_GENERATE_POINTER_TYPES(TestStep);

int main(int, char**)
{
  int errors = 0;
  
  TestStepPtr step (new TestStep());

  std::cout << "Testing step ..." << std::endl;

  if (!step->checkLockSets())
  {
    std::cout << "Error in lock set check." << std::endl;
    ++errors;
  }

  std::cout << "Removing and re-registering data." << std::endl;
  step->switchSlots();

  if (!step->checkLockSets())
  {
    std::cout << "Error in lock set check." << std::endl;
    ++errors;
  }

  std::cout << "test finished with " << errors << " error(s)." << std::endl;
  return errors;
}
