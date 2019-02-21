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

    File:        CameraSetting.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description:  Header for CameraProperty enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTING_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTING_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/camera/namespace.h"

// SYSTEM INCLUDES
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/core/version.hpp>

//--------------------------------------------------------------------------------------------------------------------
//(re)defines of our new introduced properties in OpenCV:
#ifndef CV_CAP_PROP_ISO_SPEED
  #define CV_CAP_PROP_ISO_SPEED 30
#endif


/*!@brief Enum class for camera settings.
 *
 * Use this type for the CameraGrabber::setCameraSetting() and CameraGrabber::getCameraSetting() method
 *
 * @remarks
 *  This constants are direct mapped from opencv2/highgui/highui_c.h
 *
 */
class cedar::dev::sensors::camera::Setting
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
  static const cedar::dev::sensors::camera::Setting::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::camera::Setting> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Set the framerate through the CameraGrabber::setCameraSetting() method
   *
   *  This constants are the available settings for the used camera
   *
   *  If you use a firewire camera, this settings can only be applied on startup, i.e. before the first picture
   *  is grabbed with the CameraGrabber::grab() method.
   */
#if (CV_MAJOR_VERSION > 3)
    static const Id FPS = cv::CAP_PROP_FPS; // 5;
  /// @see SETTING_FPS
  static const Id FRAME_WIDTH = cv::CAP_PROP_FRAME_WIDTH; // 3;
  /// @see SETTING_FPS
  static const Id FRAME_HEIGHT = cv::CAP_PROP_FRAME_HEIGHT; // 4;
  /// @see SETTING_FPS
  static const Id MODE = cv::CAP_PROP_MODE; // 9;

#ifdef CEDAR_USE_LIB_DC1394
  /// @see SETTING_FPS
  static const Id ISO_SPEED = cv::CAP_PROP_ISO_SPEED; // 30
#endif
#else
    static const Id FPS = CV_CAP_PROP_FPS; // 5;
    /// @see SETTING_FPS
    static const Id FRAME_WIDTH = CV_CAP_PROP_FRAME_WIDTH; // 3;
    /// @see SETTING_FPS
    static const Id FRAME_HEIGHT = CV_CAP_PROP_FRAME_HEIGHT; // 4;
    /// @see SETTING_FPS
    static const Id MODE = CV_CAP_PROP_MODE; // 9;

#ifdef CEDAR_USE_LIB_DC1394
    /// @see SETTING_FPS
    static const Id ISO_SPEED = CV_CAP_PROP_ISO_SPEED; // 30
#endif
#endif //  CV_MAJOR_VERSION > 3


protected:
  // none yet
private:
  // none yet


}; // cedar::dev::sensors::camera::Setting

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTING_H

