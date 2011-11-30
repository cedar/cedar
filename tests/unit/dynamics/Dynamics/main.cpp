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
    Date:        2011 06 03

    Description:

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/LogFile.h"
#include "cedar/processing/Arguments.h"
#include "Neuron.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/StepTime.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h"

// SYSTEM INCLUDES
#include <iostream>

using namespace cedar::aux;

typedef boost::shared_ptr<cedar::Neuron> NeuronPtr;

int main(int /* argc */, char** /* argv */)
{
  using cedar::proc::LoopedTrigger;
  using cedar::proc::Manager;

  unsigned int errors = 0;

  LogFile log_file("Dynamics.log");
  log_file.addTimeStamp();
  log_file << std::endl;

  log_file << "Creating step declaration ... ";
  cedar::proc::ElementDeclarationPtr neuron_declaration
  (
    new cedar::proc::ElementDeclarationTemplate<cedar::Neuron>("Test")
  );
  log_file << "done." << std::endl;

  log_file << "Adding declaration to the registry ... ";
  cedar::proc::DeclarationRegistrySingleton::getInstance()->declareClass(neuron_declaration);
  log_file << "done." << std::endl;

  log_file << "Reading Setup1.json ... ";
  cedar::proc::NetworkPtr network(new cedar::proc::Network());
  network->readFile("Setup1.json");
  log_file << "done." << std::endl;

  // Create trigger for the "main loop"
  NeuronPtr neuron_1 = network->getElement<cedar::Neuron>("Neuron 1");
  NeuronPtr neuron_2 = network->getElement<cedar::Neuron>("Neuron 2");
  std::cout << "input count of 1 " << neuron_1->getInputSlot("input")->getDataCount() << std::endl;
  std::cout << "input count of 2 " << neuron_2->getInputSlot("input")->getDataCount() << std::endl;
  // start the processing
  network->getElement<LoopedTrigger>("Main Trigger")->start();

  // periodically read out activation values
  for (unsigned int i = 0; i < 1000; i++)
  {
    if (i % 10 == 0)
    {
      log_file << neuron_1->getActivity()
               << " "
               << neuron_2->getActivity()
               << std::endl;
    }
    cedar::aux::sleep(cedar::unit::Milliseconds(1));
  }

  // stop the processing
  network->getElement<LoopedTrigger>("Main Trigger")->stop();

  // return
  log_file << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
