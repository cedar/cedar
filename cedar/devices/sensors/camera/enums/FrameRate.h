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

    File:        CameraFrameRate.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description:  Header for CameraFrameRate enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_FRAMERATE_H
#define CEDAR_DEV_SENSORS_CAMERA_FRAMERATE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/camera/namespace.h"

// SYSTEM INCLUDES

/*!@brief Enum class for firewire camera fps setting
 *
 * Use this type for the CameraGrabber::setCameraFps() and CameraGrabber::getCameraFps() method.
 *
 * @remarks
 * From OpenCV documentation: <br>
 * This enumeration is used for non-Format_7 modes. The framerate can be lower than expected if the
 * exposure time is longer than the requested frame period. Framerate can be controlled in a number of
 * other ways: framerate feature, external trigger, software trigger, shutter throttling and packet size
 * (Format_7)
 */
class cedar::dev::sensors::camera::FrameRate
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
  static const cedar::dev::sensors::camera::FrameRate::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::camera::FrameRate> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Allow OpenCV backend to set the framerate of the attached camera
   *
   * You can set the framerate through the the CameraGrabber::setCameraFps() method
   */
  static const Id FPS_NOT_SET = UINT_MAX-2;

  /*! @brief Set the framerate to 2 fps with the CameraGrabber::setCameraFps() method
   *
   * For firewire-cameras the framerate will be set to 1.875 fps
   */
  static const Id FPS_2 = 2;

  /*! @brief Set the framerate to 4 fps
   *
   * For firewire-cameras the framerate will be set to 3.75 fps
   */
  static const Id FPS_4 = 4;

  /*! @brief Set the framerate to 8 fps
   *
   * For firewire-cameras the framerate will be set to 7.5 fps
   */
  static const Id FPS_8 = 8;
  /// @brief Set the framerate to 15 fps
  static const Id FPS_15 = 15;
  /// @brief Set the framerate to 30 fps
  static const Id FPS_30 = 30;
  /// @brief Set the framerate to 60 fps
  static const Id FPS_60 = 60;
  /// @brief Set the framerate to 120 fps
  static const Id FPS_120 = 120;
  /// @brief Set the framerate to 240 fps
  static const Id FPS_240 = 240;


protected:
  // none yet
private:
  // none yet

}; // cedar::dev::sensors::camera::FrameRate

#endif // CEDAR_DEV_SENSORS_CAMERA_FRAMERATE_H

