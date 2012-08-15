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
#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTINGS_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTINGS_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraSetting.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/Configurable.h"

#include "cedar/devices/sensors/visual/camera/enums/CameraBackendType.h"


// SYSTEM INCLUDES


/*!@brief Base class of the misc camera grabber backends.
 *
 * Implements the common features of a camera device
 */
class cedar::dev::sensors::visual::CameraSettings
:
public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /// The standard constructor.
  CameraSettings(cedar::dev::sensors::visual::CameraBackendType::Id backendType);

  /// Destructor
  ~CameraSettings();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
//  cedar::dev::sensors::visual::CameraSettingsSet& getSettings();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /// Set a Parameter in the cv::VideoCapture class
  bool setSetting(CameraSetting::Id settingId, double value);

  /// Get a Parameter in the cv::VideoCapture class
  double getSetting(CameraSetting::Id settingId);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
//  cedar::dev::sensors::visual::CameraSettingsSet mSettings;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
  
  /// frame width
  cedar::aux::UIntParameterPtr mpWidth;

  /// frame height
  cedar::aux::UIntParameterPtr mpHeight;

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

