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
#include "cedar/processing/Group.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"

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



class StartStopTester : public cedar::proc::Step
{
public:
  StartStopTester(bool looped = false)
  :
  cedar::proc::Step(looped),
  mStartCount(0),
  mStopCount(0),
  mData(new cedar::aux::MatData())
  {
    this->declareOutput("output", this->mData);
    this->declareInput("input1", false);
    this->declareInput("input2", false);
  }

private:
  void onStart()
  {
    mStartCount += 1;
    std::cout << "Step<" << this->getName() << ">::onStart() [n=" << this->mStartCount << "]" << std::endl;
  }

  void onStop()
  {
    mStopCount += 1;
    std::cout << "Step<" << this->getName() << ">::onStop() [n=" << this->mStopCount << "]" << std::endl;
  }

  void compute(const cedar::proc::Arguments&)
  {
  }

public:
  int mStartCount;
  int mStopCount;

private:

  cedar::aux::MatDataPtr mData;
};
CEDAR_GENERATE_POINTER_TYPES(StartStopTester);


int testStartingStopping()
{
  int errors = 0;
  std::cout << "Testing starting/stopping" << std::endl;

  cedar::proc::GroupPtr network(new cedar::proc::Group());
  StartStopTesterPtr step1(new StartStopTester(true));
  StartStopTesterPtr step2(new StartStopTester(true));
  StartStopTesterPtr step3(new StartStopTester());
  cedar::proc::LoopedTriggerPtr trigger(new cedar::proc::LoopedTrigger());

  network->add(step1, "step1");
  network->add(step2, "step2");
  network->add(step3, "step3");
  network->add(trigger, "trigger");

  network->connectSlots("step1.output", "step3.input1");
  network->connectSlots("step2.output", "step3.input2");
  network->connectTrigger(trigger, step1);
  network->connectTrigger(trigger, step2);

  trigger->start();

  if (step1->mStartCount != 1)
  {
    std::cout << "ERROR: step1 has the wrong start count. Should be one, is " << step1->mStartCount << std::endl;
    ++errors;
  }

  if (step2->mStartCount != 1)
  {
    std::cout << "ERROR: step2 has the wrong start count. Should be one, is " << step2->mStartCount << std::endl;
    ++errors;
  }

  if (step3->mStartCount != 1)
  {
    std::cout << "ERROR: step3 has the wrong start count. Should be one, is " << step3->mStartCount << std::endl;
    ++errors;
  }

  trigger->stop();

  if (step1->mStopCount != 1)
  {
    std::cout << "ERROR: step1 has the wrong stop count. Should be one, is " << step1->mStopCount << std::endl;
    ++errors;
  }

  if (step2->mStopCount != 1)
  {
    std::cout << "ERROR: step2 has the wrong stop count. Should be one, is " << step2->mStopCount << std::endl;
    ++errors;
  }

  if (step3->mStopCount != 1)
  {
    std::cout << "ERROR: step3 has the wrong stop count. Should be one, is " << step3->mStopCount << std::endl;
    ++errors;
  }

  std::cout << "Start/stop test uncovered " << errors << " error(s)." << std::endl;
  return errors;
}

class ThrowsInAction : public cedar::proc::Step
{
public:
  ThrowsInAction()
  :
  mData(new cedar::aux::MatData())
  {
    this->declareOutput("output", this->mData);

    this->registerFunction("throwSomething", boost::bind(&ThrowsInAction::throwSomething, this));
  }

private:
  void compute(const cedar::proc::Arguments&)
  {
  }

  void throwSomething()
  {
    CEDAR_ASSERT(false);
  }

private:
  cedar::aux::MatDataPtr mData;
};
CEDAR_GENERATE_POINTER_TYPES(ThrowsInAction);

int testThrowInAction()
{
  std::cout << "Testing throwing in actions" << std::endl;

  ThrowsInActionPtr step(new ThrowsInAction());

  std::cout << "Testing throwing something from a step action." << std::endl;
  try
  {
    step->callAction("throwSomething");
  }
  catch (...)
  {
    // ok, there's supposed to be an exception here ...
  }

  // ... but was the data unlocked properly?
  step->onTrigger();
  return 0;
}

// global variable:
int global_errors;

void run_test()
{
  global_errors = 0;
  
  TestStepPtr step (new TestStep());

  std::cout << "Testing step ..." << std::endl;

  if (!step->checkLockSets())
  {
    std::cout << "Error in lock set check." << std::endl;
    ++global_errors;
  }

  std::cout << "Removing and re-registering data." << std::endl;
  step->switchSlots();

  if (!step->checkLockSets())
  {
    std::cout << "Error in lock set check." << std::endl;
    ++global_errors;
  }

  global_errors += testStartingStopping();
  global_errors += testThrowInAction();

  std::cout << "test finished with " << global_errors << " error(s)." << std::endl;
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


