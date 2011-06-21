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
#include "auxiliaries/computation/Arguments.h"
#include "Neuron.h"
#include "auxiliaries/computation/LoopedTrigger.h"
#include "auxiliaries/computation/StepTime.h"

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

  cedar::Neuron neuron;
  // simulated time
//  cedar::aux::comp::StepTime time(cedar::unit::Milliseconds(1.0));
//  for (unsigned int i = 0; i < 1000; i++)
//  {
//    neuron.compute(time);
//    if (i % 100 == 0)
//    {
//      log_file << neuron.getActivity() << std::endl;
//    }
//  }

  NeuronPtr p_neuron(new cedar::Neuron(100.0, 0));
  p_neuron->setThreaded(false);
  NeuronPtr p_another_neuron(new cedar::Neuron(-100.0, 100));
  p_another_neuron->setThreaded(false);
  cedar::aux::comp::LoopedTriggerPtr looped_trigger(new cedar::aux::comp::LoopedTrigger(0.1));
  looped_trigger->addListener(p_neuron);
  looped_trigger->addListener(p_another_neuron);
  cedar::aux::comp::ProcessingStep::connect(p_neuron, "output", p_another_neuron, "input");
  cedar::aux::comp::ProcessingStep::connect(p_another_neuron, "output", p_neuron, "input");
  looped_trigger->start();
  for (unsigned int i = 0; i < 1000; i++)
  {
    if (i % 100 == 0)
    {
      log_file << p_neuron->getActivity() << " " << p_another_neuron->getActivity() << std::endl;
    }
    usleep(1000);
  }
  looped_trigger->stop();

  log_file << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
