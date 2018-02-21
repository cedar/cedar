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

    File:        BackendCvVideoCapture.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::camera::BackendCvVideoCapture class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/backends/BackendCvVideoCapture.h"
#include "cedar/devices/sensors/camera/exceptions.h"

// SYSTEM INCLUDES
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::camera::BackendCvVideoCapture::BackendCvVideoCapture
(
  cedar::dev::sensors::camera::Channel* pCameraChannel
)
:
cedar::dev::sensors::camera::Backend(pCameraChannel)
{
}


cedar::dev::sensors::camera::BackendCvVideoCapture::~BackendCvVideoCapture()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::camera::BackendCvVideoCapture::init()
{
}


void cedar::dev::sensors::camera::BackendCvVideoCapture::createCaptureObject()
{
  unsigned int cam_id = mpCameraChannel->getCameraId();
  cv::VideoCapture capture(cam_id);
  if (!capture.isOpened())
  {
    std::string msg = "CvVideoCapture-Backend error: Couldn't create capture object for camera with ID "
                      + cedar::aux::toString(cam_id);
    CEDAR_THROW(cedar::dev::sensors::camera::CreateBackendException,msg);
  }
  mpCameraChannel->mVideoCapture = capture;
}


void cedar::dev::sensors::camera::BackendCvVideoCapture::readCameraProperties()
{
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

void cedar::dev::sensors::camera::BackendCvVideoCapture::applySettingsToCamera()
{
  //only the video mode could be set in cv::Videocapture backend

  //get the mode and check if it is set manually
  cedar::dev::sensors::camera::VideoMode::Id video_mode_id = mpCameraChannel->getVideoMode();
  if (video_mode_id != cedar::dev::sensors::camera::VideoMode::MODE_NOT_SET)
  {
    double value = static_cast<double>(video_mode_id);
    this->setPropertyToCamera(cedar::dev::sensors::camera::Setting::MODE,value);
  }
}
