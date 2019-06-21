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
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"


// SYSTEM INCLUDES
#include <QApplication>


class TestChannel : public cedar::dev::Channel
{
public:
  TestChannel() : mOpen(false)
  {
  }

  ~TestChannel()
  {
//    prepareChannelDestructAbsolutelyRequired();
//It seems like that function does not exist anymore
  }

  bool isOpen() const
  {
    return this->mOpen;
  }

private:
  void openHook()
  {
    this->mOpen = true;
  }
  void closeHook()
  {
    this->mOpen = false;
  }

  bool mOpen;
};
CEDAR_GENERATE_POINTER_TYPES(TestChannel);


class TestComponent : public cedar::dev::Component
{
public:
  TestComponent(cedar::dev::ChannelPtr channel)
  :
  cedar::dev::Component(channel)
  {
    this->installMeasurementType(0, "test");
    this->registerMeasurementHook(0, boost::bind(&TestComponent::pretendToMeasureSomething, this));
    this->setMeasurementDimensionality(0, 1);
  }

  ~TestComponent()
  {
    prepareComponentDestructAbsolutelyRequired();
  }

  bool applyBrakeSlowlyController()
  {
    return true;
  }

  bool applyBrakeNowController()
  {
    return true;
  }

  cv::Mat pretendToMeasureSomething()
  {
    std::cout << "Pretending to measure!" << std::endl;
    cedar::aux::sleep(cedar::unit::Time(100.0 * cedar::unit::milli * cedar::unit::seconds));
    return cv::Mat(1, 1, CV_32F);
  }
};
CEDAR_GENERATE_POINTER_TYPES(TestComponent);


void run_test()
{
  int errors = 0;

  // create test channel
  TestChannelPtr channel(new TestChannel());

  // create test components, both with the same channel
  TestComponentPtr component1(new TestComponent(channel));
  TestComponentPtr component2(new TestComponent(channel));

  // open the channel
  channel->open();

  // start the components
  component1->startCommunication();
  component2->startCommunication();

  // wait for a bit
  cedar::aux::sleep(cedar::unit::Time(20.0 * cedar::unit::milli * cedar::unit::seconds));

  // stop the components
  component1->stopCommunication();
  component2->stopCommunication();

  // close the channel
  channel->close();

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
