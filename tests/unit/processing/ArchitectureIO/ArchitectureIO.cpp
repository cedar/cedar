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

#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/DoubleData.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

/*!
 * This is a generic class for testing. When compute is called, its data is set from zero to one.
 */
class TestModule : public cedar::proc::Step
{
  public:
    TestModule(const std::string& name = "", bool looped = false)
    :
    cedar::proc::Step(looped),
    _mParam(new cedar::aux::UIntParameter(this, "param", 0, 0, 100)),
    mData(new cedar::aux::DoubleData(0.0))
    {
      this->setName(name);

      this->declareInput("input", false);
      this->declareOutput("output", this->mData);
    }

    void compute(const cedar::proc::Arguments&)
    {
      std::cout << "Setting data of step " << this->getName() << " ... ";
      mData->setData(1.0);
      std::cout << "done. Data is now: " << mData->getData() << std::endl;
    }

    cedar::aux::UIntParameterPtr _mParam;
    cedar::aux::DoubleDataPtr mData;
};

class LoopedTestModule : public TestModule
{
public:
  LoopedTestModule(const std::string& name = "")
  :
  TestModule(name, true)
  {
  }
};

/*!
 * This class is used for testing double connections between steps.
 */
class TestModuleDoubleConnection : public cedar::proc::Step
{
  public:
    TestModuleDoubleConnection(const std::string& name = "")
    :
    mData1(new cedar::aux::DoubleData(0.0)),
    mData2(new cedar::aux::DoubleData(0.0))
    {
      this->setName(name);

      this->declareInput("input1", false);
      this->declareInput("input2", false);
      this->declareOutput("output1", this->mData1);
      this->declareOutput("output2", this->mData2);
    }

    void compute(const cedar::proc::Arguments&)
    {
    }

    cedar::aux::DoubleDataPtr mData1;
    cedar::aux::DoubleDataPtr mData2;
};

CEDAR_GENERATE_POINTER_TYPES(TestModule);
CEDAR_GENERATE_POINTER_TYPES(LoopedTestModule);
CEDAR_GENERATE_POINTER_TYPES(TestModuleDoubleConnection);

int main(int, char**)
{
  unsigned int errors = 0;

  std::cout << "Creating step declarations ... ";
  cedar::proc::ElementDeclarationPtr test_module_decl
  (
    new cedar::proc::ElementDeclarationTemplate<TestModule>("Test")
  );
  cedar::proc::ElementDeclarationPtr looped_test_module_decl
  (
    new cedar::proc::ElementDeclarationTemplate<LoopedTestModule>("Test")
  );
  cedar::proc::ElementDeclarationPtr double_test_module_decl
  (
    new cedar::proc::ElementDeclarationTemplate<TestModuleDoubleConnection>("Test")
  );
  std::cout << "done." << std::endl;

  std::cout << "Adding declarations to the registry ... ";
  test_module_decl->declare();
  looped_test_module_decl->declare();
  double_test_module_decl->declare();
  std::cout << "done." << std::endl;

  std::cout << "Creating network ... " << std::endl;
  cedar::proc::GroupPtr group (new cedar::proc::Group());

  std::cout << "Creating step1 ... ";
  LoopedTestModulePtr step1 (new LoopedTestModule("step1"));
  group->add(step1);
  step1->_mParam->setValue(1);
  std::cout << "done." << std::endl;

  std::cout << "Creating step2 ... ";
  TestModulePtr step2 (new TestModule("step2"));
  group->add(step2);
  step2->_mParam->setValue(2);
  std::cout << "done." << std::endl;

  std::cout << "Connecting step1 to step2 ... ";
  group->connectSlots("step1.output", "step2.input");
  std::cout << "done." << std::endl;

  std::cout << "Creating trigger ... ";
  cedar::proc::LoopedTriggerPtr trigger
                                (
                                  new cedar::proc::LoopedTrigger
                                      (
                                        cedar::unit::Time(10.0 * cedar::unit::milli * cedar::unit::seconds),
                                        "trigger"
                                      )
                                );
  group->add(trigger);
  std::cout << "done." << std::endl;

  std::cout << "Connecting trigger to step1 ... ";
  group->connectTrigger(trigger, step1);
  std::cout << "done." << std::endl;

  std::cout << "Creating double connection steps ... ";
  TestModuleDoubleConnectionPtr double_step1 (new TestModuleDoubleConnection("doubleStep1"));
  TestModuleDoubleConnectionPtr double_step2 (new TestModuleDoubleConnection("doubleStep2"));
  group->add(double_step1);
  group->add(double_step2);
  std::cout << "done." << std::endl;

  std::cout << "Connecting double connection steps ... ";
  group->connectSlots("doubleStep1.output1", "doubleStep2.input1");
  group->connectSlots("doubleStep1.output2", "doubleStep2.input2");
  std::cout << "done." << std::endl;

  std::cout << "Group creation completed." << std::endl;

  std::cout << "Triggering ... ";
  trigger->step(cedar::unit::Time(10.0 * cedar::unit::milli * cedar::unit::seconds));
  std::cout << "done." << std::endl;

  if (step1->mData->getData() == 0.0)
  {
    std::cout << "step1 was not triggered properly: Expected value different from 0.0, got " << step1->mData->getData()
        << std::endl;
    ++errors;
  }

  if (step2->mData->getData() == 0.0)
  {
    std::cout << "step2 was not triggered properly: Expected value different from 0.0, got " << step2->mData->getData()
        << std::endl;
    ++errors;
  }

  std::cout << "Saving network ... ";
  group->writeJson("test://unit/processing/ArchitectureIO/architecture1.json");
  std::cout << "done." << std::endl;

  std::cout << "Resetting ... ";
  group.reset();
  step1.reset();
  step2.reset();
  trigger.reset();
  std::cout << "done." << std::endl;

  std::cout << "Loading network ... ";
  group = cedar::proc::GroupPtr(new cedar::proc::Group());
  group->readJson("test://unit/processing/ArchitectureIO/architecture1.json");
  std::cout << "done." << std::endl;

  try
  {
    std::cout << "Looking for step1 ... ";
    step1 = group->getElement<LoopedTestModule>("step1");
    CEDAR_ASSERT(step1);
    std::cout << "found." << std::endl;

    std::cout << "Checking parameter ... ";
    unsigned int value = step1->_mParam->getValue();
    if (value != 1)
    {
      std::cout << "wrong value: expected 1, got " << value << std::endl;
      ++errors;
    }
    else
    {
      std::cout << "done." << std::endl;
    }
  }
  catch (const cedar::aux::InvalidNameException&)
  {
    std::cout << "NOT FOUND." << std::endl;
    ++errors;
  }

  try
  {
    std::cout << "Looking for step2 ... ";
    step2 = group->getElement<TestModule>("step2");
    CEDAR_ASSERT(step2);
    std::cout << "found." << std::endl;

    std::cout << "Checking parameter ... ";
    unsigned int value = step2->_mParam->getValue();
    if (value != 2)
    {
      std::cout << "wrong value: expected 2, got " << value << std::endl;
      ++errors;
    }
    else
    {
      std::cout << "done." << std::endl;
    }
  }
  catch (const cedar::aux::InvalidNameException&)
  {
    std::cout << "NOT FOUND." << std::endl;
    ++errors;
  }

  try
  {
    std::cout << "Looking for trigger ... ";
    trigger = group->getElement<cedar::proc::LoopedTrigger>("trigger");
    CEDAR_ASSERT(trigger);
    std::cout << "found." << std::endl;
  }
  catch (const cedar::aux::InvalidNameException&)
  {
    std::cout << "NOT FOUND." << std::endl;
    ++errors;
  }

  trigger->step(cedar::unit::Time(10.0 * cedar::unit::milli * cedar::unit::seconds));

  if (step1->mData->getData() == 0.0)
  {
    std::cout << "Loaded step1 was not triggered properly: Expected value different from 0.0, got "
        << step1->mData->getData() << std::endl;
    ++errors;
  }

  if (step2->mData->getData() == 0.0)
  {
    std::cout << "Loaded step1 was not triggered properly: Expected value different from 0.0, got "
        << step2->mData->getData() << std::endl;
    ++errors;
  }

  if (step1->mData->getData() == 0.0 || step2->mData->getData() == 0.0)
  {
    std::cout << "Listeners of trigger are:" << std::endl;
    for (size_t i = 0; i < trigger->getListeners().size(); ++i)
    {
      std::cout << " -- " << boost::dynamic_pointer_cast<cedar::proc::Element>(trigger->getListeners().at(i))->getName() << std::endl;
    }
  }

  return errors;
}
