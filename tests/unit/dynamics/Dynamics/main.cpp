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
#include "auxiliaries/LogFile.h"
#include "processing/Arguments.h"
#include "Neuron.h"
#include "processing/LoopedTrigger.h"
#include "processing/Step.h"
#include "processing/StepTime.h"
#include "processing/Manager.h"
#include "processing/StepDeclaration.h"

// SYSTEM INCLUDES
#include <iostream>

using namespace cedar::aux;

typedef boost::shared_ptr<cedar::Neuron> NeuronPtr;

int main(int argc, char** argv)
{
  unsigned int errors = 0;

  LogFile log_file("Dynamics.log");
  log_file.addTimeStamp();
  log_file << std::endl;

  log_file << "Generating StepDeclaration for Neuron ... ";
  cedar::proc::StepDeclarationPtr neuron_declaration(new cedar::proc::StepDeclarationT<cedar::Neuron>("Neuron"));
  cedar::proc::Manager::getInstance().declareStepClass(neuron_declaration);
  log_file << "done." << std::endl;

  log_file << "Reading Setup1.json ... ";
  cedar::proc::Manager::getInstance().readFile("Setup1.json");
  log_file << "done." << std::endl;

  // Create trigger for the "main loop"
  NeuronPtr neuron_1
    = boost::dynamic_pointer_cast<cedar::Neuron>(cedar::proc::Manager::getInstance().getStep("Neuron 1"));
  NeuronPtr neuron_2
    = boost::dynamic_pointer_cast<cedar::Neuron>(cedar::proc::Manager::getInstance().getStep("Neuron 2"));
  cedar::proc::LoopedTriggerPtr looped_trigger(new cedar::proc::LoopedTrigger(0.1));
  looped_trigger->addListener(neuron_1);
  looped_trigger->addListener(neuron_2);

  // start the processing
  looped_trigger->start();

  // preiodically read out activation values
  for (unsigned int i = 0; i < 1000; i++)
  {
    if (i % 10 == 0)
    {
      log_file << neuron_1->getActivity()
               << " "
               << neuron_2->getActivity()
               << std::endl;
    }
    usleep(1000);
  }

  // stop the processing
  looped_trigger->stop();

  // return
  log_file << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
