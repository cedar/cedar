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

    File:        CameraBackendType.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::visual::CameraDevice class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/backends/CameraDevice.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraDevice::CameraDevice
(
 cedar::dev::sensors::visual::CameraChannelPtr pCameraChannel
)
:
mpCameraChannel(pCameraChannel)
{
}

cedar::dev::sensors::visual::CameraDevice::~CameraDevice()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::dev::sensors::visual::CameraDevice::init()
{
  bool result = true;

  // 1. lock
  this->mpCameraChannel->mpVideoCaptureLock->lockForWrite();

  // 2. close old videoCapture device
  this->mpCameraChannel->mVideoCapture = cv::VideoCapture();

  // 3  fill p_capabilities with the right values (depends on backend and camera)
  this->fillCapabilities();

  // 4. create cv::VideoCapture
  result = this->createCaptureDevice();

  // 4.1   apply settings from p_settings structure
  this->applySettingsToCamera();

  // 4.2   restore state of the device with the values in p_state
  this->applyStateToCamera();

  // 5. unlock
  this->mpCameraChannel->mpVideoCaptureLock->unlock();

  // 6. done
  return result;
}
