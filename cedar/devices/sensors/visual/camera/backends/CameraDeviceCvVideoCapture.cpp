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

    File:        CameraDeviceCvVideoCapture.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::visual::CameraDeviceCvVideoCapture class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/backends/CameraDeviceCvVideoCapture.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::CameraDeviceCvVideoCapture
(
  cedar::dev::sensors::visual::CameraGrabber* pCameraGrabber,
  cedar::dev::sensors::visual::CameraChannelPtr pCameraChannel
)
:
cedar::dev::sensors::visual::CameraDevice::CameraDevice(pCameraGrabber,pCameraChannel)
{
}


cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::~CameraDeviceCvVideoCapture()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

// void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::createPropertyAndSetting()
//{
//}


// first step
void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::setProperties()
{

  // nothing to do here, only standard values needed
  // which are already set in CameraProperties class

  // disable all firewire related stuff (if firewire is available)

  // in properties


  // in settings

}

// 2. step
bool cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::createCaptureDevice()
{

//  std::cout << "Create camera with cv::VideoCapture Backend\n"
//      << "BusId:" << mpCameraChannel->mpSettings->getBusId() << "\n"
//      << "Guid:" << mpCameraChannel->mpSettings->getGuid() << "\n"
//      << "ByGuid:" << mpCameraChannel->mpSettings->getByGuid() << "\n"
//      << std::endl;

  cv::VideoCapture capture(mpCameraChannel->mpSettings->getCameraId());
  if(capture.isOpened())
  {
    mpCameraChannel->mVideoCapture = capture;
    return true;
  }
  return false;
}

// 3. step
void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::applySettingsToCamera()
{
  //only the video mode could be set in cv::Videocapture backend

  std::cout << "applySettingsToCamera" << std::endl;

  //get the mode and check if it is set manually
  cedar::dev::sensors::visual::CameraVideoMode::Id video_mode_id = mpCameraChannel->mpSettings->getVideoMode();
  if (video_mode_id != cedar::dev::sensors::visual::CameraVideoMode::MODE_NOT_SET)
  {
    double value = static_cast<double>(video_mode_id);
    this->setPropertyToCamera(cedar::dev::sensors::visual::CameraSetting::MODE,value);
  }

  std::cout << "applySettingsToCamera finished" << std::endl;
}

// 4. step
void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::applyStateToCamera()
{
  std::cout << "applyStateToCamera" << std::endl;
  int num_properties = cedar::dev::sensors::visual::CameraProperty::type().list().size();
  for (int i=0; i<num_properties; i++)
  {
    cedar::dev::sensors::visual::CameraProperty::Id prop_id
      = cedar::dev::sensors::visual::CameraProperty::type().list().at(i).id();

    // get the value from the configuration file or from the parameters
    double value = this->mpCameraChannel->mpProperties->getProperty(prop_id);

    // get property-mode, the real value set depends on the mode!
    cedar::dev::sensors::visual::CameraPropertyMode::Id prop_mode_id;
    prop_mode_id = this->mpCameraChannel->mpProperties->getMode(prop_id);

    switch (prop_mode_id)
    {
    case cedar::dev::sensors::visual::CameraPropertyMode::MANUAL:
      this->setPropertyToCamera(prop_id,value);
      break;

    // if auto: set to auto and get value form camera
    case cedar::dev::sensors::visual::CameraPropertyMode::AUTO:
      this->setPropertyToCamera(prop_id,CAMERA_PROPERTY_MODE_AUTO);

    // if default and on mode auto: get value from camera and disable the value field
    default:  //BACKEND_DEFAULT

      this->mpCameraChannel->mpProperties->setProperty(prop_id,this->getPropertyFromCamera(prop_id));
    }
  }
  std::cout << "applyStateToCamera finished" << std::endl;
}
