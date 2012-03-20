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
#include "cedar/processing/Manager.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"

// SYSTEM INCLUDES
#include <iostream>


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
  using cedar::proc::Network;
  using cedar::proc::StepPtr;
  using cedar::proc::Step;

  unsigned int errors = 0;

  std::cout << "Creating step declaration ... ";
  cedar::proc::ElementDeclarationPtr test_module_decl
  (
    new cedar::proc::ElementDeclarationTemplate<TestModule>("Test")
  );
  std::cout << "done." << std::endl;

  std::cout << "Adding declaration to the registry ... ";
  cedar::proc::DeclarationRegistrySingleton::getInstance()->declareClass(test_module_decl);
  std::cout << "done." << std::endl;

  std::cout << "Reading Sample.json ... ";
  cedar::proc::NetworkPtr network(new cedar::proc::Network());
  network->readFile("Sample.json");
  std::cout << "done." << std::endl;

  std::cout << "Trying to call compute functions ... ";
  StepPtr step_a = network->getElement<Step>("stepA");
  step_a->onTrigger();

  StepPtr step_b = network->getElement<Step>("stepB");
  step_b->onTrigger();
  std::cout << "done." << std::endl;

  // return
  std::cout << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
