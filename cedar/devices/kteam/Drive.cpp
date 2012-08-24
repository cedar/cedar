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

    File:        Drive.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 12

    Description: The differential drive of a PWM-driven robot.

    Credits:     Original design by Andre Bartel (2011)

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/IntLimitsParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/devices/kteam/Drive.h"
#include "cedar/devices/communication/SerialCommunication.h"
#include "cedar/devices/exceptions.h"

// SYSTEM INCLUDES
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::Drive::Drive(cedar::dev::com::SerialCommunicationPtr communication)
:
cedar::dev::robot::DifferentialDrive(),
mSerialCommunication(communication),
_mNumberOfPulsesPerRevolution(new cedar::aux::DoubleParameter(this, "number of pulses per revolution", 0.1, 0.0, 1.0)),
_mEncoderLimits(new cedar::aux::math::IntLimitsParameter(this, "encoder limits", -32768, 0, 0, 32767))
{
  updateDistancePerPulse();

#ifdef DEBUG
  // send a dummy-message
  getSerialCommunication()->lock();
  getSerialCommunication()->send("A");
  std::string answer = getSerialCommunication()->receive();
  getSerialCommunication()->unlock();

  // 'a,' or 'z,' expected, else init failed
  if (answer.size() >= 2 && (answer[0] == 'a' || answer[0] == 'z') && answer[1] == ',')
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Drive: Initialization successful (Answer: '" + answer + "')",
      "cedar::dev::kteam::Drive::initialize()",
      "Drive successfully initialized"
    );
  }
  else
  {
    CEDAR_THROW
    (
      cedar::dev::SerialCommunicationException,
      "Initialization of serial communication failed."
    );
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::Drive::updateDistancePerPulse()
{
  mDistancePerPulse = 2.0 * cedar::aux::math::pi * getWheelRadius() / getNumberOfPulsesPerRevolution();
}

double cedar::dev::kteam::Drive::getNumberOfPulsesPerRevolution() const
{
  return _mNumberOfPulsesPerRevolution->getValue();
}

cedar::aux::math::IntLimitsParameterPtr cedar::dev::kteam::Drive::getEncoderLimits() const
{
  return _mEncoderLimits;
}

double cedar::dev::kteam::Drive::getDistancePerPulse() const
{
  return mDistancePerPulse;
}

char cedar::dev::kteam::Drive::getCommandCharacterSetSpeed() const
{
  return 'D';
}

char cedar::dev::kteam::Drive::getCommandCharacterSetEncoder() const
{
  return 'P';
}

char cedar::dev::kteam::Drive::getCommandCharacterGetEncoder() const
{
  return 'Q';
}

char cedar::dev::kteam::Drive::getCommandCharacterGetAcceleration() const
{
  return 'A';
}

cedar::dev::com::SerialCommunicationPtr cedar::dev::kteam::Drive::getSerialCommunication() const
{
  return mSerialCommunication;
}

void cedar::dev::kteam::Drive::sendMovementCommand()
{
  // the speed has be thresholded based on the maximum possible number
  // of pulses per second (this is hardware-specific).
  // first: convert speed from m/s into Pulses/s ...
  std::vector<int> wheel_speed_pulses(2, 0);
  wheel_speed_pulses[0] = cedar::aux::math::round(this->getWheelSpeed()[0] / mDistancePerPulse);
  wheel_speed_pulses[1] = cedar::aux::math::round(this->getWheelSpeed()[1] / mDistancePerPulse);

  // construct the command string "D,x,y"
  // where x is the speed of the left wheel (in pulses/s)
  // and y is the speed of the right wheel (in pulses/s)
  std::ostringstream command;
  command << getCommandCharacterSetSpeed() << "," << wheel_speed_pulses[0] << "," << wheel_speed_pulses[1];

  getSerialCommunication()->lock();
  // send the command via the serial connection
  getSerialCommunication()->send(command.str());
  // wait for an answer
  std::string answer = getSerialCommunication()->receive();
  getSerialCommunication()->unlock();

  checkAnswer(answer, getCommandCharacterSetSpeed());
}

std::vector<int> cedar::dev::kteam::Drive::getEncoders() const
{
  // the left and right encoder value will be saved in this vector
  std::vector<int> encoders(2);

  // send the command to receive the values of the encoders
  getSerialCommunication()->lock();
  getSerialCommunication()->send(cedar::aux::toString(getCommandCharacterGetEncoder()));
  std::string answer = getSerialCommunication()->receive();
  getSerialCommunication()->unlock();

  // check whether the answer begins with the correct character
  checkAnswer(answer, getCommandCharacterGetEncoder());

  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip the answer characters (e.g., 'q,') at the beginning
  answer_stream.ignore(2);
  checkStream(answer_stream, false);

  // read the left encoder value
  answer_stream >> encoders[0];
  checkStream(answer_stream, false);

  // skip the colon separating the encoder values
  answer_stream.ignore(1);
  checkStream(answer_stream, false);

  // read the right encoder value
  answer_stream >> encoders[1];
  checkStream(answer_stream, true);

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received encoders",
    "cedar::dev::kteam::Drive",
    "Received encoders"
  );

  return encoders;
}

void cedar::dev::kteam::Drive::setEncoders(const std::vector<int>& encoders)
{
  // create a command string which will set the encoder values
  std::ostringstream command;
  command << getCommandCharacterSetEncoder() << "," << encoders[0] << "," << encoders[1];
  // send the command string
  getSerialCommunication()->lock();
  getSerialCommunication()->send(command.str());
  // receive an answer
  std::string answer = getSerialCommunication()->receive();
  getSerialCommunication()->unlock();

  // check whether the answer begins with the correct character
  checkAnswer(answer, getCommandCharacterSetEncoder());

  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully set encoders",
    "cedar::dev::kteam::Drive",
    "Set encoders"
  );
}

void cedar::dev::kteam::Drive::reset()
{
  // stop the robot
  stop();

  // reset the encoder values of the robot to zero
  std::vector<int> encoders(2);
  encoders[0] = 0;
  encoders[1] = 0;
  setEncoders(encoders);
}

void cedar::dev::kteam::Drive::checkAnswer(const std::string& answer, char commandCharacter) const
{
  // determine the correct answer for the given command
  char correct_answer = getSerialCommunication()->determineCorrectAnswer(commandCharacter);

  // if the answer is incorrect ...
  if (answer.empty() || answer[0] != correct_answer)
  {
    // ... throw an exception
    std::string exception_message = "Unexpected answer during serial communication: " + answer;
    CEDAR_THROW
    (
      cedar::dev::SerialCommunicationException,
      exception_message
    );
  }
}

void cedar::dev::kteam::Drive::checkStream(const std::istringstream& answerStream, bool atEndOfStream) const
{
  if (answerStream.fail() || (!atEndOfStream && answerStream.eof()))
  {
    CEDAR_THROW
    (
      cedar::dev::SerialCommunicationException,
      "Error while parsing answer received from robot. Unexpected structure of answer. (Answer: " + answerStream.str() + "\")"
    );
  }
}
