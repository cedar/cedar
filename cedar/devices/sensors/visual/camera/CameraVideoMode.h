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

    File:        CameraVideoMode.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description:  Header for CameraVideoMode enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_VIDEO_MODE_H
#define CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_VIDEO_MODE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "../namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <dc1394/types.h>


/*!@brief Enum class for firewire camera frame mode settings. Directly mapped from dc1394/types.h
 *
 * Use this type for the CameraGrabber::setCameraInitMode() and getCameraInitMode() method.
 */
class cedar::dev::sensors::visual::CameraVideoMode
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
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
  static const cedar::dev::sensors::visual::CameraVideoMode::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::visual::CameraVideoMode> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  //This is the default setting for OpenCV CvVideoCapture class
  //As return value this mode means, that the camera is not yet in grabbing mode
  //and there was no other mode to set. In the case of a firewire-camera
  //OpenCV uses the best available mode when starting the grabbing
  static const Id MODE_NOT_SET = -1; //id is unsigned int: -1 is then interpreted as ULONG_MAX

  //use MODE_BY_NUM to check which modes supported.
  //OpenCV firewire backend stores supported video modes in a list and
  //it is possible to change the mode through the indizes of this list
  static const Id MODE_BY_NUM_0 = 0;
  static const Id MODE_BY_NUM_1 = 1;
  static const Id MODE_BY_NUM_2 = 2;
  static const Id MODE_BY_NUM_3 = 3;
  static const Id MODE_BY_NUM_4 = 4;
  static const Id MODE_BY_NUM_5 = 5;
  static const Id MODE_BY_NUM_6 = 6;
  static const Id MODE_BY_NUM_7 = 7;
  static const Id MODE_BY_NUM_8 = 8;
  static const Id MODE_BY_NUM_9 = 9;

  //Firewire Camera modes
  //camera modes directly mapped from libdc1394
  static const Id MODE_FW_160x120_YUV444  = DC1394_VIDEO_MODE_160x120_YUV444; //=64
  static const Id MODE_FW_320x240_YUV422  = DC1394_VIDEO_MODE_320x240_YUV422;
  static const Id MODE_FW_640x480_YUV411  = DC1394_VIDEO_MODE_640x480_YUV411;
  static const Id MODE_FW_640x480_YUV422  = DC1394_VIDEO_MODE_640x480_YUV422;
  static const Id MODE_FW_640x480_RGB8    = DC1394_VIDEO_MODE_640x480_RGB8;
  static const Id MODE_FW_640x480_MONO8   = DC1394_VIDEO_MODE_640x480_MONO8;
  static const Id MODE_FW_640x480_MONO16  = DC1394_VIDEO_MODE_640x480_MONO16;
  static const Id MODE_FW_800x600_YUV422  = DC1394_VIDEO_MODE_800x600_YUV422;
  static const Id MODE_FW_800x600_RGB8    = DC1394_VIDEO_MODE_800x600_RGB8;
  static const Id MODE_FW_800x600_MONO8   = DC1394_VIDEO_MODE_800x600_MONO8;
  static const Id MODE_FW_1024x768_YUV422 = DC1394_VIDEO_MODE_1024x768_YUV422;
  static const Id MODE_FW_1024x768_RGB8   = DC1394_VIDEO_MODE_1024x768_RGB8;
  static const Id MODE_FW_1024x768_MONO8  = DC1394_VIDEO_MODE_1024x768_MONO8;
  static const Id MODE_FW_800x600_MONO16  = DC1394_VIDEO_MODE_800x600_MONO16;
  static const Id MODE_FW_1024x768_MONO16 = DC1394_VIDEO_MODE_1024x768_MONO16;
  static const Id MODE_FW_1280x960_YUV422 = DC1394_VIDEO_MODE_1280x960_YUV422;
  static const Id MODE_FW_1280x960_RGB8   = DC1394_VIDEO_MODE_1280x960_RGB8;
  static const Id MODE_FW_1280x960_MONO8  = DC1394_VIDEO_MODE_1280x960_MONO8;
  static const Id MODE_FW_1600x1200_YUV422= DC1394_VIDEO_MODE_1600x1200_YUV422;
  static const Id MODE_FW_1600x1200_RGB8  = DC1394_VIDEO_MODE_1600x1200_RGB8;
  static const Id MODE_FW_1600x1200_MONO8 = DC1394_VIDEO_MODE_1600x1200_MONO8;
  static const Id MODE_FW_1280x960_MONO16 = DC1394_VIDEO_MODE_1280x960_MONO16;
  static const Id MODE_FW_1600x1200_MONO16=DC1394_VIDEO_MODE_1600x1200_MONO16;
  static const Id MODE_FW_EXIF =            DC1394_VIDEO_MODE_EXIF;
  static const Id MODE_FW_FORMAT7_0 =       DC1394_VIDEO_MODE_FORMAT7_0;
  static const Id MODE_FW_FORMAT7_1 =       DC1394_VIDEO_MODE_FORMAT7_1;
  static const Id MODE_FW_FORMAT7_2 =       DC1394_VIDEO_MODE_FORMAT7_2;
  static const Id MODE_FW_FORMAT7_3 =       DC1394_VIDEO_MODE_FORMAT7_3;
  static const Id MODE_FW_FORMAT7_4 =       DC1394_VIDEO_MODE_FORMAT7_4;
  static const Id MODE_FW_FORMAT7_5 =       DC1394_VIDEO_MODE_FORMAT7_5;
  static const Id MODE_FW_FORMAT7_6 =       DC1394_VIDEO_MODE_FORMAT7_6;
  static const Id MODE_FW_FORMAT7_7 =       DC1394_VIDEO_MODE_FORMAT7_7;

protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::xxx

#endif // CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_VIDEO_MODE_H

