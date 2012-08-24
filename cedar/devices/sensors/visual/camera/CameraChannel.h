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
//newfile
#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_CHANNEL_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraBackendType.h"
#include "cedar/devices/sensors/visual/camera/enums/DeBayerFilter.h"
#include "cedar/devices/sensors/visual/Grabber.h"

// SYSTEM INCLUDES


/*!@brief Base class of the misc camera grabber backends.
 *
 * Implements the common features of a camera device
 */
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
    _mpByGuid(new cedar::aux::BoolParameter(this,"by GUID",false)),
    _mpBusId(new cedar::aux::UIntParameter(this,"bus ID",0,0,255)),
    _mpGuid(new cedar::aux::UIntParameter(this,"GUID",0)),
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
                             "debayer",
                             cedar::dev::sensors::visual::DeBayerFilter::typePtr(),
                             debayerFilter
                           )
                         ),
    mpVideoCaptureLock(new QReadWriteLock())
    //mProperties(cedar::dev::sensors::visual::CameraPropertiesSet())
    {
      if (byGuid)
      {
        _mpBusId->setValue(0);
        _mpBusId->setHidden(true);
        _mpGuid->setValue(cameraId);
      }
      else
      {
        _mpBusId->setValue(cameraId);
        _mpGuid->setValue(0);
        _mpGuid->setHidden(true);
      }
    }

    ~CameraChannel()
    {
      delete mpVideoCaptureLock;
    }

    /// create with guid or with bus id
    cedar::aux::BoolParameterPtr _mpByGuid;

    /// The id on the used bus
    cedar::aux::UIntParameterPtr _mpBusId;

    /// Unique channel id (not supported on every backend)
    cedar::aux::UIntParameterPtr _mpGuid;

    /// The Backend to use
    cedar::aux::EnumParameterPtr _mpBackendType;

    /// Camera needs to apply a bayer pattern filter
    cedar::aux::EnumParameterPtr _mpDebayerFilter;



    /// The lock for the concurrent access to the cv::VideoCapture
    QReadWriteLock* mpVideoCaptureLock;

    // contains all settings
    cedar::dev::sensors::visual::CameraSettingsPtr mpSettings;

    // contains all properties
    cedar::dev::sensors::visual::CameraPropertiesPtr mpProperties;


    /// Camera interface
    cv::VideoCapture mVideoCapture;

    // Pointer to the camera device
    //CameraDevicePtr mpDevice;

  };

  //CEDAR_GENERATE_POINTER_TYPES(CameraChannel);


  //!@endcond



#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_CHANNEL_H

