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

    File:        AccelerationSensorSerial.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Acceleration sensor of the e-puck, which communicates over a serial channel.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/devices/kteam/epuck/AccelerationSensorSerial.h"
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/devices/kteam/serialChannelHelperFunctions.h"

// SYSTEM INCLUDES
#include <string>
#include <sstream>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered()
  {
    cedar::dev::ComponentManagerSingleton::getInstance()->
        registerType<cedar::dev::kteam::epuck::AccelerationSensorSerialPtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::kteam::epuck::AccelerationSensorSerial::AccelerationSensorSerial()
:
_mCommandGetAcceleration(new cedar::aux::StringParameter(this, "command get acceleration", "A"))
{
  this->installMeasurementType(0, "acceleration");
  this->setMeasurementDimensionality(0, 3);
  this->registerMeasurementHook(0, boost::bind(&cedar::dev::kteam::epuck::AccelerationSensorSerial::getData, this));
}

cedar::dev::kteam::epuck::AccelerationSensorSerial::AccelerationSensorSerial
(
  cedar::dev::kteam::SerialChannelPtr channel
)
:
cedar::dev::Sensor(cedar::aux::asserted_pointer_cast<cedar::dev::Channel>(channel)),
_mCommandGetAcceleration(new cedar::aux::StringParameter(this, "command get acceleration", "A"))
{
}

cedar::dev::kteam::epuck::AccelerationSensorSerial::~AccelerationSensorSerial()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cv::Mat cedar::dev::kteam::epuck::AccelerationSensorSerial::getData()
{
  // the acceleration values for the x-, y-, and z-axis will be stored in this matrix
  cv::Mat acceleration = cv::Mat::zeros(1, 3, CV_32F);

  // cast the channel into a serial channel
  cedar::dev::kteam::SerialChannelPtr serial_channel
    = cedar::aux::asserted_pointer_cast<cedar::dev::kteam::SerialChannel>(getChannel());

  // send a command string to the robot to receive the current acceleration values
  std::string answer = serial_channel->writeAndReadLocked(_mCommandGetAcceleration->getValue());

  // check whether the first character of the answer is correct
  checkSerialCommunicationAnswer(answer, _mCommandGetAcceleration->getValue());

  // create a string stream on the received answer
  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip 'a,' at the beginning of the answer
  answer_stream.ignore(2);
  checkStreamValidity(answer_stream, false);

  // read the acceleration along the x-axis
  answer_stream >> acceleration.at<float>(0);
  checkStreamValidity(answer_stream, false);

  // skip the colon
  answer_stream.ignore(1);
  checkStreamValidity(answer_stream, false);

  // read the acceleration along the y-axis
  answer_stream >> acceleration.at<float>(1);
  checkStreamValidity(answer_stream, false);

  // skip the colon
  answer_stream.ignore(1);
  checkStreamValidity(answer_stream, false);

  // read the acceleration along the z-axis
  answer_stream >> acceleration.at<float>(2);
  checkStreamValidity(answer_stream, true);

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received acceleration values",
    "cedar::dev::kteam::epuck::AccelerationSensorSerial",
    "Read acceleration"
  );

  return acceleration;
}
