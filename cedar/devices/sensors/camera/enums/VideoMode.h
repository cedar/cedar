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

    File:        CameraVideoMode.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description:  Header for CameraVideoMode enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_VIDEO_MODE_H
#define CEDAR_DEV_SENSORS_CAMERA_VIDEO_MODE_H

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/camera/namespace.h"

// SYSTEM INCLUDES
#ifdef CEDAR_USE_LIB_DC1394
  #include <dc1394/types.h>
#endif

/*!@brief Enum class for camera frame mode settings.
 *
 * Use this type for the CameraGrabber::setCameraMode() and getCameraMode() method.
 */
class cedar::dev::sensors::camera::VideoMode
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
  static const cedar::dev::sensors::camera::VideoMode::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::camera::VideoMode> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*!@brief This is the default setting for the video mode of the camera in the CameraGrabber class.
   *
   * If you get this value as a return value, then the default value of the OpenCV backend is used.
   * In the case of a firewire-camera OpenCV uses the best available mode when starting the camera
   */
  static const Id MODE_NOT_SET = UINT_MAX-2;

  /*!@brief Use the first available videomode
   *
   * This constants used in the CameraCrabber class to set the video mode of the camera by supported modes
   *
   * @remarks
   * Use MODE_BY_NUM to check which modes are supported from the cam
   * OpenCV firewire backend stores supported video modes in a list and
   * it is possible to change the mode through the indizes of this list
   */
  static const Id NUM_0 = 0;
  /// @see NUM_0
  static const Id NUM_1 = 1;
  /// @see NUM_0
  static const Id NUM_2 = 2;
  /// @see NUM_0
  static const Id NUM_3 = 3;
  /// @see NUM_0
  static const Id NUM_4 = 4;
  /// @see NUM_0
  static const Id NUM_5 = 5;
  /// @see NUM_0
  static const Id NUM_6 = 6;
  /// @see NUM_0
  static const Id NUM_7 = 7;
  /// @see NUM_0
  static const Id NUM_8 = 8;
  /// @see NUM_0
  static const Id NUM_9 = 9;


#ifdef CEDAR_USE_LIB_DC1394
  //Firewire Camera modes

  /*!@brief Set the mode of a firewire camera to 160x120 pixel and YUV444
   *
   * @remarks
   *   This constants are only available if you are using a firewire/IEEE1394 camera. The
   *   camera modes are directly mapped from libdc1394
   */
  static const Id FW_160x120_YUV444 = DC1394_VIDEO_MODE_160x120_YUV444; //=64
  /// @see FW_160x120_YUV444
  static const Id FW_320x240_YUV422 = DC1394_VIDEO_MODE_320x240_YUV422;
  /// @see FW_160x120_YUV444
  static const Id FW_640x480_YUV411 = DC1394_VIDEO_MODE_640x480_YUV411;
  /// @see FW_160x120_YUV444
  static const Id FW_640x480_YUV422 = DC1394_VIDEO_MODE_640x480_YUV422;
  /// @see FW_160x120_YUV444
  static const Id FW_640x480_RGB8 = DC1394_VIDEO_MODE_640x480_RGB8;
  /// @see FW_160x120_YUV444
  static const Id FW_640x480_MONO8 = DC1394_VIDEO_MODE_640x480_MONO8;
  /// @see FW_160x120_YUV444
  static const Id FW_640x480_MONO16 = DC1394_VIDEO_MODE_640x480_MONO16;
  /// @see FW_160x120_YUV444
  static const Id FW_800x600_YUV422 = DC1394_VIDEO_MODE_800x600_YUV422;
  /// @see FW_160x120_YUV444
  static const Id FW_800x600_RGB8 = DC1394_VIDEO_MODE_800x600_RGB8;
  /// @see FW_160x120_YUV444
  static const Id FW_800x600_MONO8 = DC1394_VIDEO_MODE_800x600_MONO8;
  /// @see FW_160x120_YUV444
  static const Id FW_1024x768_YUV422 = DC1394_VIDEO_MODE_1024x768_YUV422;
  /// @see FW_160x120_YUV444
  static const Id FW_1024x768_RGB8 = DC1394_VIDEO_MODE_1024x768_RGB8;
  /// @see FW_160x120_YUV444
  static const Id FW_1024x768_MONO8 = DC1394_VIDEO_MODE_1024x768_MONO8;
  /// @see FW_160x120_YUV444
  static const Id FW_800x600_MONO16 = DC1394_VIDEO_MODE_800x600_MONO16;
  /// @see FW_160x120_YUV444
  static const Id FW_1024x768_MONO16 = DC1394_VIDEO_MODE_1024x768_MONO16;
  /// @see FW_160x120_YUV444
  static const Id FW_1280x960_YUV422 = DC1394_VIDEO_MODE_1280x960_YUV422;
  /// @see FW_160x120_YUV444
  static const Id FW_1280x960_RGB8 = DC1394_VIDEO_MODE_1280x960_RGB8;
  /// @see FW_160x120_YUV444
  static const Id FW_1280x960_MONO8 = DC1394_VIDEO_MODE_1280x960_MONO8;
  /// @see FW_160x120_YUV444
  static const Id FW_1600x1200_YUV422 = DC1394_VIDEO_MODE_1600x1200_YUV422;
  /// @see FW_160x120_YUV444
  static const Id FW_1600x1200_RGB8 = DC1394_VIDEO_MODE_1600x1200_RGB8;
  /// @see FW_160x120_YUV444
  static const Id FW_1600x1200_MONO8 = DC1394_VIDEO_MODE_1600x1200_MONO8;
  /// @see FW_160x120_YUV444
  static const Id FW_1280x960_MONO16 = DC1394_VIDEO_MODE_1280x960_MONO16;
  /// @see FW_160x120_YUV444
  static const Id FW_1600x1200_MONO16 = DC1394_VIDEO_MODE_1600x1200_MONO16;
  /// @see FW_160x120_YUV444
  static const Id FW_EXIF = DC1394_VIDEO_MODE_EXIF;

  /* Format7 isn't supported from OpenCV firewire backend
  /// @see FW_160x120_YUV444
  static const Id FW_FORMAT7_0 = DC1394_VIDEO_MODE_FORMAT7_0;
  /// @see FW_160x120_YUV444
  static const Id FW_FORMAT7_1 = DC1394_VIDEO_MODE_FORMAT7_1;
  /// @see FW_160x120_YUV444
  static const Id FW_FORMAT7_2 = DC1394_VIDEO_MODE_FORMAT7_2;
  /// @see FW_160x120_YUV444
  static const Id FW_FORMAT7_3 = DC1394_VIDEO_MODE_FORMAT7_3;
  /// @see FW_160x120_YUV444
  static const Id FW_FORMAT7_4 = DC1394_VIDEO_MODE_FORMAT7_4;
  /// @see FW_160x120_YUV444
  static const Id FW_FORMAT7_5 = DC1394_VIDEO_MODE_FORMAT7_5;
  /// @see FW_160x120_YUV444
  static const Id FW_FORMAT7_6 = DC1394_VIDEO_MODE_FORMAT7_6;
  /// @see FW_160x120_YUV444
  static const Id FW_FORMAT7_7 = DC1394_VIDEO_MODE_FORMAT7_7;
  */
#endif // CEDAR_USE_LIB_DC1394

protected:
  // none yet
private:
  // none yet

}; // cedar::dev::sensors::camera::VideoMode

#endif // CEDAR_DEV_SENSORS_CAMERA_VIDEO_MODE_H

