/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

// cedar includes
#include "cedar/processing/Step.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/DataRole.h"
#include "cedar/processing/StepTime.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/processing/steps/Resize.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/LogFile.h"
#include "cedar/auxiliaries/logFilter/Type.h"
#include "cedar/auxiliaries/NullLogger.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/units/prefixes.h"
#include "cedar/testingUtilities/helpers.h"

// global includes
#include <QApplication>
#include <exception>
#include <string>
#include <vector>

class EmptyMatrixProvider : public cedar::proc::Step
{
  public:
    EmptyMatrixProvider()
    :
    mData(new cedar::aux::MatData(cv::Mat()))
    {
      this->declareOutput("empty matrix", this->mData);
    }

  private:
    void compute(const cedar::proc::Arguments&)
    {
    }

    cedar::aux::MatDataPtr mData;
};

CEDAR_GENERATE_POINTER_TYPES(EmptyMatrixProvider);


unsigned int testStep(cedar::proc::GroupPtr network, cedar::proc::StepPtr testStep)
{
  // try connecting steps of different types
  unsigned int i = 0;
  CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
    network->add(testStep, "testStep");

    // test if the step reacts properly when its parameters change (without an input)
    for (auto parameter : testStep->getParameters())
    {
      std::cout << "Emitting valueChanged() of parameter \"" << parameter->getName() << "\"." << std::endl;
      parameter->emitChangedSignal();

      size_t count = 0;
      while (QApplication::hasPendingEvents() && ++count < 1000)
      {
        QApplication::processEvents();
      }
    }

    // how many inputs does this step have?
    if (testStep->hasRole(cedar::proc::DataRole::INPUT))
    {
      const cedar::proc::Connectable::SlotList& inputs = testStep->getOrderedDataSlots(cedar::proc::DataRole::INPUT);
      std::vector<std::string> sources;
      sources.push_back("0D.boost");
      sources.push_back("1D.Gauss input");
      sources.push_back("2D.Gauss input");
      sources.push_back("3D.Gauss input");
      sources.push_back("double_0D.converted matrix");
      sources.push_back("double_1D.converted matrix");
      sources.push_back("double_2D.converted matrix");
      sources.push_back("double_3D.converted matrix");
      sources.push_back("emp.empty matrix");
      for (unsigned int src = 0; src < sources.size(); ++src)
      {
        std::cout << "Connecting " << sources.at(src) << " to " << inputs.at(i)->getName() << std::endl;
        for (unsigned int i = 0; i < inputs.size(); ++i)
        {
          network->connectSlots(sources.at(src), std::string("testStep." + inputs.at(i)->getName()));
        }

        if (!testStep->isLooped())
        {
          testStep->onTrigger();
        }
        else
        {
          // send a dummy step time
          cedar::unit::Time time(0.1 * cedar::unit::milli * cedar::unit::seconds);
          cedar::proc::ArgumentsPtr arguments (new cedar::proc::StepTime(time));
          testStep->onTrigger(arguments);
        }

        // try a reset
        testStep->callReset();

        for (unsigned int i = 0; i < inputs.size(); ++i)
        {
          network->disconnectSlots(sources.at(src), std::string("testStep." + inputs.at(i)->getName()));
        }
      }
    }
    network->removeAll();
  CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(i, "testing step.");
  return i;
}

void run_tests()
{
  // Filter out mem-debug messages so the output reamins readable
  cedar::aux::logFilter::TypePtr memdebug_filter(new cedar::aux::logFilter::Type(cedar::aux::LOG_LEVEL_MEM_DEBUG));
  cedar::aux::LogInterfacePtr memdebug_logger(new cedar::aux::NullLogger());
  cedar::aux::LogSingleton::getInstance()->addLogger(memdebug_logger, memdebug_filter);

  unsigned int errors = 0;

  std::vector<std::string> failed_steps;

  auto declarations = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarations();
  for (auto declaration_iter = declarations.begin(); declaration_iter != declarations.end(); ++declaration_iter)
  {
    cedar::aux::ConstPluginDeclarationPtr declaration = *declaration_iter;
    cedar::proc::GroupPtr network(new cedar::proc::Group());
    cedar::proc::ElementPtr elem = cedar::proc::ElementManagerSingleton::getInstance()->allocate(declaration->getClassName());
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(elem))
    {
      std::cout << "=========================================" << std::endl;
      std::cout << "  Testing class " << declaration->getClassName() << std::endl;
      std::cout << "=========================================" << std::endl;
      network->readJson("processing_steps.json");

      EmptyMatrixProviderPtr empty_provider = EmptyMatrixProviderPtr(new EmptyMatrixProvider());
      network->add(empty_provider, "emp");

      int error_count = testStep(network, step);
      errors += error_count;

      if (error_count > 0)
      {
        failed_steps.push_back(declaration->getClassName());
      }
    }
  }

  std::cout << "Test finished with " << errors << " error(s)." << std::endl;

  if (!failed_steps.empty())
  {
    std::cout << "The following steps produced errors:" << std::endl;

    for (size_t i = 0; i < failed_steps.size(); ++i)
    {
      std::cout << "- " << failed_steps.at(i) << std::endl;
    }
  }

  QCoreApplication::exit(errors);
}

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  cedar::aux::CallFunctionInThread caller(boost::bind(&run_tests));
  caller.start();

  return app.exec();
}
