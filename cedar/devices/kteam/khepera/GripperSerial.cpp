/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/math/UIntLimitsParameter.h"
#include "cedar/devices/kteam/khepera/GripperSerial.h"
#include "cedar/devices/kteam/serialChannelHelperFunctions.h"
#include "cedar/devices/kteam/SerialChannel.h"

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
:
_mCommandSetGripperPosition(new cedar::aux::StringParameter(this, "command set gripper position", "T,1,D")),
_mCommandGetGripperPosition(new cedar::aux::StringParameter(this, "command get gripper position", "T,1,H,0")),
_mAnswerGetGripperPosition(new cedar::aux::StringParameter(this, "answer get gripper position", "t,1,h")),
_mCommandSetArmPosition(new cedar::aux::StringParameter(this, "command set arm position", "T,1,E")),
_mCommandGetArmPosition(new cedar::aux::StringParameter(this, "command get arm position", "T,1,H,1")),
_mAnswerGetArmPosition(new cedar::aux::StringParameter(this, "answer get arm position", "t,1,h")),
_mCommandGetGripperResistivity(new cedar::aux::StringParameter(this, "command get gripper resistivity", "T,1,F")),
_mCommandGetGripperOpticalSensor
(
  new cedar::aux::StringParameter(this, "command get gripper optical sensor", "T,1,G")
)
{
}

// constructor taking an externally created channel
cedar::dev::kteam::khepera::GripperSerial::GripperSerial(cedar::dev::kteam::SerialChannelPtr channel)
:
cedar::dev::kteam::khepera::Gripper(cedar::aux::asserted_pointer_cast<cedar::dev::Channel>(channel)),
_mCommandSetGripperPosition(new cedar::aux::StringParameter(this, "command set gripper position", "T,1,D")),
_mCommandGetGripperPosition(new cedar::aux::StringParameter(this, "command get gripper position", "T,1,H,0")),
_mAnswerGetGripperPosition(new cedar::aux::StringParameter(this, "answer get gripper position", "t,1,h")),
_mCommandSetArmPosition(new cedar::aux::StringParameter(this, "command set arm position", "T,1,E")),
_mCommandGetArmPosition(new cedar::aux::StringParameter(this, "command get arm position", "T,1,H,1")),
_mAnswerGetArmPosition(new cedar::aux::StringParameter(this, "answer get arm position", "t,1,h")),
_mCommandGetGripperResistivity(new cedar::aux::StringParameter(this, "command get gripper resistivity", "T,1,F")),
_mCommandGetGripperOpticalSensor
(
  new cedar::aux::StringParameter(this, "command get gripper optical sensor", "T,1,G")
)
{
}

cedar::dev::kteam::khepera::GripperSerial::~GripperSerial()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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

void cedar::dev::kteam::khepera::GripperSerial::setArmPosition(unsigned int position)
{
  position = _mArmPositionLimits->getValue().limit(position);

  // create a command string which will set the arm position
  std::ostringstream command;
  command << _mCommandSetArmPosition->getValue() << "," << position;
  // send the command string
  std::string answer
    = convertToSerialChannel(getChannel())->writeAndReadLocked(command.str());

  // check whether the answer begins with the correct character
  checkSerialCommunicationAnswer(answer, _mCommandSetArmPosition->getValue());
}

unsigned int cedar::dev::kteam::khepera::GripperSerial::getArmPosition()
{
  // position of the arm
  unsigned int position;

  // send a command string to the robot to receive the current acceleration values
  std::string answer
    = convertToSerialChannel(getChannel())->writeAndReadLocked(_mCommandGetArmPosition->getValue());

  // check whether the first character of the answer is correct
  checkSerialCommunicationAnswer(answer, _mCommandGetArmPosition->getValue(), _mAnswerGetArmPosition->getValue());

  // create a string stream on the received answer
  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip 'a,' at the beginning of the answer
  answer_stream.ignore(_mAnswerGetArmPosition->getValue().size() + 1);
  checkStreamValidity(answer_stream, false);

  // read the acceleration along the x-axis
  answer_stream >> position;
  checkStreamValidity(answer_stream, true);

#ifdef DEBUG_VERBOSE
  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received position value of the arm",
    "cedar::dev::kteam::khepera::GripperSerial",
    "Read arm position"
  );
#endif // DEBUG_VERBOSE

  return position;
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
