/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the drive of the E-Puck, a differential drive mobile robot.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/kteam/EPuckDrive.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::EPuckDrive::EPuckDrive
(
  cedar::dev::com::SerialCommunication* peCommunication
)
{
  mInitialized = false;
  // set the e-puck specific default values
  _mWheelDistance->setDefault(0.053);
  _mWheelDistance->makeDefault();
  _mWheelRadius->setDefault(0.0205);
  _mWheelRadius->makeDefault();
  _mPulsesPerRevolution->setDefault(1000);
  _mPulsesPerRevolution->makeDefault();
  _mMaximalEncoderValue->setDefault(32767);
  _mMaximalEncoderValue->makeDefault();
  _mMinimalEncoderValue->setDefault(-32768);
  _mMinimalEncoderValue->makeDefault();
  _mMaximalNumberPulsesPerSecond->setDefault(1000);
  _mMaximalNumberPulsesPerSecond->makeDefault();
  init(peCommunication);
}

cedar::dev::kteam::EPuckDrive::~EPuckDrive()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
int cedar::dev::kteam::EPuckDrive::init(cedar::dev::com::SerialCommunication* peCommunication)
{
  if(mInitialized)
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Initialization failed (Already initialized)\n";
    }
    return 0;
  }

  if (!(peCommunication->isInitialized())) //Communication not initialized
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Initialization failed (Given communication not initialized)\n";
    }
    return 0;
  }

  // initialization of members
  mpeCommunication = 0;
  std::string dummy_answer = ""; //answer for testing the communication

  // calculate distance per pulse and maximal wheel speed
  mpeCommunication = peCommunication;

  // send a dummy-message
  mpeCommunication->lock();
  mpeCommunication->send("A");
  mpeCommunication->receive(dummy_answer);
  mpeCommunication->unlock();

  // 'a,' or 'z,' expected, else init failed
  if ((dummy_answer[0] == 'a' || dummy_answer[0] == 'z') && dummy_answer[1] == ',')
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Initialization successful (Answer: '" << dummy_answer << "')\n";
    }
    mInitialized = true;
    return 1;
  }
  else
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Initialization failed\n";
    }
    return 0;
  }
}

void cedar::dev::kteam::EPuckDrive::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::dev::kteam::Drive::readConfiguration(node);

  if (this->getWheelDistance() <= 0)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Wheel distance (" + cedar::aux::toString(this->getWheelDistance()) + ") <= 0. EPuck initialization failed.",
      "cedar::dev::kteam::EPuckDrive::readConfiguration"
    );
    return;
  }

  if (this->getWheelRadius() <= 0)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Wheel radius (" + cedar::aux::toString(this->getWheelRadius()) + ") <= 0. EPuck initialization failed.",
      "cedar::dev::kteam::EPuckDrive::readConfiguration"
    );
    return;
  }

  if (this->getPulsesPerRevolution() <= 0)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Pulses Per Revolution (" + cedar::aux::toString(this->getPulsesPerRevolution()) + ") <= 0. EPuck initialization failed.",
      "cedar::dev::kteam::EPuckDrive::readConfiguration"
    );
    return;
  }


  if (this->getMaximalNumberPulsesPerSecond() <= 0)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Maximal Number Pulses Per Second (" + cedar::aux::toString(this->getMaximalNumberPulsesPerSecond()) + ") <= 0. EPuck initialization failed.",
      "cedar::dev::kteam::EPuckDrive::readConfiguration"
    );
    return;
  }

  mDistancePerPulse = 2 * cedar::aux::math::pi * this->getWheelRadius() / this->getPulsesPerRevolution();
  mMaximalWheelSpeed = this->getMaximalNumberPulsesPerSecond() * mDistancePerPulse;
}

bool cedar::dev::kteam::EPuckDrive::isInitialized() const
{
  return mInitialized;
}

int cedar::dev::kteam::EPuckDrive::getEncoder(int &leftEncoder, int &rightEncoder)
{
  if (!mInitialized)
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Getting Encoder (Drive not initialized)\n";
    }
    return 0;
  }

  std::string answer;
  std::istringstream answer_stream;

  // send command 'Q', the E-Puck command for readEncoders
  mpeCommunication->lock();
  mpeCommunication->send("Q");
  int s = mpeCommunication->receive(answer);
  mpeCommunication->unlock();

 if (s == 0) // communication failed
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Getting Encoder (Communication Failed)\n";
    }
    return 0;
  }
  else if (answer.find("q,") != 0) //unexpected answer of the robot, does not begin with 'q,'
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Getting Encoder (Unexpected Answer: '" << answer << "')\n";
    }
    return 0;
  }
  else //expected answer
  {
    answer_stream.str(answer);
    answer_stream.ignore(2); //skip 'q,' at start
    if(answer_stream.fail() || answer_stream.eof())
    {
      if (this->debug())
      {
        std::cout << "EPuckDrive: Error Getting Encoder (Extracting encoder values failed)\n";
      }
      return 0;
    }
    answer_stream >> leftEncoder; //read in left encoder value
    if(answer_stream.fail() || answer_stream.eof())
    {
      if (this->debug())
      {
        std::cout << "EPuckDrive: Error Getting Encoder (Extracting encoder values failed)\n";
      }
      return 0;
    }
    answer_stream.ignore(1); //skip the colon
    if(answer_stream.fail() || answer_stream.eof())
    {
      if (this->debug())
      {
        std::cout << "EPuckDrive: Error Getting Encoder (Extracting encoder values failed)\n";
      }
      return 0;
    }
    answer_stream >> rightEncoder; //read in right encoder value
    if(answer_stream.fail() || !answer_stream.eof())
    {
      if (this->debug())
      {
        std::cout << "EPuckDrive: Error Getting Encoder (Extracting encoder values failed)\n";
      }
      return 0;
    }

    if (this->debug())
    {
      std::cout << "EPuckDrive: Getting Encoder Successful (Answer: '" << answer << "')\n";
    }

    return 1;
  }
}

int cedar::dev::kteam::EPuckDrive::getAcceleration(int &xAcceleration, int &yAcceleration, int &zAcceleration)
{
  if (!mInitialized)
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Getting Acceleration (Drive Not Initialized)\n";
    }
    return 0;
  }

  std::string answer;
  std::istringstream answer_stream;

  // send command 'A', the E-Puck command for readAccelerator
  mpeCommunication->lock();
  mpeCommunication->send("A");
  int s = mpeCommunication->receive(answer);
  mpeCommunication->unlock();
  if (s == 0) // communication failed
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Getting Acceleration (Communication Failed)\n";
    }
    return 0;
  }
  else if (answer.find("a,") != 0) //unexpected answer of the robot, does not begin with 'a,'
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Getting Acceleration (Unexpected Answer: '" << answer << "')\n";
    }
    return 0;
  }
  else
  {
    answer_stream.str(answer);
    answer_stream.ignore(2); //skip 'a,' at start
    if(answer_stream.fail() || answer_stream.eof())
    {
      if (this->debug())
      {
        std::cout << "EPuckDrive: Error Getting Acceleration (Extracting acceleration values failed)\n";
      }
      return 0;
    }
    answer_stream >> xAcceleration; //read in first acceleration value
    if(answer_stream.fail() || answer_stream.eof())
    {
      if (this->debug())
      {
        std::cout << "EPuckDrive: Error Getting Acceleration (Extracting acceleration values failed)\n";
      }
      return 0;
    }
    answer_stream.ignore(1); //skip the colon
    if(answer_stream.fail() || answer_stream.eof())
    {
      if (this->debug())
      {
        std::cout << "EPuckDrive: Error Getting Acceleration (Extracting acceleration values failed)\n";
      }
      return 0;
    }
    answer_stream >> yAcceleration; //read in second acceleration value
    if(answer_stream.fail() || answer_stream.eof())
    {
      if (this->debug())
      {
        std::cout << "EPuckDrive: Error Getting Acceleration (Extracting acceleration values failed)\n";
      }
      return 0;
    }
    answer_stream.ignore(1); //skip the colon
    if(answer_stream.fail() || answer_stream.eof())
    {
      if (this->debug())
      {
        std::cout << "EPuckDrive: Error Getting Acceleration (Extracting acceleration values failed)\n";
      }
      return 0;
    }
    answer_stream >> zAcceleration; //read in third acceleration value
    if(answer_stream.fail() || !answer_stream.eof())
    {
      if (this->debug())
      {
        std::cout << "EPuckDrive: Error Getting Acceleration (Extracting acceleration values failed)\n";
      }
      return 0;
    }

    if (this->debug())
    {
      std::cout << "EPuckDrive: Getting Acceleration Successful (Answer: '" << answer << "')\n";
    }
    return 1;
  }
}

int cedar::dev::kteam::EPuckDrive::setWheelSpeed(double leftWheelSpeed, double rightWheelSpeed)
{
  if (!mInitialized)
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Setting Wheel Speed (Drive not initialized)\n";
    }
    return 0;
  }

  // round the wheel speeds (the E-Puck only accepts integers)
  // convert from m/s into Pulses/s (which the E-Puck expects)
  leftWheelSpeed = cedar::aux::math::round (leftWheelSpeed / mDistancePerPulse);
  rightWheelSpeed = cedar::aux::math::round (rightWheelSpeed / mDistancePerPulse);

  // check if the speed to set exceeds the maximum speed
  if (leftWheelSpeed > this->getMaximalNumberPulsesPerSecond())
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Warning Setting Wheel Speed (exceeds Maximum Wheel Speed)\n";
    }
    leftWheelSpeed = this->getMaximalNumberPulsesPerSecond();
  }
  if (rightWheelSpeed > this->getMaximalNumberPulsesPerSecond())
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Warning Setting Wheel Speed (exceeds Maximum Wheel Speed)\n";
    }
    rightWheelSpeed = this->getMaximalNumberPulsesPerSecond();
  }
  if (leftWheelSpeed < - this->getMaximalNumberPulsesPerSecond())
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Warning Setting Wheel Speed (exceeds Maximum Wheel Speed)\n";
    }
    leftWheelSpeed = - this->getMaximalNumberPulsesPerSecond();
  }
  if (rightWheelSpeed < - this->getMaximalNumberPulsesPerSecond())
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Warning Setting Wheel Speed (exceeds Maximum Wheel Speed)\n";
    }
    rightWheelSpeed = - this->getMaximalNumberPulsesPerSecond();
  }

  std::ostringstream command;
  std::string answer;

  // send string "D,x,y" with x = left wheel speed and y = right wheel speed
  command << "D," << leftWheelSpeed << "," << rightWheelSpeed;
  mpeCommunication->lock();
  mpeCommunication->send(command.str());
  int s = mpeCommunication->receive(answer);
  mpeCommunication->unlock();
  if (s == 0) // communication failed
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Setting Wheel Speed (communication failed)\n";
    }
    return 0;
  }
  else if (s != 1 || answer[0] !=  'd') //unexpected answer of the robot (not 'd')
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Setting Wheel Speed (unexpected answer: '" << answer << "')\n";
    }
    return 0;
  }
  else
  {
    // store the wheel speed in local variable mWheelSpeed on success
    mWheelSpeed[0] = leftWheelSpeed * mDistancePerPulse;
    mWheelSpeed[1] = rightWheelSpeed * mDistancePerPulse;
    mVelocity = calculateVelocity(mWheelSpeed[0], mWheelSpeed[1]); //update forward velocity and turning rate
    if (this->debug())
    {
      std::cout << "EPuckDrive: Setting Wheel Speed Successful (Answer: '" << answer << "')\n";
    }
    return 1;
  }
}

int cedar::dev::kteam::EPuckDrive::setEncoder(int leftEncoder, int rightEncoder)
{
  if (!mInitialized)
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Setting Encoder (Drive not initialized)\n";
    }
    return 0;
  }

  std::ostringstream command;
  std::string answer;

  // send string "P,x,y" with x = left encoder value and y = right encoder value
  command << "P," << leftEncoder << "," << rightEncoder;
  mpeCommunication->lock();
  mpeCommunication->send(command.str());
  int s = mpeCommunication->receive(answer);
  mpeCommunication->unlock();
  if (s == 0) // communication failed
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Setting Encoder (communication failed)\n";
    }
    return 0;
  }
  else if (s != 1 || answer[0] != 'p') // unexpected answer of the robot (not 'p')
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Error Setting Encoder (unexpected answer: '" << answer << "')\n";
    }
    return 0;
  }
  else
  {
    if (this->debug())
    {
      std::cout << "EPuckDrive: Setting Encoder Successful (Answer: '" << answer << "')\n";
    }
    return 1;
  }
}
