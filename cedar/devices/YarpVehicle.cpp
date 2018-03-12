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

    File:        YarpVehicle.cpp

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2016 06 27

    Description: Source file for the class cedar::dev::YarpVehicle.

    Credits:

======================================================================================================================*/


// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP
// CLASS HEADER
#include "cedar/devices/YarpVehicle.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES


namespace
{
  bool registered()
  {
    cedar::dev::ComponentManagerSingleton::getInstance()->registerType<cedar::dev::YarpVehiclePtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::YarpVehicle::YarpVehicle()
:
    mReadPort(new cedar::aux::StringParameter(this, "readPortName", "readPortYarpVehicle")),
    mWritePort(new cedar::aux::StringParameter(this, "writePortName", "writePortYarpVehicle"))
{
  registerCommandHook(cedar::dev::Vehicle::WHEEL_VELOCITIES, boost::bind(&cedar::dev::YarpVehicle::sendVelocities, this, _1));
  registerMeasurementHook(cedar::dev::Vehicle::WHEEL_VELOCITIES, boost::bind(&cedar::dev::YarpVehicle::retrieveVelocities, this));

  this->applyDeviceSideCommandsAs(cedar::dev::YarpVehicle::WHEEL_VELOCITIES);
//  std::cout<<"YarpVehicle Constructor! Meine WheelListe hat folgende Größe: "<< getNumberOfWheels()<<std::endl;
}

cedar::dev::YarpVehicle::~YarpVehicle()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::YarpVehicle::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::dev::Vehicle::readConfiguration(node);

  this->registerPortNames();
}

void cedar::dev::YarpVehicle::registerPortNames()
{
//  std::cout<<"YarpVehicle:postStart : Meine WheelListe hat folgende größe: "<< getNumberOfWheels()<<std::endl;
  auto yarpChannel = boost::static_pointer_cast< cedar::dev::YarpChannel<cv::Mat> >(this->getChannel());
  if (!yarpChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error("Lost yarpChannel pointer", CEDAR_CURRENT_FUNCTION_NAME);
    return;
  }
  else
  {
    yarpChannel->addReaderPort(mReadPort->getValue());
    yarpChannel->addWriterPort(mWritePort->getValue());
  }
}

void cedar::dev::YarpVehicle::sendVelocities(cv::Mat mat)
{
  auto yarpChannel = boost::static_pointer_cast < cedar::dev::YarpChannel<cv::Mat> > (this->getChannel());
  if (!yarpChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error("Lost yarpChannel pointer", CEDAR_CURRENT_FUNCTION_NAME);
    return;
  }
  yarpChannel->write(mat, mWritePort->getValue());
}

cv::Mat cedar::dev::YarpVehicle::retrieveVelocities()
{
  auto yarpChannel = boost::static_pointer_cast < cedar::dev::YarpChannel<cv::Mat> > (this->getChannel());
  if (!yarpChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error("Lost yarpChannel pointer", CEDAR_CURRENT_FUNCTION_NAME);
    return cv::Mat();
  }
  auto ret = yarpChannel->read(mReadPort->getValue());
//  std::cout << "  yarp vehicle: " << ret << std::endl;
  return ret;
}

bool cedar::dev::YarpVehicle::applyCrashbrake()
{
//    cv::Mat::zeros( getNumberOfJoints(), 1, CV_64F );

  return false;
}



#endif //CEDAR_USE_YARP
