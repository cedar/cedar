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

    File:        CameraSettings.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::visual::CameraSettings class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_CHANNEL_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/devices/sensors/visual/camera/backends/CameraBackendType.h"
#include "cedar/devices/sensors/visual/camera/enums/DeBayerFilter.h"
#include "cedar/devices/sensors/visual/Grabber.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>


/*! @struct CameraChannel
 *  @brief Additional data of a camera channel
 */
struct cedar::dev::sensors::visual::CameraChannel
:
cedar::dev::sensors::visual::Grabber::Channel
{
public:
  CameraChannel
  (
    unsigned int cameraId = 0,
    bool byGuid = false,
    cedar::dev::sensors::visual::CameraBackendType::Id backendType
      = cedar::dev::sensors::visual::CameraBackendType::AUTO,
    cedar::dev::sensors::visual::DeBayerFilter::Id debayerFilter
      = cedar::dev::sensors::visual::DeBayerFilter::NONE
  )
  :
  cedar::dev::sensors::visual::Grabber::Channel(),
  _mpBackendType(new cedar::aux::EnumParameter
                (
                  this,
                  "backend type",
                  cedar::dev::sensors::visual::CameraBackendType::typePtr(),
                  backendType
                )
               ),
  _mpDebayerFilter(new cedar::aux::EnumParameter
                         (
                           this,
                           "decoding",
                           cedar::dev::sensors::visual::DeBayerFilter::typePtr(),
                           debayerFilter
                         )
                       ),
  mpVideoCaptureLock(new QReadWriteLock())
  {
    // create settings
    mpSettings = cedar::dev::sensors::visual::CameraSettingsPtr
                 (
                   new cedar::dev::sensors::visual::CameraSettings(cameraId,byGuid,backendType)
                 );

    // add settings as configurable child
    this->addConfigurableChild("settings", mpSettings);

    // create properties
    mpProperties = cedar::dev::sensors::visual::CameraPropertiesPtr
                 (
                   new cedar::dev::sensors::visual::CameraProperties(this,mVideoCapture,mpVideoCaptureLock)
                 );
  }

  /// Destructor
  ~CameraChannel()
  {
    delete mpVideoCaptureLock;
  }

  /// The backend to use
  cedar::aux::EnumParameterPtr _mpBackendType;

  /// Set the the camera bayer pattern filter (if any is needed)
  cedar::aux::EnumParameterPtr _mpDebayerFilter;

  /*!@brief The lock for the concurrent access to the cv::VideoCapture.
   *
   *   Used in the CameraGrabber itself.
   *   Do not mix up with the capture lock from the CameraGrabber, which will be used to lock the grabbed images
   *   not the VideoCapture class
   */
  QReadWriteLock* mpVideoCaptureLock;

  /// Class for all settings
  cedar::dev::sensors::visual::CameraSettingsPtr mpSettings;

  /// Class for all properties
  cedar::dev::sensors::visual::CameraPropertiesPtr mpProperties;

  /// Camera interface
  cv::VideoCapture mVideoCapture;

};



#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_CHANNEL_H

