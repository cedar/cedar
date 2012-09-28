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

    File:        CameraSettings.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::visual::CameraSettings class

    Credits:

======================================================================================================================*/
// newfile
#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTINGS_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTINGS_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/Configurable.h"

#include "cedar/devices/sensors/visual/camera/backends/CameraBackendType.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraSetting.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraVideoMode.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraFrameRate.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraIsoSpeed.h"

// SYSTEM INCLUDES
#include <QObject>


/*!@brief Base class of the misc camera grabber backends.
 *
 * Implements the common features of a camera device
 */
class cedar::dev::sensors::visual::CameraSettings
:
public QObject,
public cedar::aux::Configurable
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------

protected slots:

//!@brief A slot that is triggered if a setting has changed
  void settingChanged();


signals:

//!@brief This signal is emitted, when a setting has changed.
// The CameraGrabber class have to create a new grabber with the new settings.
  void settingsChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /// The standard constructor.
  CameraSettings
  (
    unsigned int cameraId = 0,
    bool byGuid = false,
    cedar::dev::sensors::visual::CameraBackendType::Id backendType = cedar::dev::sensors::visual::CameraBackendType::AUTO
  );

  /// Destructor
  ~CameraSettings();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  // bool setSetting(cedar::dev::sensors::visual::CameraSetting::Id settingId, double value);
  // double getSetting(cedar::dev::sensors::visual::CameraSetting::Id settingId);

  unsigned int getCameraId();
  bool getByGuid();

  cedar::dev::sensors::visual::CameraVideoMode::Id getVideoMode();
  cedar::dev::sensors::visual::CameraFrameRate::Id getFPS();

#ifdef CEDAR_USE_LIB_DC1394
  cedar::dev::sensors::visual::CameraIsoSpeed::Id getIsoSpeed();
  void setIsoSpeed( cedar::dev::sensors::visual::CameraIsoSpeed::Id isoSpeed);
#endif // CEDAR_USE_LIB_DC1394

  void setCameraId(unsigned int CameraId, bool isGuid = false);

  void setVideoMode(cedar::dev::sensors::visual::CameraVideoMode::Id videoMode);
  void setFPS(cedar::dev::sensors::visual::CameraFrameRate::Id fps);

  /// Change visible attributes to various parameters of the settings-part
  void setBackendType(cedar::dev::sensors::visual::CameraBackendType::Id backendType);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void hideFwVideoModes();


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::dev::sensors::visual::CameraBackendType::Id mBackendType;
  //unsigned int mCameraId;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  /// create with guid or with bus id
  cedar::aux::BoolParameterPtr _mpByGuid;

  /// Camera-ID. Either the Bus-ID or the GUID (depends on _mpByGuid)
  cedar::aux::UIntParameterPtr _mpCameraId;

  /// framesize as mode
  cedar::aux::EnumParameterPtr _mpGrabMode;

  /// framerate of grabbing
  cedar::aux::EnumParameterPtr _mpFPS;
  
  
#ifdef CEDAR_USE_LIB_DC1394
  /// the iso-speed of the firewire bus
  cedar::aux::EnumParameterPtr _mpIsoSpeed;
#endif


private:
  // none yet

}; // class cedar::dev::sensors::visual::CameraSettings

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTINGS_H

