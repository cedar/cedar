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

    File:        InfraredSensorYarp.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 03 20

    Description: Infrared sensor, which communicates over a yarp channel.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_YARP

// CEDAR INCLUDES
#include "cedar/devices/kteam/InfraredSensorYarp.h"
#include "cedar/devices/YarpChannel.h"
#include "cedar/devices/ComponentSlot.h"
#include "cedar/devices/Robot.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered()
  {
    cedar::dev::ComponentManagerSingleton::getInstance()->
        registerType<cedar::dev::kteam::InfraredSensorYarpPtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::kteam::InfraredSensorYarp::InfraredSensorYarp()
:
mSensorPortWithPrefix(""),
mValues(new cedar::aux::MatData(cv::Mat::zeros(1, 8, CV_32F))),
_mSensorPort(new cedar::aux::StringParameter(this, "sensor port", "proximitySensors"))
{
  //!@todo this has to be fixed
//  this->addMeasuredData("sensor values", mValues, boost::bind(&cedar::dev::kteam::InfraredSensorYarp::updateSensorValues, this));
}

cedar::dev::kteam::InfraredSensorYarp::InfraredSensorYarp
(
  YarpMatChannelPtr channel
)
:
cedar::dev::Sensor(channel),
mSensorPortWithPrefix(""),
mValues(new cedar::aux::MatData(cv::Mat::zeros(1, 8, CV_32F))),
_mSensorPort(new cedar::aux::StringParameter(this, "sensor port", "proximitySensors"))
{
  //!@todo this has to be fixed
//  this->addMeasuredData("sensor values", mValues, boost::bind(&cedar::dev::kteam::InfraredSensorYarp::updateSensorValues, this));
}

cedar::dev::kteam::InfraredSensorYarp::~InfraredSensorYarp()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::InfraredSensorYarp::open()
{
  CEDAR_ASSERT(this->getChannel());
  YarpMatChannelPtr yarp_channel = boost::static_pointer_cast<YarpMatChannel>(this->getChannel());
  std::string robot_name = this->getSlot().lock()->getRobot()->getName() + "/";
  mSensorPortWithPrefix = robot_name + _mSensorPort->getValue();
  yarp_channel->addReaderPort(mSensorPortWithPrefix);
}

cv::Mat cedar::dev::kteam::InfraredSensorYarp::getData()
{
  return mValues->getData();
}

void cedar::dev::kteam::InfraredSensorYarp::updateSensorValues()
{
  // cast the channel into a serial channel
  YarpMatChannelPtr yarp_channel = boost::static_pointer_cast<YarpMatChannel>(this->getChannel());
  cv::Mat measured_values = yarp_channel->read(mSensorPortWithPrefix);
  if (measured_values.rows == 8)
  {
    this->mValues->setData(measured_values);
  }
}

void cedar::dev::kteam::InfraredSensorYarp::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::dev::Sensor::readConfiguration(node);
  this->open();
}

#endif // CEDAR_USE_YARP
