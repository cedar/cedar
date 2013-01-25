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

    File:        DeviceCvVideoCapture.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::camera::DeviceCvVideoCapture class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/backends/DeviceCvVideoCapture.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::DeviceCvVideoCapture::DeviceCvVideoCapture
(
  cedar::dev::sensors::camera::Channel* pCameraChannel
)
:
cedar::dev::sensors::camera::Device(pCameraChannel)
{
}


cedar::dev::sensors::camera::DeviceCvVideoCapture::~DeviceCvVideoCapture()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

// void cedar::dev::sensors::camera::DeviceCvVideoCapture::createPropertyAndSetting()
//{
//}


bool cedar::dev::sensors::camera::DeviceCvVideoCapture::createCaptureObject()
{

//  std::cout << "Create camera with cv::VideoCapture Backend\n"
//      << "BusId:" << mpCameraChannel->getBusId() << "\n"
//      << "Guid:" << mpCameraChannel->getGuid() << "\n"
//      << "ByGuid:" << mpCameraChannel->getByGuid() << "\n"
//      << std::endl;

  cv::VideoCapture capture(mpCameraChannel->getCameraId());
  if(capture.isOpened())
  {
    mpCameraChannel->mVideoCapture = capture;
    return true;
  }
  return false;
}


void cedar::dev::sensors::camera::DeviceCvVideoCapture::getAvailablePropertiesFromCamera()
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;

  int num_properties = cedar::dev::sensors::camera::Property::type().list().size();
  for (int i=0; i<num_properties; i++)
  {
    cedar::dev::sensors::camera::Property::Id prop_id
      = cedar::dev::sensors::camera::Property::type().list().at(i).id();

    //try to get the value from the camera
    double value = this->getPropertyFromCamera(prop_id);

    if (value == CAMERA_PROPERTY_NOT_SUPPORTED )
    {
      mpCameraChannel->mpProperties->disableProperty(prop_id);
    }
    else
    {
      mpCameraChannel->mpProperties->enableProperty(prop_id);
    }
  }
}

void cedar::dev::sensors::camera::DeviceCvVideoCapture::applySettingsToCamera()
{
  //only the video mode could be set in cv::Videocapture backend

  //std::cout << "applySettingsToCamera" << std::endl;

  //get the mode and check if it is set manually
  cedar::dev::sensors::camera::VideoMode::Id video_mode_id = mpCameraChannel->getVideoMode();
  if (video_mode_id != cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET)
  {
    double value = static_cast<double>(video_mode_id);
    this->setPropertyToCamera(cedar::dev::sensors::camera::Setting::MODE,value);
  }

  //std::cout << "applySettingsToCamera finished" << std::endl;
}
