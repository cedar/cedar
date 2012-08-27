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

    File:        EPuckDrive.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 17

    Description: An object of this class represents the drive of the E-Puck, a differential drive mobile robot.

    Credits:     Original design by Andre Bartel (2011)

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/kteam/EPuckDrive.h"
#include "cedar/devices/communication/SerialCommunication.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::EPuckDrive::EPuckDrive(cedar::dev::com::SerialCommunicationPtr communication)
:
cedar::dev::kteam::Drive(communication)
{
  _mWheelDistance->setDefault(0.053);
  _mWheelDistance->makeDefault();
  _mWheelRadius->setDefault(0.0205);
  _mWheelRadius->makeDefault();
  _mNumberOfPulsesPerRevolution->setDefault(1000);
  _mNumberOfPulsesPerRevolution->makeDefault();
  _mEncoderLimits->setDefaults(-32768, 32767);
  _mEncoderLimits->makeDefault();
  _mHardwareSpeedLimits->setDefaults(-2000, 2000);
  _mHardwareSpeedLimits->makeDefault();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::vector<int> cedar::dev::kteam::EPuckDrive::getAcceleration()
{
  // the acceleration values for the x-, y-, and z-axis will be stored in this vector
  std::vector<int> acceleration(3);

  // send a command string to the robot to receive the current acceleration values
  std::string answer = getSerialCommunication()->sendAndReceiveLocked
                       (
                         cedar::aux::toString(getCommandCharacterGetAcceleration())
                       );

  // check whether the first character of the answer is correct
  checkAnswer(answer, getCommandCharacterGetAcceleration());

  // create a string stream on the received answer
  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip 'a,' at the beginning of the answer
  answer_stream.ignore(2);
  checkStream(answer_stream, false);

  // read the acceleration along the x-axis
  answer_stream >> acceleration[0];
  checkStream(answer_stream, false);

  // skip the colon
  answer_stream.ignore(1);
  checkStream(answer_stream, false);

  // read the acceleration along the y-axis
  answer_stream >> acceleration[1];
  checkStream(answer_stream, false);

  // skip the colon
  answer_stream.ignore(1);
  checkStream(answer_stream, false);

  // read the acceleration along the z-axis
  answer_stream >> acceleration[2];
  checkStream(answer_stream, true);

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received acceleration values",
    "cedar::dev::kteam::EPuckDrive",
    "Read acceleration"
  );

  return acceleration;
}
