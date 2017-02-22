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

    File:        GripperSerial.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Gripper that can be attached to the Khepera robot.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/kteam/khepera/GripperSerial.h"
#include "cedar/devices/kteam/serialChannelHelperFunctions.h"
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/auxiliaries/math/UIntLimitsParameter.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <sstream>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered()
  {
    cedar::dev::ComponentManagerSingleton::getInstance()->
        registerType<cedar::dev::kteam::khepera::GripperSerialPtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::kteam::khepera::GripperSerial::GripperSerial()
{
  this->construct();
}

// constructor taking an externally created channel
cedar::dev::kteam::khepera::GripperSerial::GripperSerial(cedar::dev::kteam::SerialChannelPtr channel)
:
cedar::dev::kteam::khepera::Gripper(cedar::aux::asserted_pointer_cast<cedar::dev::Channel>(channel))
{
  this->construct();
}

cedar::dev::kteam::khepera::GripperSerial::~GripperSerial()
{
  prepareComponentDestructAbsolutelyRequired();
}

void cedar::dev::kteam::khepera::GripperSerial::construct()
{
  this->_mCommandSetGripperPosition = new cedar::aux::StringParameter(this, "command set gripper position", "T,1,D");
  this->_mCommandGetGripperPosition = new cedar::aux::StringParameter(this, "command get gripper position", "T,1,H,0");
  this->_mAnswerGetGripperPosition = new cedar::aux::StringParameter(this, "answer get gripper position", "t,1,h");
  this->_mCommandGetGripperResistivity = new cedar::aux::StringParameter(this, "command get gripper resistivity", "T,1,F");
  this->_mCommandGetGripperOpticalSensor = new cedar::aux::StringParameter(this, "command get gripper optical sensor", "T,1,G");

  this->registerCommandHook(GRIPPER_POSITION, boost::bind(&cedar::dev::kteam::khepera::GripperSerial::applyGripperPosition, this, _1));

  this->registerMeasurementHook(RESISTIVITY, boost::bind(&cedar::dev::kteam::khepera::GripperSerial::measureResistivity, this));
  this->registerMeasurementHook(OPTICAL_SENSOR, boost::bind(&cedar::dev::kteam::khepera::GripperSerial::measureOpticalSensor, this));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::khepera::GripperSerial::applyGripperPosition(cv::Mat openClose)
{
  CEDAR_DEBUG_ASSERT(openClose.type() == CV_32F);
  CEDAR_DEBUG_ASSERT(openClose.rows > 0);
  CEDAR_DEBUG_ASSERT(openClose.cols > 0);
  float value = openClose.at<float>(0, 0);
  this->setGripperPosition(value >= 1.0);
}

cv::Mat cedar::dev::kteam::khepera::GripperSerial::measureResistivity()
{
  cv::Mat measured(1, 1, CV_32F);
  measured.at<float>(0, 0) = static_cast<float>(this->getGripperResistivity());
  return measured;
}

cv::Mat cedar::dev::kteam::khepera::GripperSerial::measureOpticalSensor()
{
  cv::Mat measured(1, 1, CV_32F);
  measured.at<float>(0, 0) = static_cast<float>(this->getGripperOpticalSensor());
  return measured;
}

void cedar::dev::kteam::khepera::GripperSerial::setGripperPosition(bool position)
{
  unsigned int position_value = 0;
  if (!position)
  {
    position_value = 1;
  }

  // create a command string which will set the gripper position
  std::ostringstream command;
  command << _mCommandSetGripperPosition->getValue() << "," << position_value;
  // send the command string
  std::string answer
    = convertToSerialChannel(getChannel())->writeAndReadLocked(command.str());

  // check whether the answer begins with the correct character
  checkSerialCommunicationAnswer(answer, _mCommandSetGripperPosition->getValue());
}

unsigned int cedar::dev::kteam::khepera::GripperSerial::getGripperPosition()
{
  // position of the arm
  unsigned int position;

  // send a command string to the robot to receive the current acceleration values
  std::string answer
    = convertToSerialChannel(getChannel())->writeAndReadLocked(_mCommandGetGripperPosition->getValue());

  // check whether the first character of the answer is correct
  checkSerialCommunicationAnswer(answer, _mCommandGetGripperPosition->getValue(), _mAnswerGetGripperPosition->getValue());

  // create a string stream on the received answer
  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip 'a,' at the beginning of the answer
  answer_stream.ignore(_mAnswerGetGripperPosition->getValue().size() + 1);
  checkStreamValidity(answer_stream, false);

  // read the acceleration along the x-axis
  answer_stream >> position;
  checkStreamValidity(answer_stream, true);

#ifdef DEBUG_VERBOSE
  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received position value of the gripper",
    "cedar::dev::kteam::khepera::GripperSerial",
    "Read gripper position"
  );
#endif // DEBUG_VERBOSE

  return position;
}

unsigned int cedar::dev::kteam::khepera::GripperSerial::getGripperOpticalSensor()
{
  // value of the optical sensor of the gripper
  unsigned int sensor_value;

  // send a command string to the robot to receive the current acceleration values
  std::string answer
    = convertToSerialChannel(getChannel())->writeAndReadLocked
                                            (
                                              std::string(_mCommandGetGripperOpticalSensor->getValue())
                                            );

  // check whether the first character of the answer is correct
  checkSerialCommunicationAnswer(answer, _mCommandGetGripperOpticalSensor->getValue());

  // create a string stream on the received answer
  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip 'a,' at the beginning of the answer
  answer_stream.ignore(2);
  checkStreamValidity(answer_stream, false);

  // read the acceleration along the x-axis
  answer_stream >> sensor_value;
  checkStreamValidity(answer_stream, true);

#ifdef DEBUG_VERBOSE
  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received the value of the optical sensor of the gripper",
    "cedar::dev::kteam::khepera::GripperSerial",
    "Read gripper optical sensor"
  );
#endif // DEBUG_VERBOSE

  return sensor_value;
}

unsigned int cedar::dev::kteam::khepera::GripperSerial::getGripperResistivity()
{
  // value of the resistivity of the gripper
  unsigned int resistivity;

  // send a command string to the robot to receive the current acceleration values
  std::string answer
    = convertToSerialChannel(getChannel())->writeAndReadLocked(_mCommandGetGripperResistivity->getValue());

  // check whether the first character of the answer is correct
  checkSerialCommunicationAnswer(answer, _mCommandGetGripperResistivity->getValue());

  // create a string stream on the received answer
  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip 'a,' at the beginning of the answer
  answer_stream.ignore(2);
  checkStreamValidity(answer_stream, false);

  // read the acceleration along the x-axis
  answer_stream >> resistivity;
  checkStreamValidity(answer_stream, true);

#ifdef DEBUG_VERBOSE
  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received the resistivity of the gripper",
    "cedar::dev::kteam::khepera::GripperSerial",
    "Read gripper resistivity"
  );
#endif // DEBUG_VERBOSE

  return resistivity;
}
