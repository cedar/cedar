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

    File:        CameraIsoSpeed.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description:  Header for CameraIsoSpeed enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_ISO_SPEED_H
#define CEDAR_DEV_SENSORS_CAMERA_ISO_SPEED_H

// CEDAR INCLUDES
#include "cedar/configuration.h"   // MAKE FIREWIRE OPTIONAL

#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/camera/namespace.h"

// SYSTEM INCLUDES


/*!@brief Enum class for firewire ISO-speed
 *
 * Use this type for the CameraGrabber::setCameraInitIsoSpeed() and getCameraInitIsoSpeed() method
 */
class cedar::dev::sensors::camera::IsoSpeed
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
  static const cedar::dev::sensors::camera::IsoSpeed::TypePtr& typePtr();
  
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::camera::IsoSpeed> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Allow OpenCV backend to set the ISO-speed of the firewire bus.
   *
   *  Set the ISO-speed manually with the CameraGrabber::setCameraIsoSpeed() method.
   *
   *  @remarks
   *  As every setting, this can only be done before the first frame was grabbed.
   */

  static const Id ISO_NOT_SET = UINT_MAX-2;

  /// @brief Set the ISO-speed to 100
  static const Id ISO_100 = 100;
  /// @see ISO_100
  static const Id ISO_200 = 200;
  /// @see ISO_100
  static const Id ISO_400 = 400;
  /// @see ISO_100
  static const Id ISO_800 = 800;
  /// @see ISO_100
  static const Id ISO_1600 = 1600;
  /// @see ISO_100
  static const Id ISO_3200 = 3200;

    
protected:
  // none yet
private:
  // none yet

}; // cedar::dev::sensors::camera::IsoSpeed

#endif // CEDAR_DEV_SENSORS_CAMERA_ISO_SPEED_H

