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
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <QApplication>



class TestComponent : public cedar::dev::Component
{
public:
  ~TestComponent()
  {
    prepareComponentDestructAbsolutelyRequired();
  }

  int testTypeInstalling()
  {
    int errors = 0;

    {
      this->installCommandType(0, "command0");
      auto name = this->getNameForCommandType(0);
      CEDAR_UNIT_TEST_CONDITION(errors, name == "command0");
    }

    CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION();
    this->installCommandType(0, "command0");
    CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "installing the same command type again.");

    CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION();
    this->getNameForCommandType(1);
    CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "retrieving name of uninstalled command type.");

    {
      this->installMeasurementType(0, "measurement0");
      auto name = this->getNameForMeasurementType(0);
      CEDAR_UNIT_TEST_CONDITION(errors, name == "measurement0");
    }

    CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION();
    this->installMeasurementType(0, "measurement0");
    CEDAR_UNIT_TEST_END_EXPECTING_SPECIFIC_EXCEPTION
    (
      errors,
      "installing the same measurement type again.",
      cedar::dev::Component::DuplicateTypeException
    );

    CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION();
    this->getNameForMeasurementType(1);
    CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "retrieving name of uninstalled measurement type.");

    return errors;
  }

  int testHookChecks()
  {
    int errors = 0;

    auto dummy_command_hook = [](cv::Mat) {};
    CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
    this->registerCommandHook(0, boost::bind<void>(dummy_command_hook, _1));
    CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "registering a hook for an uninstalled command type.");

    auto dummy_measurement_hook = []() { return cv::Mat();};
    CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
    this->registerMeasurementHook(0, boost::bind<cv::Mat>(dummy_measurement_hook));
    CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "registering a hook for an uninstalled measurement type.");

    CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
    this->installCommandAndMeasurementType(0, "test");
    this->registerMeasurementHook(0, boost::bind<cv::Mat>(dummy_measurement_hook));
    this->registerCommandHook(0, boost::bind<void>(dummy_command_hook, _1));
    CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "registering a measurement.");

    CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
    this->registerMeasurementHook(0, boost::bind<cv::Mat>(dummy_measurement_hook));
    CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "registering measurement hook a second time");

    CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
    this->registerCommandHook(0, boost::bind<void>(dummy_command_hook, _1));
    CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "registering command hook a second time");

    return errors;
  }


  int testHookData()
  {
    int errors = 0;

    {
      auto dummy_command_hook = [](cv::Mat) {};
      this->installCommandType(0, "command");
      this->registerCommandHook(0, boost::bind<void>(dummy_command_hook, _1));

      CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
      this->getDeviceSideCommandData(0);
      CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "getting device command data that doesn't have a dimensionality set.");

      CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
      this->setCommandDimensionality(0, 1);
      CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "setting command dimensionality.");

      CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
      this->setCommandDimensionality(0, 1);
      CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "setting command dimensionality.");

      cedar::aux::DataPtr data;
      CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
      data = this->getDeviceSideCommandData(0);
      CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "getting measurement data.");

      CEDAR_UNIT_TEST_CONDITION(errors, data.get() != nullptr);
    }

    {
      auto dummy_measurement_hook = []() {return cv::Mat();};
      this->installMeasurementType(0, "measurement");
      this->registerMeasurementHook(0, boost::bind<cv::Mat>(dummy_measurement_hook));

      CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION()
      this->getMeasurementData(0);
      CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(errors, "getting device measurement data that doesn't have a dimensionality set.");

      CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
      this->setMeasurementDimensionality(0, 1);
      CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "setting measurement dimensionality.");

      cedar::aux::DataPtr data;
      CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE();
      data = this->getMeasurementData(0);
      CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(errors, "getting measurement data.");

      CEDAR_UNIT_TEST_CONDITION(errors, data.get() != nullptr);
    }

    return errors;
  }


  int testLoop()
  {
    int errors = 0;

    std::cout << " installing measurements" << std::endl;
    this->installMeasurementType(0, "measurement");
    this->setMeasurementDimensionality(0, 10);

    this->installCommandType(0, "command");
    this->setCommandDimensionality(0, 10);

    std::cout << " starting device" << std::endl;
    this->startCommunication();

    std::cout << " sleeping" << std::endl;
    cedar::aux::sleep(0.02 * cedar::unit::seconds);

    std::cout << " stopping device" << std::endl;
    this->stopCommunication();

    return errors;
  }

  void installTestMeasurement()
  {
    this->installMeasurementType(0, "test measurement");
    this->setMeasurementDimensionality(0, 1);
    this->registerMeasurementHook(0, boost::bind(&TestComponent::makeTestMeasurement, this));
  }

  cv::Mat makeTestMeasurement() const
  {
    std::cout << "Making measurement." << std::endl;
    return 1.234 * cv::Mat::ones(1, 1, CV_32F);
  }

private:
  bool applyBrakeNowController()
  {
    return true;
  }

  bool applyBrakeSlowlyController()
  {
    return true;
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

  TestComponentPtr component(new TestComponent());
  return component->testHookData();
}

int test_loop()
{
  std::cout << "Testing component loop." << std::endl;

  TestComponentPtr component(new TestComponent());
  return component->testLoop();
}

int test_measurements()
{
  std::cout << "Testing measurements." << std::endl;

  int errors = 0;

  TestComponentPtr component(new TestComponent());
  component->installTestMeasurement();
  component->startCommunication();
  cedar::aux::sleep(0.02 * cedar::unit::second);
  component->stopCommunication();

  auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(component->getMeasurementData(0));
  CEDAR_UNIT_TEST_CONDITION(errors, mat_data.get() != nullptr);
  cv::Mat measurement = mat_data->getData();
  CEDAR_UNIT_TEST_CONDITION(errors, measurement.rows == 1);
  CEDAR_UNIT_TEST_CONDITION(errors, measurement.cols == 1);
  CEDAR_UNIT_TEST_CONDITION(errors, measurement.type() == CV_32F);
  CEDAR_UNIT_TEST_CONDITION(errors, cedar::aux::math::isZero(measurement.at<float>(0, 0) - 1.234f));
  std::cout << "Measurement matrix: " << measurement << std::endl;

  return errors;
}

void run_test()
{
  int errors = 0;

  errors += test_construction();
  errors += test_type_installing();
  errors += test_hook_checks();
  errors += test_hook_data();
  errors += test_loop();
  errors += test_measurements();

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
