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
cedar::dev::sensors::visual::CameraDeviceCvVideoCaputre::CameraDeviceCvVideoCaputre
(
  cedar::dev::sensors::visual::CameraCapabilitiesPtr p_capabilities,
  cedar::dev::sensors::visual::CameraSettingsPtr p_settings,
  cedar::dev::sensors::visual::CameraStatePtr p_state,
  cv::VideoCapture videoCapture,
  QReadWriteLock* p_videoCaptureLock
)
:
cedar::dev::sensors::visual::CameraDevice::CameraDevice
(
  p_capabilities,
  p_settings,
  p_state,
  videoCapture,
  p_videoCaptureLock
)
{
  //!@todo Implement the basic features with cvVideoCapture backend:
  //
  // 1. lock
  // 2. if capture already there, delete it
  // 3  fill p_capabilities with the right values (depends on backend and camera)
  // 4. create cv::Videocapture
  // 4.1   apply settings from p_settings structure
  // 4.2   restore state of the device with the values in p_state

  // 5. done
}


cedar::dev::sensors::visual::CameraDeviceCvVideoCaputre::~CameraDeviceCvVideoCaputre()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
