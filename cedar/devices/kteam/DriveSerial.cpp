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

    File:        DriveSerial.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Add description.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/devices/kteam/DriveSerial.h"
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/devices/kteam/serialChannelHelperFunctions.h"
#include "cedar/units/Frequency.h"

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
    cedar::dev::ComponentManagerSingleton::getInstance()->registerType<cedar::dev::kteam::DriveSerialPtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::DriveSerial::DriveSerial()
:
_mCommandSetSpeed(new cedar::aux::StringParameter(this, "command set speed", "D")),
_mCommandSetEncoder(new cedar::aux::StringParameter(this, "command set encoder", "P")),
_mCommandGetEncoder(new cedar::aux::StringParameter(this, "command get encoder", "Q"))
{
  this->init();
}

cedar::dev::kteam::DriveSerial::DriveSerial(cedar::dev::kteam::SerialChannelPtr channel)
:
cedar::dev::kteam::Drive(cedar::aux::asserted_pointer_cast<cedar::dev::Channel>(channel)),
_mCommandSetSpeed(new cedar::aux::StringParameter(this, "command set speed", "D")),
_mCommandSetEncoder(new cedar::aux::StringParameter(this, "command set encoder", "P")),
_mCommandGetEncoder(new cedar::aux::StringParameter(this, "command get encoder", "Q"))
{
  this->init();
}

void cedar::dev::kteam::DriveSerial::init()
{
  this->registerCommandHook(cedar::dev::DifferentialDrive::WHEEL_SPEED, boost::bind(&cedar::dev::kteam::DriveSerial::sendMovementCommand, this));
  this->registerMeasurementHook(cedar::dev::kteam::Drive::ENCODERS, boost::bind(&cedar::dev::kteam::DriveSerial::retrieveEncoders, this));
}

cedar::dev::kteam::DriveSerial::~DriveSerial()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::DriveSerial::sendMovementCommand()
{
  // the speed has be thresholded based on the maximum possible number
  // of pulses per second (this is hardware-specific).
  // first: convert speed from m/s into Pulses/s ...
  std::vector<cedar::unit::Frequency> wheel_speed_pulses = convertWheelSpeedToPulses(getWheelSpeed());


  // construct the command string "D,x,y"
  // where x is the speed of the left wheel (in pulses/s)
  // and y is the speed of the right wheel (in pulses/s)
  std::ostringstream command;
  command << _mCommandSetSpeed->getValue()
          << ","
          << static_cast<int>(wheel_speed_pulses[0] / cedar::unit::DEFAULT_FREQUENCY_UNIT)
          << ","
          << static_cast<int>(wheel_speed_pulses[1] / cedar::unit::DEFAULT_FREQUENCY_UNIT);

  // wait for an answer
  std::string answer = convertToSerialChannel(getChannel())->writeAndReadLocked(command.str());

  checkSerialCommunicationAnswer(answer, _mCommandSetSpeed->getValue());
}

cv::Mat cedar::dev::kteam::DriveSerial::retrieveEncoders() const
{
  // the left and right encoder value will be saved in this vector
  cv::Mat encoders = cv::Mat(2, 1, CV_32F);

  // send the command to receive the values of the encoders
  std::string answer
    = convertToSerialChannel(getChannel())->writeAndReadLocked(_mCommandGetEncoder->getValue());

  // check whether the answer begins with the correct character
  checkSerialCommunicationAnswer(answer, _mCommandGetEncoder->getValue());

  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip the answer characters (e.g., 'q,') at the beginning
  answer_stream.ignore(2);
  checkStreamValidity(answer_stream, false);

  // read the left encoder value
  answer_stream >> encoders.at<float>(0,0);
  checkStreamValidity(answer_stream, false);

  // skip the colon separating the encoder values
  answer_stream.ignore(1);
  checkStreamValidity(answer_stream, false);

  // read the right encoder value
  answer_stream >> encoders.at<float>(1,0);
  checkStreamValidity(answer_stream, true);

  return encoders;
}

void cedar::dev::kteam::DriveSerial::sendEncoders(const cv::Mat &mat)
{
  // create a command string which will set the encoder values
  std::ostringstream command;
  command << _mCommandSetEncoder->getValue() << "," << mat.at<float>(0,0) << "," << mat.at<float>(1,0);

  std::string answer = convertToSerialChannel(getChannel())->writeAndReadLocked(command.str());

  // check whether the answer begins with the correct character
  checkSerialCommunicationAnswer(answer, _mCommandSetEncoder->getValue());
}

