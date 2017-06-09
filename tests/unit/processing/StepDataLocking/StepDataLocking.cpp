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

    File:        StepDataLocking.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 03 05

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <iostream>

class TestStep : public cedar::proc::Step
{
  public:
    TestStep()
    :
    mErrors(0),
    mInput(new cedar::aux::MatData()),
    mBuffer(new cedar::aux::MatData()),
    mOutput(new cedar::aux::MatData())
    {
      this->declareInput("input");
      // because we don't use a network here, we manually set some input data
      this->setInput("input", this->mInput);

      this->declareBuffer("buffer", this->mBuffer);

      this->declareOutput("output", this->mOutput);
    }

  public:
    int mErrors;

  private:
    void compute(const cedar::proc::Arguments&)
    {
      auto& in_lock = this->mInput->getLock();
      if (in_lock.tryLockForWrite())
      {
        std::cout << "ERROR: input write lock acquired successfully." << std::endl;
        ++mErrors;
      }
      else
      {
        std::cout << "Passed: input write lock could not be acquired." << std::endl;
      }

      auto& buf_lock = this->mBuffer->getLock();
      if (buf_lock.tryLockForWrite())
      {
        std::cout << "ERROR: buffer write lock acquired successfully." << std::endl;
        ++mErrors;
      }
      else
      {
        std::cout << "Passed: buffer write lock could not be acquired." << std::endl;
      }

      auto& out_lock = this->mOutput->getLock();
      if (out_lock.tryLockForWrite())
      {
        std::cout << "ERROR: output write lock acquired successfully." << std::endl;
        ++mErrors;
      }
      else
      {
        std::cout << "Passed: output write lock could not be acquired." << std::endl;
      }
    }

    cedar::aux::MatDataPtr mInput;
    cedar::aux::MatDataPtr mBuffer;
    cedar::aux::MatDataPtr mOutput;
};

CEDAR_GENERATE_POINTER_TYPES(TestStep);



void run_test()
{
  int errors = 0;
  
  TestStepPtr step (new TestStep());

  step->onTrigger();

  errors += step->mErrors;

  std::cout << "test finished with " << errors << " error(s)." << std::endl;
  QApplication::exit(errors);
}

int main(int argc, char* argv[])
{
  QCoreApplication app(argc,argv);

  auto test_thread = cedar::aux::CallFunctionInThreadPtr(new cedar::aux::CallFunctionInThread(run_test));
  test_thread->start();

  return app.exec();
}


