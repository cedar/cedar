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
#include "cedar/processing/Arguments.h"
#include "Neuron.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/StepTime.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <iostream>


typedef boost::shared_ptr<cedar::Neuron> NeuronPtr;

// global variable
unsigned int global_errors;

void run_test()
{
  using cedar::proc::LoopedTrigger;

  global_errors = 0;

  std::cout << "Creating step declaration ... ";
  cedar::proc::ElementDeclarationPtr neuron_declaration
  (
    new cedar::proc::ElementDeclarationTemplate<cedar::Neuron>("Test")
  );
  std::cout << "done." << std::endl;

  std::cout << "Adding declaration to the registry ... ";
  neuron_declaration->declare();
  std::cout << "done." << std::endl;

  std::cout << "Reading Setup1.json ... ";
  cedar::proc::GroupPtr network(new cedar::proc::Group());
  network->readJson("test://unit/dynamics/Dynamics/Setup1.json");
  std::cout << "done." << std::endl;

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
      std::cout << neuron_1->getActivity()
               << " "
               << neuron_2->getActivity()
               << std::endl;
    }
    cedar::aux::sleep(cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second));
  }

  // stop the processing
  network->getElement<LoopedTrigger>("Main Trigger")->stop();

  // return
  std::cout << "Done. There were " << global_errors << " errors." << std::endl;
}

int main(int argc, char* argv[])
{
  QCoreApplication* app;
  app = new QCoreApplication(argc,argv);

  auto testThread = new cedar::aux::CallFunctionInThread(run_test);

  QObject::connect( testThread, SIGNAL(finishedThread()), app, SLOT(quit()), Qt::QueuedConnection );  // alternatively: call app->quit() in runTests()

  testThread->start();
  app->exec();

  delete testThread;
  delete app;

  return global_errors;
}

