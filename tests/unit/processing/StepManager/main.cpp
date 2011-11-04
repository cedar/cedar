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

    File:        main.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 24

    Description:

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/LogFile.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/StepDeclaration.h"
#include "cedar/processing/Arguments.h"

// SYSTEM INCLUDES
#include <iostream>

using namespace cedar::aux;

class TestModule : public cedar::proc::Step
{
public:
  TestModule()
  {
  }

  void compute(const cedar::proc::Arguments& /* arguments */)
  {
    std::cout << "processing " << this->getName() << " = " << this->_mMessage << std::endl;
  }

  void readConfiguration(const cedar::aux::ConfigurationNode& node)
  {
    this->cedar::proc::Step::readConfiguration(node);
    _mMessage = node.get<std::string>("outputString", "defaultOutputString");
  }

  std::string _mMessage;
};



int main(int /* argc */, char** /* argv */)
{
  using cedar::proc::Manager;
  using cedar::proc::StepPtr;

  unsigned int errors = 0;

  LogFile log_file("StepManager.log");
  log_file.addTimeStamp();
  log_file << std::endl;

  log_file << "Creating TestModule declaration ... ";
  cedar::proc::StepDeclarationPtr test_module_declaration (new cedar::proc::StepDeclarationT<TestModule>("TestModule"));
  Manager::getInstance().steps().declareClass(test_module_declaration);
  log_file << "done." << std::endl;

  log_file << "Reading Sample.json ... ";
  cedar::proc::NetworkPtr network(new cedar::proc::Network());
  network->readFile("Sample.json");
  log_file << "done." << std::endl;

  log_file << "Trying to call compute functions ... ";
  StepPtr step_a = Manager::getInstance().steps().get("stepA");
  step_a->onTrigger();

  StepPtr step_b = Manager::getInstance().steps().get("stepB");
  step_b->onTrigger();
  log_file << "done." << std::endl;

  // return
  log_file << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
