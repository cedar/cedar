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

    File:        ArchitectureIO.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 14

    Description: Unit test that first writes a processing architecture to a file and then attempts to load it again.

    Credits:

======================================================================================================================*/

#include "cedar/processing/Step.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleData.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"


class TestModule : public cedar::proc::Step
{
  public:
    TestModule()
    {
      this->declareInput("input1", true);
      this->declareInput("input2", true);
    }

    void compute(const cedar::proc::Arguments&)
    {
      // access both inputs, this makes sure that the step is not computed when it shouldn't be
      double data1 = this->getInput("input1")->getData<double>();
      std::cout << "input1:" << data1 << std::endl;
      double data2 = this->getInput("input2")->getData<double>();
      std::cout << "input2:" << data2 << std::endl;
    }

    bool containsInputLock(QReadWriteLock* pLock)
    {
      auto locks = this->getLocks(this->getInputLockSet());
      for (auto iter = locks.begin(); iter != locks.end(); ++iter)
      {
        if (iter->first == pLock)
        {
          return true;
        }
      }

      return false;
    }
};
CEDAR_GENERATE_POINTER_TYPES(TestModule);

class TestSource : public cedar::proc::Step
{
  public:
    TestSource(double value)
    :
    mOutput(new cedar::aux::DoubleData(value))
    {
      this->declareOutput("output", mOutput);
    }

    void compute(const cedar::proc::Arguments&)
    {
      // nothing to do here
    }

    cedar::aux::DoubleDataPtr mOutput;
};
CEDAR_GENERATE_POINTER_TYPES(TestSource);


class TestSource2 : public cedar::proc::Step
{
  public:
    TestSource2()
    :
    mOutput(new cedar::aux::DoubleData(0.0))
    {
      this->declareOutput("output", mOutput);
    }

    void compute(const cedar::proc::Arguments&)
    {
      // nothing to do here
    }

    void changeOutputPtr()
    {
      mOutput = cedar::aux::DoubleDataPtr(new cedar::aux::DoubleData(1.0));
      this->setOutput("output", mOutput);
    }

    bool containsOutputLock(QReadWriteLock* pLock)
    {
      auto locks = this->getLocks(this->getOutputLockSet());
      for (auto iter = locks.begin(); iter != locks.end(); ++iter)
      {
        if (iter->first == pLock)
        {
          return true;
        }
      }

      return false;
    }

    cedar::aux::DoubleDataPtr mOutput;
};
CEDAR_GENERATE_POINTER_TYPES(TestSource2);


class ThrowsDuringDeterminInputValidity : public cedar::proc::Step
{
  public:
    ThrowsDuringDeterminInputValidity()
    {
      this->declareInput("input");
    }

    void compute(const cedar::proc::Arguments&)
    {
      // nothing to do here
    }

    cedar::proc::DataSlot::VALIDITY determineInputValidity
    (
      cedar::proc::ConstDataSlotPtr /* slot */,
      cedar::aux::ConstDataPtr /* data */
    )
    const
    {
      // we want to throw a failed assertion exception
      CEDAR_ASSERT(false);

      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
};
CEDAR_GENERATE_POINTER_TYPES(ThrowsDuringDeterminInputValidity);

int testDetermineInputValidityThrow()
{
  int errors = 0;

  std::cout << "Testing throwing from determineInputValidity while connecting." << std::endl;

  cedar::proc::GroupPtr group (new cedar::proc::Group());

  TestSourcePtr src(new TestSource(0.0));
  ThrowsDuringDeterminInputValidityPtr tar(new ThrowsDuringDeterminInputValidity());

  group->add(src, "source");
  group->add(tar, "target");

  // connect the slots; this should properly deal with the exception
  group->connectSlots("source.output", "target.input");

  // trigger the source; should not trigger the target
  src->onTrigger();

  return errors;
}



int testPtrChange()
{
  int errors = 0;

  cedar::proc::GroupPtr network (new cedar::proc::Group());
  TestSource2Ptr src(new TestSource2());
  TestModulePtr tar(new TestModule());
  network->add(src, "source");
  network->add(tar, "target");

  network->connectSlots("source.output", "target.input1");

  cedar::aux::ConstDataPtr data_src = src->getOutput("output");
  if (!data_src)
  {
    std::cout << "ERROR: data was not connected properly." << std::endl;
    ++errors;
    return errors;
  }

  // remember address of the initial lock
  QReadWriteLock* p_first_lock = &data_src->getLock();

  cedar::aux::ConstDataPtr data_tar = tar->getInput("input1");
  if (!data_tar)
  {
    std::cout << "ERROR: data was not connected properly." << std::endl;
    ++errors;
    return errors;
  }

  if (data_src != data_tar)
  {
    std::cout << "ERROR: data was connected, but the pointers aren't right." << std::endl;
    ++errors;
    return errors;
  }

  src->changeOutputPtr();

  data_src = src->getOutput("output");
  if (!data_src)
  {
    std::cout << "ERROR after pointer change: data was not connected properly." << std::endl;
    ++errors;
    return errors;
  }

  data_tar = tar->getInput("input1");
  if (!data_tar)
  {
    std::cout << "ERROR after pointer change: data was not connected properly." << std::endl;
    ++errors;
    return errors;
  }


  if (data_src != data_tar)
  {
    std::cout << "ERROR after pointer change: data was connected, but the pointers aren't right." << std::endl;
    ++errors;
    return errors;
  }

  if (tar->containsInputLock(p_first_lock))
  {
    std::cout << "ERROR: lock of initial data was not properly removed from target." << std::endl;
    ++errors;
  }

  if (src->containsOutputLock(p_first_lock))
  {
    std::cout << "ERROR: lock of initial data was not properly removed from source." << std::endl;
    ++errors;
  }

  return errors;
}

int testOnlineDisconnecting()
{
  int errors = 0;

  std::cout << "Testing projection for online removal ..." << std::endl;

  size_t trials = 10;

  for (size_t i = 0; i < trials; ++i)
  {
    cedar::proc::GroupPtr group(new cedar::proc::Group());
    group->readJson("test://unit/processing/Connecting/projection.json");

    auto trigger = group->getElement<cedar::proc::LoopedTrigger>("trigger");

    trigger->start();

    cedar::aux::sleep(0.05 * cedar::unit::seconds);

    group->disconnectSlots("step.result", "projection.input");

    cedar::aux::sleep(0.05 * cedar::unit::seconds);

    trigger->stop();
  }

  std::cout << "Online (dis-)connecting revealed " << errors << " error(s)." << std::endl;
  return errors;
}

// global variable:
unsigned int global_errors;

void run_test()
{
  global_errors = 0;

  cedar::proc::GroupPtr network (new cedar::proc::Group());
  network->add(TestSourcePtr(new TestSource(1)), "source1");
  network->add(TestSourcePtr(new TestSource(2)), "source2");
  network->add(TestModulePtr(new TestModule()), "target");

  network->connectSlots("source1.output", "target.input1");
  network->connectSlots("source2.output", "target.input2");

  global_errors += testPtrChange();
  global_errors += testOnlineDisconnecting();
  global_errors += testDetermineInputValidityThrow();

  std::cout << "Done. There were " << global_errors << " error(s)." << std::endl;
}

int main(int argc, char* argv[])
{
  QCoreApplication* app;
  app = new QCoreApplication(argc,argv);

  auto testThread = new cedar::aux::CallFunctionInThread(run_test);

  QObject::connect( testThread, SIGNAL(finishedThread()), app, SLOT(quit()), Qt::QueuedConnection );  // alternatively: call app->quit() in runTests()

  testThread->start();
  app->exec();

  delete testThread;
  delete app;

  return global_errors;
}

