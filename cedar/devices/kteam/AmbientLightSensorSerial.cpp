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

    File:        AmbientLightSensorSerial.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 12 03

    Description: Ambient light sensor, which communicates over a serial channel.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/devices/kteam/AmbientLightSensorSerial.h"
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/devices/kteam/serialChannelHelperFunctions.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::kteam::AmbientLightSensorSerial::AmbientLightSensorSerial()
{
}

cedar::dev::kteam::AmbientLightSensorSerial::AmbientLightSensorSerial
(
  cedar::dev::kteam::SerialChannelPtr channel
)
:
cedar::dev::Sensor(cedar::aux::asserted_pointer_cast<cedar::dev::Channel>(channel))
{
}

cedar::dev::kteam::AmbientLightSensorSerial::~AmbientLightSensorSerial()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cv::Mat cedar::dev::kteam::AmbientLightSensorSerial::getData()
{
  // the left and right encoder value will be saved in this vector
  cv::Mat ambient_light_values = cv::Mat::zeros(1, 8, CV_32F);

  // cast the channel into a serial channel
  cedar::dev::kteam::SerialChannelPtr serial_channel
    = convertToSerialChannel(getChannel());

  // send the command to receive the values of the encoders
  std::string answer = serial_channel->sendAndReceiveLocked(_mCommandGetAmbientLight->getValue());

  // check whether the answer begins with the correct character
  checkSerialCommunicationAnswer(answer, _mCommandGetAmbientLight->getValue());

  std::istringstream answer_stream;
  answer_stream.str(answer);

  // skip the answer characters (e.g., 'q,') at the beginning
  answer_stream.ignore(2);
  checkStreamValidity(answer_stream, false);

  for (unsigned int i = 0; i < static_cast<unsigned int>(ambient_light_values.size().height); ++i)
  {
    // read the left encoder value
    answer_stream >> ambient_light_values.at<float>(i);
    checkStreamValidity(answer_stream, false);

    // skip the colon separating the encoder values
    if (i < static_cast<unsigned int>(ambient_light_values.size().height) - 1)
    {
      answer_stream.ignore(1);
      checkStreamValidity(answer_stream, false);
    }
  }

#ifdef DEBUG_VERBOSE
  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received ambient light values",
    "cedar::dev::kteam::AmbientLightSensorSerial",
    "Received ambient light values"
  );
#endif // DEBUG_VERBOSE

  return ambient_light_values;
}
