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

    File:        ArmSerial.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 07 30

    Description: Source file for the class cedar::dev::kteam::khepera::ArmSerial.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/kteam/khepera/ArmSerial.h"
#include "cedar/devices/kteam/serialChannelHelperFunctions.h"
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered()
  {
    cedar::dev::ComponentManagerSingleton::getInstance()->
        registerType<cedar::dev::kteam::khepera::ArmSerialPtr>();
    return true;
  }

  bool registerFnCall = registered();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::kteam::khepera::ArmSerial::ArmSerial()
{
  this->_mCommandSetArmPosition = new cedar::aux::StringParameter(this, "command set arm position", "T,1,E");
  this->_mCommandGetArmPosition = new cedar::aux::StringParameter(this, "command get arm position", "T,1,H,1");
  this->_mAnswerGetArmPosition = new cedar::aux::StringParameter(this, "answer get arm position", "t,1,h");

  this->registerCommandHook(POSITION, boost::bind(&cedar::dev::kteam::khepera::ArmSerial::applyArmPosition, this, _1));
  this->registerMeasurementHook(POSITION, boost::bind(&cedar::dev::kteam::khepera::ArmSerial::measureArmPosition, this));
}

cedar::dev::kteam::khepera::ArmSerial::~ArmSerial()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::khepera::ArmSerial::applyArmPosition(cv::Mat angle)
{
  CEDAR_DEBUG_ASSERT(angle.type() == CV_32F);
  CEDAR_DEBUG_ASSERT(angle.rows > 0);
  CEDAR_DEBUG_ASSERT(angle.cols > 0);
  float value = angle.at<float>(0, 0);
  this->setArmPosition(static_cast<unsigned int>(180.0 * value / cedar::aux::math::pi));
}

cv::Mat cedar::dev::kteam::khepera::ArmSerial::measureArmPosition()
{
  return cv::Mat(1, 1, CV_32F, static_cast<float>(this->getArmPosition()) / 180.0 * cedar::aux::math::pi);
}

void cedar::dev::kteam::khepera::ArmSerial::setArmPosition(unsigned int position)
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

unsigned int cedar::dev::kteam::khepera::ArmSerial::getArmPosition()
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
