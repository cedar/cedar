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

    File:        CameraDevice.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::visual::CameraDevice class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_DEVICE_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_DEVICE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>


/*!@brief Base class of the misc camera grabber backends.
 *
 * Implements the common features of a camera device
 */
class cedar::dev::sensors::visual::CameraDevice
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The standard constructor.
  CameraDevice
  (
    cedar::dev::sensors::visual::CameraCapabilitiesPtr p_capabilities,
    cedar::dev::sensors::visual::CameraSettingsPtr p_settings,
    cedar::dev::sensors::visual::CameraStatePtr p_state,
    cv::VideoCapture videoCapture,
    QReadWriteLock* p_videoCaptureLock
  );

public:
  //!@brief Destructor
  virtual ~CameraDevice();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  cedar::dev::sensors::visual::CameraCapabilitiesPtr mpCapabilities;
  cedar::dev::sensors::visual::CameraSettingsPtr mpSettings;
  cedar::dev::sensors::visual::CameraStatePtr mpState;
  cv::VideoCapture mVideoCapture;
  QReadWriteLock* mpVideoCaptureLock;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::CameraDevice

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_DEVICE_H

