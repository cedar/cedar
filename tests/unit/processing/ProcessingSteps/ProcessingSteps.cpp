/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        ProcessingSteps.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 09 20

    Description: Unit test that checks if basic inputs to various steps fail.

    Credits:

======================================================================================================================*/

#include "cedar/processing/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/DataRole.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/processing/steps/Resize.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/LogFile.h"
#include "cedar/auxiliaries/logFilter/Type.h"
#include "cedar/auxiliaries/NullLogger.h"
#include <exception>
#include <string>
#include <vector>

unsigned int testStep(cedar::proc::NetworkPtr network, cedar::proc::StepPtr testStep)
{
  unsigned int i = 0;
  try
  {
    network->add(testStep, "testStep");
    // how many inputs does this step have?
    const cedar::proc::Connectable::SlotList& inputs = testStep->getOrderedDataSlots(cedar::proc::DataRole::INPUT);
    std::vector<std::string> sources;
    sources.push_back("0D.boost");
    sources.push_back("1D.Gauss input");
    sources.push_back("2D.Gauss input");
    sources.push_back("3D.Gauss input");
    for (unsigned int src = 0; src < sources.size(); ++src)
    {
      std::cout << "Testing connecting to " << sources.at(src) << std::endl;
      for (unsigned int i = 0; i < inputs.size(); ++i)
      {
        network->connectSlots(sources.at(src), std::string("testStep." + inputs.at(i)->getName()));
      }
      testStep->onTrigger();

      for (unsigned int i = 0; i < inputs.size(); ++i)
      {
        network->disconnectSlots(sources.at(src), std::string("testStep." + inputs.at(i)->getName()));
      }
    }
    network->removeAll();
  }
  catch (cedar::proc::InvalidRoleException& exc)
  {
    // that's fine, it's a source
  }
  catch (cedar::aux::ExceptionBase& exc)
  {
    exc.printInfo();
    ++i;
  }
  return i;
}

int main(int, char**)
{
  // Filter out mem-debug messages so the output reamins readable
  cedar::aux::logFilter::TypePtr memdebug_filter(new cedar::aux::logFilter::Type(cedar::aux::LOG_LEVEL_MEM_DEBUG));
  cedar::aux::LogInterfacePtr memdebug_logger(new cedar::aux::NullLogger());
  cedar::aux::LogSingleton::getInstance()->addLogger(memdebug_logger, memdebug_filter);

  unsigned int errors = 0;

  cedar::proc::DeclarationRegistryPtr declaration_registry = cedar::proc::DeclarationRegistrySingleton::getInstance();
  const cedar::proc::DeclarationRegistry::Declarations& declarations = declaration_registry->declarations();
  for
  (
    cedar::proc::DeclarationRegistry::Declarations::const_iterator declaration = declarations.begin();
    declaration != declarations.end();
    ++declaration
  )
  {
    cedar::proc::NetworkPtr network(new cedar::proc::Network());
    cedar::proc::ElementPtr elem = declaration_registry->allocateClass(declaration->second->getClassId());
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(elem))
    {
      std::cout << "Testing class " << declaration->second->getClassId() << std::endl;
      network->readFile("processing_steps.json");
      errors += testStep(network, step);
    }
  }

  std::cout << "Done. There were " << errors << " error(s)." << std::endl;
  return errors;
}
