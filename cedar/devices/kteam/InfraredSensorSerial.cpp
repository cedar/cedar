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

    File:        InfraredSensorSerial.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 12 03

    Description: Infrared sensor, which communicates over a serial channel.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/kteam/InfraredSensorSerial.h"
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/devices/kteam/serialChannelHelperFunctions.h"
#include "cedar/devices/ComponentDeclaration.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>


//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::Component::ComponentDataType cedar::dev::kteam::InfraredSensorSerial::M_IR_VALUES_ID = 0;

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered()
  {
    cedar::dev::ComponentDeclarationPtr declaration
    (
      new cedar::dev::ComponentDeclarationTemplate<cedar::dev::kteam::InfraredSensorSerial>
      (
        "Sensors"
      )
    );

    declaration->setIconPath(":/cedar/dev/gui/icons/ir_sensor_icon.svg");

    declaration->declare();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::kteam::InfraredSensorSerial::InfraredSensorSerial()
:
_mCommandGetInfrared(new cedar::aux::StringParameter(this, "command get infrared", "N")),
mValues(new cedar::aux::MatData(cv::Mat::zeros(1, 8, CV_32F)))
{
  this->installMeasurementType(M_IR_VALUES_ID, "IR values");
  this->setMeasurementDimensionality(M_IR_VALUES_ID, 8);
  this->registerMeasurementHook(M_IR_VALUES_ID, boost::bind(&cedar::dev::kteam::InfraredSensorSerial::updateIrValues, this));
//  this->addMeasuredData("proximity", mValues, boost::bind(&cedar::dev::kteam::InfraredSensorSerial::updateIrValues, this));
}

cedar::dev::kteam::InfraredSensorSerial::~InfraredSensorSerial()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cv::Mat cedar::dev::kteam::InfraredSensorSerial::getData()
{
  return mValues->getData();
}

cv::Mat cedar::dev::kteam::InfraredSensorSerial::updateIrValues()
{
  // the left and right encoder value will be saved in this vector
  //!@todo This allocates a new matrix every time the values are retrieved -- not a problem here due to the small size, but what about other cases? (e.g., camera image)
  cv::Mat infrared_values = cv::Mat::zeros(8, 1, CV_32F);

  // cast the channel into a serial channel
  cedar::dev::kteam::SerialChannelPtr serial_channel = convertToSerialChannel(getChannel());

  // send the command to receive the values of the encoders
  std::string answer = serial_channel->writeAndReadLocked(_mCommandGetInfrared->getValue());

  // check whether the answer begins with the correct character
  checkSerialCommunicationAnswer(answer, _mCommandGetInfrared->getValue());

  std::vector<std::string> parts;
  cedar::aux::split(answer, ",", parts);

  // should be ['n', 'number1', ..., 'numberN'] where N = size of infrared_values
  CEDAR_ASSERT(parts.size() == cedar::aux::math::get1DMatrixSize(infrared_values) + 1);

  for (unsigned int i = 0; i < cedar::aux::math::get1DMatrixSize(infrared_values); ++i)
  {
    const std::string& value = parts[i + 1]; // +1 to skip the command/reply string
    infrared_values.at<float>(i) = cedar::aux::fromString<float>(value);
  }

#ifdef DEBUG_VERBOSE
  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully received infrared values",
    "cedar::dev::kteam::InfraredSensorSerial",
    "Received infrared values"
  );
#endif // DEBUG_VERBOSE

  return infrared_values;
}
