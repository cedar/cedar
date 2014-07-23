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

    File:        main.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 08

    Description: Implements all unit tests for the @em cedar::dev::Robot class.

    Credits:

======================================================================================================================*/


// PROJECT INCLUDES
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/testingUtilities/helpers.h"

// SYSTEM INCLUDES
#include <QApplication>



class TestComponent : public cedar::dev::Component
{
public:
  int testTypeInstalling()
  {
    int errors = 0;

    {
      this->installCommandType(0, "command0");
      auto name = this->getNameForCommandType(0);
      CEDAR_UNIT_TEST_CONDITION(errors, name == "command0");
    }

    //TODO test if exception is thrown when same type is installed again

    {
      CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
      this->getNameForCommandType(1);
      CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "retrieving name of uninstalled command type.");
    }

    {
      this->installMeasurementType(0, "measurement0");
      auto name = this->getNameForMeasurementType(0);
      CEDAR_UNIT_TEST_CONDITION(errors, name == "measurement0");
    }

    {
      CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
      this->getNameForMeasurementType(1);
      CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "retrieving name of uninstalled measurement type.");
    }

    return errors;
  }

  int testHookChecks()
  {
    int errors = 0;
    auto dummy_command_hook = [](cv::Mat) {};

    CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
    this->registerDeviceCommandHook(0, boost::bind<void>(dummy_command_hook, _1));
    CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "registering a hook for an uninstalled command type.");

    auto dummy_measurement_hook = []() { return cv::Mat();};
    CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
    this->registerDeviceMeasurementHook(0, boost::bind<cv::Mat>(dummy_measurement_hook));
    CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "registering a hook for an uninstalled measurement type.");

    //!@todo Test duplicate registration

    return errors;
  }


  int testHookData()
  {
    int errors = 0;

    {
      auto dummy_command_hook = [](cv::Mat) {};
      this->installCommandType(0, "command");
      this->registerDeviceCommandHook(0, boost::bind<void>(dummy_command_hook, _1));

      CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
      this->getDeviceCommandData(0);
      CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "getting device command data that doesn't have a dimensionality set.");

      CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
      this->setCommandDimensionality(0, 1);
      CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "setting command dimensionality.");

      CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
      this->setCommandDimensionality(0, 1);
      CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "setting command dimensionality.");

      cedar::aux::DataPtr data;
      CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
      data = this->getDeviceCommandData(0);
      CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "getting measurement data.");

      CEDAR_UNIT_TEST_CONDITION(errors, data.get() != nullptr);
    }

    {
      auto dummy_measurement_hook = []() {return cv::Mat();};
      this->installMeasurementType(0, "measurement");
      this->registerDeviceMeasurementHook(0, boost::bind<cv::Mat>(dummy_measurement_hook));

      CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
      this->getDeviceMeasurementData(0);
      CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "getting device measurement data that doesn't have a dimensionality set.");

      CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
      this->setMeasurementDimensionality(0, 1);
      CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "setting measurement dimensionality.");

      cedar::aux::DataPtr data;
      CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
      data = this->getDeviceMeasurementData(0);
      CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "getting measurement data.");

      CEDAR_UNIT_TEST_CONDITION(errors, data.get() != nullptr);
    }

    return errors;
  }

};
CEDAR_GENERATE_POINTER_TYPES(TestComponent);

int test_construction()
{
  std::cout << "Constructing an empty component." << std::endl;
  volatile TestComponentPtr component(new TestComponent());
  return 0;
}

int test_type_installing()
{
  std::cout << "Testing installing and recalling of command- and measurement types." << std::endl;
  TestComponentPtr component(new TestComponent());
  return component->testTypeInstalling();
}

int test_hook_checks()
{
  TestComponentPtr component(new TestComponent());
  return component->testHookChecks();
}

int test_hook_data()
{
  std::cout << "Testing hook setting." << std::endl;
  int errors = 0;

  TestComponentPtr component(new TestComponent());
  errors += component->testHookData();

  return errors;
}

void run_test()
{
  int errors = 0;

  errors += test_construction();
  errors += test_type_installing();
  errors += test_hook_checks();
  errors += test_hook_data();

  QApplication::exit(errors);
}


int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  cedar::aux::CallFunctionInThread caller(boost::bind(&run_test));

  caller.start();

  int errors = app.exec();
  std::cout << "Test finished with " << errors << " error(s)." << std::endl;
  return errors;
}
