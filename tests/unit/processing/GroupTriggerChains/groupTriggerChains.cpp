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

    File:        triggerChains.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 08 08

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

// CEDAR INCLUDES
#include "cedar/processing/sources/Noise.h"
#include "cedar/processing/steps/StaticGain.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/LoopedTrigger.h"
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

int global_errors = 0;
int num_superfluous_triggers = 0;
std::vector<std::string> failed_configurations;

typedef cedar::aux::DataTemplate<unsigned int> UIntData;
CEDAR_GENERATE_POINTER_TYPES(UIntData);

class TriggerTest : public cedar::proc::Step
{
public:
  TriggerTest(bool looped = false)
  :
  cedar::proc::Step(looped),
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
    std::cout << "Computing " << this->getName();
    ++mTriggerCount;
    unsigned int in1 = 0;
    unsigned int in2 = 0;
    auto data_1 = this->getInput("in1");
    if (auto uint_1 = boost::dynamic_pointer_cast<ConstUIntData>(data_1))
    {
      in1 = uint_1->getData();
    }
    auto data_2 = this->getInput("in2");
    if (auto uint_2 = boost::dynamic_pointer_cast<ConstUIntData>(data_2))
    {
      in2 = uint_2->getData();
    }
    unsigned int new_data = in1+in2+1;

    std::cout << "; new data is " << new_data << std::endl;
    this->mDataOut->setData(new_data);
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

void test_trigger(cedar::proc::LoopedTriggerPtr trigger, TriggerTestPtr sink, const std::string& testName)
{
  using cedar::proc::Group;
  using cedar::proc::GroupPtr;

  sink->resetData();
  std::cout << ">>> Triggering looped trigger." << std::endl;
  trigger->singleStep();
  // wait for processing of the trigger chain
  //!@todo Is there a better way than just randomly waiting?
  cedar::aux::usleep(10000);

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

void test_group(TriggerTestPtr source, TriggerTestPtr sink, unsigned int data, const std::string& testName)
{
  using cedar::proc::Group;
  using cedar::proc::GroupPtr;

  source->resetData();
  sink->resetData();
  std::cout << ">>> Triggering source." << std::endl;
  source->onTrigger();
  if (!sink->isLooped() && sink->mTriggerCount == 0)
  {
    ++global_errors;
    std::cout << "Sink step was not triggered in " << testName << std::endl;
    failed_configurations.push_back(testName + " failed to trigger the sink.");
  }
  else if (!sink->isLooped() && sink->mTriggerCount > 1)
  {
    ++num_superfluous_triggers;
    ++global_errors;
    std::cout << "Sink step was triggered multiple (" << sink->mTriggerCount << ") times in " << testName << std::endl;
    failed_configurations.push_back(testName + " triggered sink more than once.");
  }
  else if (sink->isLooped() && sink->mTriggerCount > 0)
  {
    ++num_superfluous_triggers;
    ++global_errors;
    std::cout << "Looped sink step was triggered (" << sink->mTriggerCount << " times) in " << testName << std::endl;
    failed_configurations.push_back(testName + " triggered looped sink .");
  }
  if (!sink->isLooped() && sink->mDataOut->getData() != data)
  {
    ++global_errors;
    std::cout << "Sink step does not contain correct data in " << testName << std::endl;
    std::cout << "  Expected: " << data << ", got " << sink->mDataOut->getData() << std::endl;
    failed_configurations.push_back(testName + " did not compute the correct data.");
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

    test_group(group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step4"), 5, "configuration 1");
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

    test_group(group->getElement<TriggerTest>("step1"), nested_group->getElement<TriggerTest>("step4"), 5, "configuration 2");
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

    test_group(nested_group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step4"), 5, "configuration 3");
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

    test_group(nested_group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step3"), 3, "configuration 4");
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

    test_group(nested_group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step2"), 2, "configuration 5");
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

    test_group(nested_group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step2"), 2, "configuration 6");
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

    test_group(group->getElement<TriggerTest>("step1"), nested_group->getElement<TriggerTest>("step2"), 2, "configuration 7");
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

    test_group(group->getElement<TriggerTest>("step1"), nested_group->getElement<TriggerTest>("step2"), 2, "configuration 8");
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

    test_group(group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step4"), 5, "configuration 9");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 10" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    nested_group->setIsLooped(true);
    group->add(nested_group, "nested");
    group->add(boost::make_shared<cedar::proc::LoopedTrigger>(), "trigger");
    nested_group->add(boost::make_shared<cedar::proc::sources::Noise>(), "noise");
    nested_group->add(boost::make_shared<TriggerTest>(), "step1");
    std::cout << "Connecting noise -> step1.in1" << std::endl;
    nested_group->connectSlots("noise.random", "step1.in1");
    group->connectTrigger(group->getElement<cedar::proc::LoopedTrigger>("trigger"), nested_group);

    test_trigger(group->getElement<cedar::proc::LoopedTrigger>("trigger"), nested_group->getElement<TriggerTest>("step1"), "configuration 10");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 11" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    GroupPtr nested_nested_group(new Group());
    group->add(nested_group, "nested");
    nested_group->add(nested_nested_group, "nested nested");
    group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(), "step2");
    group->add(boost::make_shared<TriggerTest>(), "step3");

    std::cout << "Connecting step1.out -> step2.in1" << std::endl;
    group->connectSlots("step1.out", "step2.in1");
    std::cout << "Connecting step2.out -> step3.in1" << std::endl;
    group->connectSlots("step2.out", "step3.in1");

    std::cout << "Moving steps to nested group." << std::endl;
    std::list<cedar::proc::ElementPtr> moved;
    moved.push_back(group->getElement("step2"));
    nested_nested_group->add(moved);

    test_group(group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step3"), 3, "configuration 11");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 12" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    GroupPtr also_nested_group(new Group());
    group->add(nested_group, "nested");
    group->add(also_nested_group, "also nested");
    group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(), "step2");
    group->add(boost::make_shared<TriggerTest>(), "step3");
    group->add(boost::make_shared<TriggerTest>(), "step4");

    std::cout << "Connecting step1.out -> step2.in1" << std::endl;
    group->connectSlots("step1.out", "step2.in1");
    std::cout << "Connecting step2.out -> step3.in1" << std::endl;
    group->connectSlots("step2.out", "step3.in1");
    std::cout << "Connecting step3.out -> step4.in1" << std::endl;
    group->connectSlots("step3.out", "step4.in1");

    std::cout << "Moving steps to nested group." << std::endl;
    std::list<cedar::proc::ElementPtr> moved;
    moved.push_back(group->getElement("step2"));
    nested_group->add(moved);

    std::cout << "Moving more steps to nested group." << std::endl;
    std::list<cedar::proc::ElementPtr> more_moved;
    more_moved.push_back(group->getElement("step3"));
    also_nested_group->add(more_moved);

    test_group(group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step4"), 4, "configuration 12");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 13" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    nested_group->addConnector("external output", false);
    nested_group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(true), "step2");

    std::cout << "Connecting external output -> step2.in1" << std::endl;
    group->connectSlots("nested.external output", "step2.in1");
    std::cout << "Connecting step1.out -> external output" << std::endl;
    nested_group->connectSlots("step1.out", "external output.input");

    test_group(nested_group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step2"), 0, "configuration 13");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 14" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    nested_group->addConnector("external output", false);
    nested_group->add(boost::make_shared<TriggerTest>(), "step1");
    group->add(boost::make_shared<TriggerTest>(true), "step2");

    std::cout << "Connecting step1.out -> external output" << std::endl;
    nested_group->connectSlots("step1.out", "external output.input");
    std::cout << "Connecting external output -> step2.in1" << std::endl;
    group->connectSlots("nested.external output", "step2.in1");

    test_group(nested_group->getElement<TriggerTest>("step1"), group->getElement<TriggerTest>("step2"), 0, "configuration 14");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 15" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    nested_group->addConnector("external input", true);
    group->add(boost::make_shared<TriggerTest>(true), "step1");
    nested_group->add(boost::make_shared<TriggerTest>(true), "step2");

    std::cout << "Connecting external input -> step2.in1" << std::endl;
    nested_group->connectSlots("external input.output", "step2.in1");
    std::cout << "Connecting step1.out -> external input" << std::endl;
    group->connectSlots("step1.out", "nested.external input");

    test_group(group->getElement<TriggerTest>("step1"), nested_group->getElement<TriggerTest>("step2"), 0, "configuration 15");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 16" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    GroupPtr group(new Group());
    GroupPtr nested_group(new Group());
    group->add(nested_group, "nested");
    nested_group->addConnector("external input", true);
    nested_group->addConnector("external input 2", true);
    group->add(boost::make_shared<TriggerTest>(true), "step1");
    nested_group->add(boost::make_shared<TriggerTest>(true), "step2");
    nested_group->add(boost::make_shared<TriggerTest>(true), "step3");

    std::cout << "Connecting step1.out -> external input" << std::endl;
    group->connectSlots("step1.out", "nested.external input");
    std::cout << "Connecting step1.out -> external input 2" << std::endl;
    group->connectSlots("step1.out", "nested.external input 2");
    std::cout << "Connecting external input -> step2.in1" << std::endl;
    nested_group->connectSlots("external input.output", "step2.in1");
    std::cout << "Connecting external input 2 -> step3.in1" << std::endl;
    nested_group->connectSlots("external input 2.output", "step3.in1");

    test_group(group->getElement<TriggerTest>("step1"), nested_group->getElement<TriggerTest>("step2"), 0, "configuration 16");
    test_group(group->getElement<TriggerTest>("step1"), nested_group->getElement<TriggerTest>("step3"), 0, "configuration 16");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 17" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;
    /* Configuration 17:
     +-Grp-(looped)+   +-Grp-(looped)-+
     |             |   |              |
     | +-loopd-+   |   |   +-------+  |
     | | Step  |o-o|o-o|o-o| Step  |  |
     | +-------+   |   |   +-------+  |
     +-------------+   +--------------+
     */

    GroupPtr group(new Group());
    GroupPtr nested_group_1(new Group());
    group->add(nested_group_1, "nested1");
    nested_group_1->addConnector("output", false);
    nested_group_1->add(boost::make_shared<TriggerTest>(true), "step1");
    nested_group_1->setIsLooped(true);

    GroupPtr nested_group_2(new Group());
    group->add(nested_group_2, "nested2");
    nested_group_2->addConnector("input", true);
    nested_group_2->add(boost::make_shared<TriggerTest>(false), "step2");
    nested_group_2->setIsLooped(true);

    std::cout << std::endl << "Connecting nested1.step1 -> nested1.output" << std::endl;
    nested_group_1->connectSlots("step1.out", "output.input");

    std::cout << std::endl << "Connecting nested1.output -> nested2.input" << std::endl;
    group->connectSlots("nested1.output", "nested2.input");

    std::cout << std::endl << "Connecting nested2.input -> nested2.step2.input" << std::endl;
    nested_group_2->connectSlots("input.output", "step2.in1");

    auto trigger = boost::make_shared<cedar::proc::LoopedTrigger>();
    group->add(trigger, "trigger");
    group->connectTrigger(trigger, nested_group_1);
    group->connectTrigger(trigger, nested_group_2);

    test_trigger(trigger, nested_group_2->getElement<TriggerTest>("step2"), "configuration 17");
  }

  {
    std::cout << "==================================" << std::endl;
    std::cout << " Checking group configuration 18" << std::endl;
    std::cout << "==================================" << std::endl << std::endl;
    /* Configuration 18:
     +-Grp-(looped)------+    +-Grp-(looped)-------+
     | +-Grp-(looped)+   |    |   +-Grp-(looped)-+ |
     | |             |   |    |   |              | |
     | | +-loopd-+   |   |    |   |   +-------+  | |
     | | | Step  |o-o|o-o|o--o|o-o|o-o| Step  |  | |
     | | +-------+   |   |    |   |   +-------+  | |
     | +-------------+   |    |   +--------------+ |
     +-------------------+    +--------------------+
     */

    GroupPtr group(new Group());
    GroupPtr nested_group_1(new Group());
    nested_group_1->setIsLooped(true);
    group->add(nested_group_1, "nested1");
    nested_group_1->addConnector("output_1", false);

    GroupPtr nested_group_1_1(new Group());
    nested_group_1_1->setIsLooped(true);
    nested_group_1->add(nested_group_1_1, "nested1_1");
    nested_group_1_1->addConnector("output_1_1", false);
    nested_group_1_1->add(boost::make_shared<TriggerTest>(true), "step1");

    GroupPtr nested_group_2(new Group());
    nested_group_2->setIsLooped(true);
    group->add(nested_group_2, "nested2");
    nested_group_2->addConnector("input_2", true);

    GroupPtr nested_group_2_2(new Group());
    nested_group_2_2->setIsLooped(true);
    nested_group_2->add(nested_group_2_2, "nested2_2");
    nested_group_2_2->addConnector("input_2_2", true);
    nested_group_2_2->add(boost::make_shared<TriggerTest>(false), "step2");

    std::cout << std::endl << "Connecting nested1_1.step1 -> nested1_1.output" << std::endl;
    nested_group_1_1->connectSlots("step1.out", "output_1_1.input");

    std::cout << std::endl << "Connecting nested1_1.output -> nested1.sink" << std::endl;
    nested_group_1->connectSlots("nested1_1.output_1_1", "output_1.input");

    std::cout << std::endl << "Connecting nested1.output -> nested2.input" << std::endl;
    group->connectSlots("nested1.output_1", "nested2.input_2");

    std::cout << std::endl << "Connecting nested2.input -> nested2_2.sink" << std::endl;
    nested_group_2->connectSlots("input_2.output", "nested2_2.input_2_2");

    std::cout << std::endl << "Connecting nested2_2.input -> nested2_2.step2.input" << std::endl;
    nested_group_2_2->connectSlots("input_2_2.output", "step2.in1");

    auto trigger = boost::make_shared<cedar::proc::LoopedTrigger>();
    group->add(trigger, "trigger");
    group->connectTrigger(trigger, nested_group_1);
    group->connectTrigger(trigger, nested_group_2);

    test_trigger(trigger, nested_group_2_2->getElement<TriggerTest>("step2"), "configuration 18");
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
  std::cout << "Test finished with " << global_errors << " error(s)." << std::endl;
  std::cout << "These are the failed test configurations:" << std::endl;
  for (auto fail : failed_configurations)
  {
    std::cout << fail << std::endl;
  }

  return global_errors;
}

