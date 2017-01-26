/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTY_MODE_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTY_MODE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/camera/namespace.h"

// SYSTEM INCLUDES
//#include <opencv2/highgui/highgui_c.h>

/*! @brief The mode of a camera property
 *
 *  This class encapsulates the mode of a specific Property. The property itself could be set to auto or to manual.
 *  By default, the backend sets the property-value to a default value. This state is mapped to the third possible
 *  value: BackendDefault
 *
 */
class cedar::dev::sensors::camera::PropertyMode
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Typedef for the enum values
  typedef cedar::aux::EnumId Id;
public:
  //! Typedef for this enum as shared pointer
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Construct
  static void construct();

  //! Type
  static const cedar::aux::EnumBase& type();

  //! Type Pointer
  static const cedar::dev::sensors::camera::PropertyMode::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //! Storage
  static cedar::aux::EnumType<cedar::dev::sensors::camera::PropertyMode> mType;

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


}; // cedar::dev::sensors::camera::PropertyMode

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTY_MODE_H

