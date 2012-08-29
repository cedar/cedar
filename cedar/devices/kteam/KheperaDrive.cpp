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

    File:        KheperaDrive.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 17

    Description: An object of this class represents the drive of the E-Puck, a differential drive mobile robot.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/UIntLimitsParameter.h"
#include "cedar/devices/kteam/KheperaDrive.h"
#include "cedar/devices/communication/SerialCommunication.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::KheperaDrive::KheperaDrive(cedar::dev::com::SerialCommunicationPtr communication)
:
cedar::dev::kteam::Drive(communication),
_mArmPositionLimits(new cedar::aux::math::UIntLimitsParameter(this, "arm position limits", 0, 255, 0, 255))

{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::KheperaDrive::openGripper()
{
  this->setGripperPosition(true);
}

void cedar::dev::kteam::KheperaDrive::closeGripper()
{
  this->setGripperPosition(false);
}

void cedar::dev::kteam::KheperaDrive::setGripperPosition(bool position)
{
  unsigned int position_value = 0;
  if (!position)
  {
    position_value = 1;
  }

  // create a command string which will set the gripper position
  std::ostringstream command;
  command << getCommandSetGripperPosition() << "," << position_value;
  // send the command string
  std::string answer
    = getSerialCommunication()->sendAndReceiveLocked(command.str());

  // check whether the answer begins with the correct character
  checkAnswer(answer, getCommandSetGripperPosition());

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully set gripper position",
    "cedar::dev::kteam::KheperaDrive",
    "Set gripper position"
  );
}

void cedar::dev::kteam::KheperaDrive::setArmPosition(unsigned int position)
{
  position = _mArmPositionLimits->getValue().limit(position);

  // create a command string which will set the arm position
  std::ostringstream command;
  command << getCommandSetArmPosition() << "," << position;
  // send the command string
  std::string answer
    = getSerialCommunication()->sendAndReceiveLocked(command.str());

  // check whether the answer begins with the correct character
  checkAnswer(answer, getCommandSetArmPosition());

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully set arm position",
    "cedar::dev::kteam::KheperaDrive",
    "Set arm position"
  );
}

unsigned int cedar::dev::kteam::KheperaDrive::getArmPosition()
{
  // position of the arm
  unsigned int position;

  // send a command string to the robot to receive the current acceleration values
  std::string answer
    = getSerialCommunication()->sendAndReceiveLocked(getCommandGetArmPosition());

  // check whether the first character of the answer is correct
  checkAnswer(answer, getCommandGetArmPosition());

  // create a string stream on the received answer
  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip 'a,' at the beginning of the answer
  answer_stream.ignore(2);
  checkStream(answer_stream, false);

  // read the acceleration along the x-axis
  answer_stream >> position;
  checkStream(answer_stream, true);

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received position value of the arm",
    "cedar::dev::kteam::KheperaDrive",
    "Read arm position"
  );

  return position;
}

unsigned int cedar::dev::kteam::KheperaDrive::getGripperPosition()
{
  // position of the arm
  unsigned int position;

  // send a command string to the robot to receive the current acceleration values
  std::string answer
    = getSerialCommunication()->sendAndReceiveLocked(getCommandGetGripperPosition());

  // check whether the first character of the answer is correct
  checkAnswer(answer, getCommandGetGripperPosition());

  // create a string stream on the received answer
  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip 'a,' at the beginning of the answer
  answer_stream.ignore(2);
  checkStream(answer_stream, false);

  // read the acceleration along the x-axis
  answer_stream >> position;
  checkStream(answer_stream, true);

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received position value of the gripper",
    "cedar::dev::kteam::KheperaDrive",
    "Read gripper position"
  );

  return position;
}

unsigned int cedar::dev::kteam::KheperaDrive::getGripperOpticalSensor()
{
  // value of the optical sensor of the gripper
  unsigned int sensor_value;

  // send a command string to the robot to receive the current acceleration values
  std::string answer
    = getSerialCommunication()->sendAndReceiveLocked(std::string(getCommandGetGripperOpticalSensor()));

  // check whether the first character of the answer is correct
  checkAnswer(answer, getCommandGetGripperOpticalSensor());

  // create a string stream on the received answer
  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip 'a,' at the beginning of the answer
  answer_stream.ignore(2);
  checkStream(answer_stream, false);

  // read the acceleration along the x-axis
  answer_stream >> sensor_value;
  checkStream(answer_stream, true);

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received the value of the optical sensor of the gripper",
    "cedar::dev::kteam::KheperaDrive",
    "Read gripper optical sensor"
  );

  return sensor_value;
}

unsigned int cedar::dev::kteam::KheperaDrive::getGripperResistivity()
{
  // value of the resistivity of the gripper
  unsigned int resistivity;

  // send a command string to the robot to receive the current acceleration values
  std::string answer
    = getSerialCommunication()->sendAndReceiveLocked(getCommandGetGripperResistivity());

  // check whether the first character of the answer is correct
  checkAnswer(answer, getCommandGetGripperResistivity());

  // create a string stream on the received answer
  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip 'a,' at the beginning of the answer
  answer_stream.ignore(2);
  checkStream(answer_stream, false);

  // read the acceleration along the x-axis
  answer_stream >> resistivity;
  checkStream(answer_stream, true);

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received the resistivity of the gripper",
    "cedar::dev::kteam::KheperaDrive",
    "Read gripper resistivity"
  );

  return resistivity;
}

void cedar::dev::kteam::KheperaDrive::setLEDState(unsigned int ledId, bool state)
{
  // create a command string which will set LED state
  std::ostringstream command;
  command << getCommandSetLEDState() << "," << ledId << "," << state;
  // send the command string
  std::string answer
    = getSerialCommunication()->sendAndReceiveLocked(command.str());

  // check whether the answer begins with the correct character
  checkAnswer(answer, getCommandSetLEDState());

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully set the state of an LED",
    "cedar::dev::kteam::KheperaDrive",
    "Set LED state"
  );
}

std::vector<unsigned int> cedar::dev::kteam::KheperaDrive::getProximitySensors()
{
  // the left and right encoder value will be saved in this vector
  std::vector<unsigned int> proximity_values(8);

  // send the command to receive the values of the encoders
  std::string answer
    = getSerialCommunication()->sendAndReceiveLocked(getCommandGetProximitySensors());

  // check whether the answer begins with the correct character
  checkAnswer(answer, getCommandGetProximitySensors());

  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip the answer characters (e.g., 'q,') at the beginning
  answer_stream.ignore(2);
  checkStream(answer_stream, false);

  for (unsigned int i = 0; i < proximity_values.size()-1; ++i)
  {
    // read the left encoder value
    answer_stream >> proximity_values[i];
    checkStream(answer_stream, false);

    // skip the colon separating the encoder values
    answer_stream.ignore(1);
    checkStream(answer_stream, false);
  }

  // read the right encoder value
  answer_stream >> proximity_values[proximity_values.size()-1];
  checkStream(answer_stream, true);

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received proximity values",
    "cedar::dev::kteam::KheperaDrive",
    "Received proximity values"
  );

  return proximity_values;
}

std::vector<unsigned int> cedar::dev::kteam::KheperaDrive::getAmbientSensors()
{
  // the left and right encoder value will be saved in this vector
  std::vector<unsigned int> ambient_values(8);

  // send the command to receive the values of the encoders
  std::string answer
    = getSerialCommunication()->sendAndReceiveLocked(getCommandGetAmbientSensors());

  // check whether the answer begins with the correct character
  checkAnswer(answer, getCommandGetAmbientSensors());

  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip the answer characters (e.g., 'q,') at the beginning
  answer_stream.ignore(2);
  checkStream(answer_stream, false);

  for (unsigned int i = 0; i < ambient_values.size()-1; ++i)
  {
    // read the left encoder value
    answer_stream >> ambient_values[i];
    checkStream(answer_stream, false);

    // skip the colon separating the encoder values
    answer_stream.ignore(1);
    checkStream(answer_stream, false);
  }

  // read the right encoder value
  answer_stream >> ambient_values[ambient_values.size()-1];
  checkStream(answer_stream, true);

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received ambient values",
    "cedar::dev::kteam::KheperaDrive",
    "Received ambient values"
  );

  return ambient_values;
}


std::string cedar::dev::kteam::KheperaDrive::getCommandSetGripperPosition() const
{
  return "T,1,D";
}

std::string cedar::dev::kteam::KheperaDrive::getCommandGetGripperPosition() const
{
  return "T,1,H,0";
}

std::string cedar::dev::kteam::KheperaDrive::getCommandSetArmPosition() const
{
  return "T,1,E";
}

std::string cedar::dev::kteam::KheperaDrive::getCommandGetArmPosition() const
{
  return "T,1,H,1";
}

std::string cedar::dev::kteam::KheperaDrive::getCommandGetGripperOpticalSensor() const
{
  return "T,1,G";
}

std::string cedar::dev::kteam::KheperaDrive::getCommandGetGripperResistivity() const
{
  return "T,1,F";
}

std::string cedar::dev::kteam::KheperaDrive::getCommandSetLEDState() const
{
  return "L";
}

std::string cedar::dev::kteam::KheperaDrive::getCommandGetProximitySensors() const
{
  return "N";
}

std::string cedar::dev::kteam::KheperaDrive::getCommandGetAmbientSensors() const
{
  return "O";
}
