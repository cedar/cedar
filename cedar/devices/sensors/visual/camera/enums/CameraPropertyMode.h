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

    File:        CameraPropertyMode.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 08 7

    Description:  Header for CameraPropertyMode enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTYMODE_H
#define CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTYMODE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/visual/namespace.h"

// SYSTEM INCLUDES
//#include <opencv2/highgui/highgui_c.h>

class cedar::dev::sensors::visual::CameraPropertyMode
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
//!@cond SKIPPED_DOCUMENTATION
public:
  typedef cedar::aux::EnumId Id;
public:
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  static void construct();

  static const cedar::aux::EnumBase& type();
  static const cedar::dev::sensors::visual::CameraPropertyMode::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::visual::CameraPropertyMode> mType;
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

  ///@brief Set the operational mode of a camera property. In this case, the backend deceide which mode to use
  static const Id BACKEND_DEFAULT = 0;

  ///@brief Set the operational mode of a camera property to auto. The property must be auto-capable
  static const Id AUTO = 1;

  ///@brief Set the operational mode of a camera property to manual settings. The property must be manual-capable
  static const Id MANUAL = 2;

  //firewire mode "one-push" and "on/off" are not supported from cedar until now
#ifdef CEDAR_USE_LIB_DC1394
  ///@brief Set the operational mode of a camera property to switch on or off. The property must be on/off-capable
  //static const Id ON_OFF = 3;

  //@brief Set the operational mode to one_push
  //static const Id ONE_PUSH = 4;
#endif


protected:
  // none yet
private:
  // none yet


}; // cedar::dev::sensors::visual::CameraPropertyMode

#endif // CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTYMODE_H

