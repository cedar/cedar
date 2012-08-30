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

//void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::createPropertyAndSetting()
//{
//}


//first step
void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::setProperties()
{

  //nothing to do here, only standard values needed
  //which are already set in CameraProperties class

  //disable all firewire related stuff (if firewire is available)

  // in properties


  // in settings

}

//2. step
bool cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::createCaptureDevice()
{
  //

  std::cout << "Create camera with cv::VideoCapture Backend\n"
      << "BusId:" << mpCameraChannel->mpSettings->getBusId() << "\n"
      << "Guid:" << mpCameraChannel->mpSettings->getGuid() << "\n"
      << "ByGuid:" << mpCameraChannel->mpSettings->getByGuid() << "\n"
      << std::endl;

  cv::VideoCapture capture(mpCameraChannel->mpSettings->getBusId());
  if(capture.isOpened())
  {
    mpCameraChannel->mVideoCapture = capture;
    return true;
  }
  return false;
}

//3. step
void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::applySettingsToCamera()
{

}

//4. step
void cedar::dev::sensors::visual::CameraDeviceCvVideoCapture::applyStateToCamera()
{

}
