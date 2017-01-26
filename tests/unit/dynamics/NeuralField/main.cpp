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
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/StepTime.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/ObjectParameter.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/auxiliaries/kernel/Box.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/ObjectListParameter.h"
#include "cedar/auxiliaries/ObjectParameter.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <iostream>

// global variables
unsigned int global_errors;

void run_test()
{
  using cedar::proc::LoopedTrigger;
  using cedar::dyn::NeuralField;

  global_errors = 0;

  std::cout << "Reading group.json ... " << std::endl;
  cedar::proc::GroupPtr network(new cedar::proc::Group());
  network->readJson("test://unit/dynamics/NeuralField/group.json");
  std::cout << "done." << std::endl;

#ifdef CEDAR_USE_FFTW
  std::cout << "Testing change of convolution engine." << std::endl;
  cedar::aux::ConfigurablePtr convolution
    = network->getElement<NeuralField>("Field")->getConfigurableChild("lateral kernel convolution");
  cedar::aux::ObjectParameterPtr engine
    = cedar::aux::asserted_pointer_cast<cedar::aux::ObjectParameter>
      (
        convolution->getParameter("engine")
      );
  //engine->setType("cedar.aux.conv.FFTW");

  cedar::aux::ObjectListParameterPtr kernel
    = cedar::aux::asserted_pointer_cast<cedar::aux::ObjectListParameter>
      (
        network->getElement<NeuralField>("Field 1")->getParameter("lateral kernels")
      );
  kernel->pushBack("cedar.aux.kernel.Box");
  kernel->pushBack("cedar.aux.kernel.Gauss");

  // start the processing
  network->getElement<LoopedTrigger>("Main Trigger")->start();
  cedar::aux::sleep(1.0 * cedar::unit::second);

  // stop the processing
  network->getElement<LoopedTrigger>("Main Trigger")->stop();
#endif

  std::cout << "Copying..." << std::endl;
  // check if copying configuration works
  network->getElement<NeuralField>("Field")->copyTo(network->getElement<NeuralField>("Field 1"));

  network->getElement<NeuralField>("Field 1")->copyFrom(network->getElement<NeuralField>("Field"));

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


