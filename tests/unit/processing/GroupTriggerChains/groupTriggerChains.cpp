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

    File:        triggerChains.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 08 08

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

// CEDAR INCLUDES
#include "cedar/processing/Group.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Trigger.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <QCoreApplication>
#include <boost/make_shared.hpp>
#include <limits.h>


// ---------------------------------------------------------------------------------------------------------------------

typedef cedar::aux::DataTemplate<unsigned int> UIntData;
CEDAR_GENERATE_POINTER_TYPES(UIntData);

class TriggerTest : public cedar::proc::Step
{
public:
  TriggerTest()
  :
  mTriggerCount(0),
  mDataOut(new UIntData(0))
  {
    this->declareInput("in1", false);
    this->declareInput("in2", false);
    this->declareOutput("out", mDataOut);
  }

  void inputConnectionChanged(const std::string& inputName)
  {
    auto data = boost::dynamic_pointer_cast<ConstUIntData>(this->getInput(inputName));
    if (inputName == "in1")
    {
      this->mDataIn1 = data;
    }
    else if (inputName == "in2")
    {
      this->mDataIn2 = data;
    }
  }

  void compute(const cedar::proc::Arguments&)
  {
    std::cout << "Computing " << this->getName() << std::endl;
    ++mTriggerCount;
  }

  void resetData()
  {
    this->mDataOut->setData(0);
    this->mTriggerCount = 0;
  }

  unsigned int mTriggerCount;

  // these data are a kind of time stamp; i.e., the higher the value in them, the more recent they are.
  ConstUIntDataPtr mDataIn1;
  ConstUIntDataPtr mDataIn2;
  UIntDataPtr mDataOut;
};

CEDAR_GENERATE_POINTER_TYPES(TriggerTest);

int global_errors = 0;
int num_superfluous_triggers = 0;
std::vector<std::string> failed_configurations;

void test_group(TriggerTestPtr source, TriggerTestPtr sink, const std::string& testName)
{
  using cedar::proc::Group;
  using cedar::proc::GroupPtr;

  source->resetData();
  sink->resetData();
  std::cout << ">>> Triggering source." << std::endl;
  source->onTrigger();
  if (sink->mTriggerCount == 0)
  {
    ++global_errors;
    std::cout << "Sink step was not triggered in " << testName << std::endl;
    failed_configurations.push_back(testName + " failed to trigger the sink.");
  }
  else if (sink->mTriggerCount > 1)
  {
    ++num_superfluous_triggers;
    std::cout << "Sink step was triggered multiple (" << sink->mTriggerCount << ") times in " << testName << std::endl;
    failed_configurations.push_back(testName + " triggered sink more than once.");
  }
  std::cout << ">>> Done testing configuration." << std::endl;
}


void run_test()
{
  using cedar::proc::Group;
  using cedar::proc::GroupPtr;
  // connectivity looks like this:
  /*
   *  step 1 - step 2 - step 4
   *         \        /
   *           step 3
   */
  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 1" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(), "step2");
    group->add(boost::make_shared<TriggerTest>(), "step3");
    group->add(boost::make_shared<TriggerTest>(), "step4");

    std::cout << "Connecting step1.out -> step2.in1" << std::endl;
    group->connectSlots("step1.out", "step2.in1");
    std::cout << "Connecting step1.out -> step3.in1" << std::endl;
    group->connectSlots("step1.out", "step3.in1");
    std::cout << "Connecting step2.out -> step4.in1" << std::endl;
    group->connectSlots("step2.out", "step4.in1");
    std::cout << "Connecting step3.out -> step4.in2" << std::endl;
    group->connectSlots("step3.out", "step4.in2");

    std::cout << "Moving steps to nested group." << std::endl;
    std::list<cedar::proc::ElementPtr> moved;
    moved.push_back(group->getElement("step2"));
    moved.push_back(group->getElement("step3"));
    nested_group->add(moved);

    test_group(group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step4"), "configuration 1");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 2" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(), "step2");
    group->add(boost::make_shared<TriggerTest>(), "step3");
    group->add(boost::make_shared<TriggerTest>(), "step4");

    std::cout << "Connecting step1.out -> step2.in1" << std::endl;
    group->connectSlots("step1.out", "step2.in1");
    std::cout << "Connecting step1.out -> step3.in1" << std::endl;
    group->connectSlots("step1.out", "step3.in1");
    std::cout << "Connecting step2.out -> step4.in1" << std::endl;
    group->connectSlots("step2.out", "step4.in1");
    std::cout << "Connecting step3.out -> step4.in2" << std::endl;
    group->connectSlots("step3.out", "step4.in2");

    std::cout << "Moving steps to nested group." << std::endl;
    std::list<cedar::proc::ElementPtr> moved;
    moved.push_back(group->getElement("step4"));
    nested_group->add(moved);

    test_group(group->getElement<TriggerTest>("step1"), nested_group->getElement<TriggerTest>("step4"), "configuration 2");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 3" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(), "step2");
    group->add(boost::make_shared<TriggerTest>(), "step3");
    group->add(boost::make_shared<TriggerTest>(), "step4");

    std::cout << "Connecting step1.out -> step2.in1" << std::endl;
    group->connectSlots("step1.out", "step2.in1");
    std::cout << "Connecting step1.out -> step3.in1" << std::endl;
    group->connectSlots("step1.out", "step3.in1");
    std::cout << "Connecting step2.out -> step4.in1" << std::endl;
    group->connectSlots("step2.out", "step4.in1");
    std::cout << "Connecting step3.out -> step4.in2" << std::endl;
    group->connectSlots("step3.out", "step4.in2");

    std::cout << "Moving steps to nested group." << std::endl;
    std::list<cedar::proc::ElementPtr> moved;
    moved.push_back(group->getElement("step1"));
    nested_group->add(moved);

    test_group(nested_group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step4"),  "configuration 3");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 4" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(), "step2");
    group->add(boost::make_shared<TriggerTest>(), "step3");

    std::cout << "Connecting step1.out -> step2.in1" << std::endl;
    group->connectSlots("step1.out", "step2.in1");
    std::cout << "Connecting step2.out -> step3.in1" << std::endl;
    group->connectSlots("step2.out", "step3.in1");

    std::cout << "Moving steps to nested group." << std::endl;
    std::list<cedar::proc::ElementPtr> moved;
    moved.push_back(group->getElement("step1"));
    moved.push_back(group->getElement("step2"));
    nested_group->add(moved);

    test_group(nested_group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step3"), "configuration 4");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 5" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    nested_group->addConnector("external output", false);
    nested_group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(), "step2");

    std::cout << "Connecting step1.out -> external output" << std::endl;
    nested_group->connectSlots("step1.out", "external output.input");
    std::cout << "Connecting external output -> step2.in1" << std::endl;
    group->connectSlots("nested.external output", "step2.in1");

    test_group(nested_group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step2"), "configuration 5");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 6" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    nested_group->addConnector("external output", false);
    nested_group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(), "step2");

    std::cout << "Connecting external output -> step2.in1" << std::endl;
    group->connectSlots("nested.external output", "step2.in1");
    std::cout << "Connecting step1.out -> external output" << std::endl;
    nested_group->connectSlots("step1.out", "external output.input");

    test_group(nested_group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step2"), "configuration 6");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 7" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    nested_group->addConnector("external input", true);
    group->add(boost::make_shared<TriggerTest>(), "step1");
    nested_group->add(boost::make_shared<TriggerTest>(), "step2");

    std::cout << "Connecting step1.out -> external input" << std::endl;
    group->connectSlots("step1.out", "nested.external input");
    std::cout << "Connecting external input -> step2.in1" << std::endl;
    nested_group->connectSlots("external input.output", "step2.in1");

    test_group(group->getElement<TriggerTest>("step1"), nested_group->getElement<TriggerTest>("step2"), "configuration 7");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 8" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    nested_group->addConnector("external input", true);
    group->add(boost::make_shared<TriggerTest>(), "step1");
    nested_group->add(boost::make_shared<TriggerTest>(), "step2");

    std::cout << "Connecting external input -> step2.in1" << std::endl;
    nested_group->connectSlots("external input.output", "step2.in1");
    std::cout << "Connecting step1.out -> external input" << std::endl;
    group->connectSlots("step1.out", "nested.external input");

    test_group(group->getElement<TriggerTest>("step1"), nested_group->getElement<TriggerTest>("step2"), "configuration 8");
  }

  // this should trigger any non-looped steps connected to group inputs
  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 9" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    nested_group->setIsLooped(true);
    group->add(nested_group, "nested");
    group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(), "step2");
    group->add(boost::make_shared<TriggerTest>(), "step3");
    group->add(boost::make_shared<TriggerTest>(), "step4");

    std::cout << "Connecting step1.out -> step2.in1" << std::endl;
    group->connectSlots("step1.out", "step2.in1");
    std::cout << "Connecting step1.out -> step3.in1" << std::endl;
    group->connectSlots("step1.out", "step3.in1");
    std::cout << "Connecting step2.out -> step4.in1" << std::endl;
    group->connectSlots("step2.out", "step4.in1");
    std::cout << "Connecting step3.out -> step4.in1" << std::endl;
    group->connectSlots("step3.out", "step4.in2");

    std::cout << "Moving steps to nested group." << std::endl;
    std::list<cedar::proc::ElementPtr> moved;
    moved.push_back(group->getElement("step2"));
    nested_group->add(moved);

    test_group(group->getElement<TriggerTest>("step1"), nested_group->getElement<TriggerTest>("step2"), "configuration 9");
  }
}

int main(int argc, char** argv)
{
  QCoreApplication app(argc,argv);

  cedar::aux::CallFunctionInThread testThread(run_test);

  QObject::connect(&testThread, SIGNAL(finishedThread()), &app, SLOT(quit()), Qt::QueuedConnection);  // alternatively: call app->quit() in runTests()

  testThread.start();
  app.exec();

  std::cout << "Superfluous trigger calls: " << num_superfluous_triggers << std::endl;
  std::cout << "Test finished with " << global_errors + num_superfluous_triggers << " error(s)." << std::endl;
  std::cout << "These are the failed test configurations:" << std::endl;
  for (auto fail : failed_configurations)
  {
    std::cout << fail << std::endl;
  }

  return global_errors;
}

