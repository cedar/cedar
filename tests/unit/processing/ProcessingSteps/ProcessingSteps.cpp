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
#include "cedar/auxiliaries/logFilter/Type.h"
#include "cedar/auxiliaries/NullLogger.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/units/prefixes.h"

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

unsigned int checkOutputsForInvalidValues(cedar::proc::StepPtr testStep)
{
  unsigned int errors = 0;

  if (testStep->hasSlotForRole(cedar::proc::DataRole::OUTPUT))
  {
    for (auto name_slot_pair : testStep->getDataSlots(cedar::proc::DataRole::OUTPUT))
    {
      auto slot = name_slot_pair.second;
      if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(slot->getData()))
      {
        if (!cv::checkRange(mat_data->getData(), true))
        {
          std::cout << "ERROR: NaN detected in step's output slot " << slot->getName() << std::endl;
          std::cout << "matrix: " << mat_data->getData() << std::endl;
          ++errors;
        }
      }
    }
  }

  return errors;
}

unsigned int testStep(cedar::proc::GroupPtr group, cedar::proc::StepPtr testStep)
{
  // try connecting steps of different types
  unsigned int errors = 0;

  // check if the step starts out with invalid values
  std::cout << "Checking initial state of the step for invalid values." << std::endl;
  errors += checkOutputsForInvalidValues(testStep);

  try
  {
    group->add(testStep, "testStep");

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
    if (testStep->hasSlotForRole(cedar::proc::DataRole::INPUT))
    {
      const auto& inputs = testStep->getOrderedDataSlots(cedar::proc::DataRole::INPUT);
      for (unsigned int src = 0; src < sources.size(); ++src)
      {
        for (unsigned int i = 0; i < inputs.size(); ++i)
        {
          std::cout << "Connecting " << sources.at(src) << " to " << inputs.at(i)->getName() << std::endl;
          group->connectSlots(sources.at(src), std::string("testStep." + inputs.at(i)->getName()));
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

        // check if the step produces NaNs/INFs or other invalid values
        errors += checkOutputsForInvalidValues(testStep);

        // try a reset
        testStep->callReset();

        // try to re-read the configuration written by the step
        {
          std::cout << "Trying to re-read configuration into existing step." << std::endl;
          cedar::aux::ConfigurationNode conf;
          testStep->writeConfiguration(conf);
          testStep->readConfiguration(conf);
          std::cout << "Done rereading." << std::endl;
        }

        for (unsigned int i = 0; i < inputs.size(); ++i)
        {
          group->disconnectSlots(sources.at(src), std::string("testStep." + inputs.at(i)->getName()));
        }
      }
      group->removeAll();
    }
  }
  catch (cedar::aux::ExceptionBase& exc)
  {
    std::cout << "ERROR: An exception occurred." << std::endl;
    exc.printInfo();
    ++errors;
  }

  std::cout << "Done with " << testStep->getName() << ". It produced " << errors << " error(s)." << std::endl;
  return errors;
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
  for (auto declaration : declarations)
  {
    cedar::proc::GroupPtr group(new cedar::proc::Group());
    auto elem = cedar::proc::ElementManagerSingleton::getInstance()->allocate(declaration->getClassName());
    if (auto step = boost::dynamic_pointer_cast<cedar::proc::Step>(elem))
    {
      std::cout << "=========================================" << std::endl;
      std::cout << "  Testing class " << declaration->getClassName() << std::endl;
      std::cout << "=========================================" << std::endl;
      group->readJson("test://unit/processing/ProcessingSteps/processing_steps.json");

      auto empty_provider = EmptyMatrixProviderPtr(new EmptyMatrixProvider());
      group->add(empty_provider, "emp");

      unsigned int error_count = testStep(group, step);
      errors += error_count;

      if (error_count > 0)
      {
        std::cout << "Step failed the test: " << step->getName() << std::endl;
        failed_steps.push_back(declaration->getClassName());
      }
    }
  }

  std::cout << "Test finished with " << errors << " error(s)." << std::endl;

  if (!failed_steps.empty())
  {
    std::cout << "The following steps produced errors:" << std::endl;

    for (const auto& failed : failed_steps)
    {
      std::cout << "- " << failed << std::endl;
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
