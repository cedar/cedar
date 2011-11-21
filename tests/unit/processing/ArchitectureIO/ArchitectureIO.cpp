/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

#include "cedar/auxiliaries/LogFile.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/processing/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/StepDeclaration.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"

class TestModule : public cedar::proc::Step
{
  public:
    TestModule(const std::string& name = "")
    :
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

CEDAR_GENERATE_POINTER_TYPES(TestModule);

int main(int, char**)
{
  using cedar::aux::LogFile;

  unsigned int errors = 0;

  LogFile log_file("ArchitectureIO.log");
  log_file.addTimeStamp();
  log_file << std::endl;

  log_file << "Creating step declaration ... ";
  cedar::proc::ElementDeclarationPtr test_module_decl
  (
    new cedar::proc::ElementDeclarationT<TestModule>("TestModule")
  );
  log_file << "done." << std::endl;

  log_file << "Adding declaration to the manager ... ";
//  cedar::proc::Manager::getInstance().steps().declareClass(test_module_decl);
  cedar::proc::DeclarationRegistrySingleton::getInstance()->declareClass(test_module_decl);
  log_file << "done." << std::endl;

  log_file << "Creating network ... " << std::endl;
  cedar::proc::NetworkPtr network (new cedar::proc::Network());

  log_file << "Creating step1 ... ";
  TestModulePtr step1 (new TestModule("step1"));
  network->add(step1);
  step1->_mParam->setValue(1);
  log_file << "done." << std::endl;

  log_file << "Creating step2 ... ";
  TestModulePtr step2 (new TestModule("step2"));
  network->add(step2);
  step2->_mParam->setValue(2);
  log_file << "done." << std::endl;

  log_file << "Connecting step1 to step2 ... ";
  network->connectSlots("step1.output", "step2.input");
  log_file << "done." << std::endl;

  log_file << "Creating trigger ... ";
  cedar::proc::TriggerPtr trigger(new cedar::proc::Trigger("trigger"));
  network->add(trigger);
  log_file << "done." << std::endl;

  log_file << "Connecting trigger to step1 ... ";
  network->connectTrigger(trigger, step1);
  log_file << "done." << std::endl;

  log_file << "Network creation completed." << std::endl;

  log_file << "Triggering ... ";
  trigger->trigger();
  log_file << "done." << std::endl;

  if (step1->mData->getData() == 0.0)
  {
    log_file << "step1 was not triggered properly: Expected value different from 0.0, got " << step1->mData->getData()
        << std::endl;
    ++errors;
  }

  if (step2->mData->getData() == 0.0)
  {
    log_file << "step2 was not triggered properly: Expected value different from 0.0, got " << step2->mData->getData()
        << std::endl;
    ++errors;
  }

  log_file << "Saving network ... ";
  network->writeFile("architecture1.json");
  log_file << "done." << std::endl;

  log_file << "Resetting ... ";
  network.reset();
  step1.reset();
  step2.reset();
  trigger.reset();
  log_file << "done." << std::endl;

  log_file << "Loading network ... ";
  network = cedar::proc::NetworkPtr(new cedar::proc::Network());
  network->readFile("architecture1.json");
  log_file << "done." << std::endl;

  try
  {
    log_file << "Looking for step1 ... ";
    step1 = network->getElement<TestModule>("step1");
    CEDAR_ASSERT(step1);
    log_file << "found." << std::endl;

    log_file << "Checking parameter ... ";
    unsigned int value = step1->_mParam->getValue();
    if (value != 1)
    {
      log_file << "wrong value: expected 1, got " << value << std::endl;
      ++errors;
    }
    else
    {
      log_file << "done." << std::endl;
    }
  }
  catch (const cedar::proc::InvalidNameException&)
  {
    log_file << "NOT FOUND." << std::endl;
    ++errors;
  }

  try
  {
    log_file << "Looking for step2 ... ";
    step2 = network->getElement<TestModule>("step2");
    CEDAR_ASSERT(step2);
    log_file << "found." << std::endl;

    log_file << "Checking parameter ... ";
    unsigned int value = step2->_mParam->getValue();
    if (value != 2)
    {
      log_file << "wrong value: expected 2, got " << value << std::endl;
      ++errors;
    }
    else
    {
      log_file << "done." << std::endl;
    }
  }
  catch (const cedar::proc::InvalidNameException&)
  {
    log_file << "NOT FOUND." << std::endl;
    ++errors;
  }

  try
  {
    log_file << "Looking for trigger ... ";
    trigger = network->getElement<cedar::proc::Trigger>("trigger");
    CEDAR_ASSERT(trigger);
    log_file << "found." << std::endl;
  }
  catch (const cedar::proc::InvalidNameException&)
  {
    log_file << "NOT FOUND." << std::endl;
    ++errors;
  }

  trigger->trigger();

  if (step1->mData->getData() == 0.0)
  {
    log_file << "Loaded step1 was not triggered properly: Expected value different from 0.0, got "
        << step1->mData->getData() << std::endl;
    ++errors;
  }

  if (step2->mData->getData() == 0.0)
  {
    log_file << "Loaded step1 was not triggered properly: Expected value different from 0.0, got "
        << step2->mData->getData() << std::endl;
    ++errors;
  }

  if (step1->mData->getData() == 0.0 || step2->mData->getData() == 0.0)
  {
    log_file << "Listeners of trigger are:" << std::endl;
    for (size_t i = 0; i < trigger->getListeners().size(); ++i)
    {
      log_file << " -- " << boost::shared_dynamic_cast<cedar::proc::Element>(trigger->getListeners().at(i))->getName() << std::endl;
    }
  }

  return errors;
}
