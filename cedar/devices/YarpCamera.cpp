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

 File:        YarpCamera.cpp

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 07 05

 Description: Source file for the class cedar::dev::YarpCamera.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_YARP
// CLASS HEADER
#include "cedar/devices/YarpCamera.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES


namespace {
  bool registered() {
    cedar::dev::ComponentManagerSingleton::getInstance()->registerType<cedar::dev::YarpCameraPtr>();
    return true;
  }

  bool registerFnCall = registered();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::YarpCamera::YarpCamera()
        :
        mReadPort(new cedar::aux::StringParameter(this, "readPortName", "readPortYarpCamera"))
{
  registerMeasurementHook(cedar::dev::Camera::CAMERA_PICTURE,boost::bind(&cedar::dev::YarpCamera::retrievePicture, this));
}

cedar::dev::YarpCamera::~YarpCamera() {
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::YarpCamera::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::dev::Camera::readConfiguration(node);

  this->registerPortNames();
}

void cedar::dev::YarpCamera::registerPortNames() {
  auto yarpChannel = boost::static_pointer_cast<cedar::dev::YarpChannel<cv::Mat> >(this->getChannel());

  if (!yarpChannel) {
    std::cout<<"YarpCamera::postStart: No YarpChannel! Could not read from Port: "<< mReadPort->getValue() << std::endl;
    cedar::aux::LogSingleton::getInstance()->error("Lost yarpChannel pointer", CEDAR_CURRENT_FUNCTION_NAME);
    return;
  } else {
    yarpChannel->addReaderPort(mReadPort->getValue());
  }
}

cv::Mat cedar::dev::YarpCamera::retrievePicture() {
  auto yarpChannel = boost::static_pointer_cast<cedar::dev::YarpChannel<cv::Mat> >(this->getChannel());
  if (!yarpChannel) {
    std::cout<<"YarpCamera::retrievePicture: No YarpChannel! Could not read from Port: "<< mReadPort->getValue() << std::endl;
    cedar::aux::LogSingleton::getInstance()->error("Lost yarpChannel pointer", CEDAR_CURRENT_FUNCTION_NAME);
    return cv::Mat();
  }
    auto picture = yarpChannel->read(mReadPort->getValue());
    return picture;
}

#endif //CEDAR_USE_YARP
