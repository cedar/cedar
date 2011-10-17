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

#include "auxiliaries/LogFile.h"
#include "auxiliaries/NumericParameter.h"
#include "processing/Step.h"
#include "processing/StepDeclaration.h"
#include "processing/Manager.h"
#include "processing/Network.h"

class TestModule : public cedar::proc::Step
{
  public:
    TestModule(const std::string& name = "")
    :
    _mParam(new cedar::aux::UIntParameter(this, "param", 0, 0, 100))
    {
      this->setName(name);
    }

    void compute(const cedar::proc::Arguments&)
    {
    }

    cedar::aux::UIntParameterPtr _mParam;
};

CEDAR_GENERATE_POINTER_TYPES(TestModule);

int main(int, char**)
{
  //!@todo Test connections, triggers too
  using cedar::aux::LogFile;

  unsigned int errors = 0;

  LogFile log_file("ArchitectureIO.log");
  log_file.addTimeStamp();
  log_file << std::endl;

  log_file << "Creating step declaration ... ";
  cedar::proc::StepDeclarationPtr test_module_decl
  (
      new cedar::proc::StepDeclarationT<TestModule>("TestModule")
  );
  log_file << "done." << std::endl;

  log_file << "Adding declaration to the manager ... ";
  cedar::proc::Manager::getInstance().steps().declareClass(test_module_decl);
  log_file << "done." << std::endl;

  log_file << "Creating network ... ";
  cedar::proc::NetworkPtr network (new cedar::proc::Network());

  TestModulePtr step1 (new TestModule("step1"));
  network->add(step1);
  step1->_mParam->setValue(1);

  TestModulePtr step2 (new TestModule("step2"));
  network->add(step2);
  step2->_mParam->setValue(2);
  log_file << "done." << std::endl;

  log_file << "Saving network ... ";
  network->writeFile("architecture1.json");
  log_file << "done." << std::endl;


  log_file << "Loading network ... ";
  network = cedar::proc::NetworkPtr(new cedar::proc::Network());
  network->readFile("architecture1.json");
  log_file << "done." << std::endl;

  try
  {
    log_file << "Looking for step1 ... ";
    step1 = boost::shared_dynamic_cast<TestModule>(cedar::proc::Manager::getInstance().steps().get("step1"));
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
      log_file << "done." << value << std::endl;
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
    step2 = boost::shared_dynamic_cast<TestModule>(cedar::proc::Manager::getInstance().steps().get("step2"));
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
      log_file << "done." << value << std::endl;
    }
  }
  catch (const cedar::proc::InvalidNameException&)
  {
    log_file << "NOT FOUND." << std::endl;
    ++errors;
  }

  return errors;
}
