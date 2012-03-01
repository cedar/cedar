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

    File:        CameraSetting.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description:  Header for CameraProperty enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTING_H
#define CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTING_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/visual/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <opencv2/highgui/highgui_c.h>
//#include "../../../../../../OpenCV/OpenCV-2.3.1_patched/modules/highgui/include/opencv2/highgui/highgui_c.h"

/*!@brief Enum class for camera properties direct mapped from opencv2/highgui/highui_c.h
 * (also in dc1394/control.h)
 *
 * Use this type for the CameraGrabber::setCameraProperty() and getCameraProperty() method
 */
class cedar::dev::sensors::visual::CameraSetting
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
  static const cedar::dev::sensors::visual::CameraSetting::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::visual::CameraSetting> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

  static const Id SETTING_FRAME_WIDTH    = CV_CAP_PROP_FRAME_WIDTH; // 3;
  static const Id SETTING_FRAME_HEIGHT   = CV_CAP_PROP_FRAME_HEIGHT; // 4;
  static const Id SETTING_FPS            = CV_CAP_PROP_FPS; // 5;
  static const Id SETTING_MODE           = CV_CAP_PROP_MODE; // 9;
  static const Id SETTING_ISO_SPEED      = 30; //CV_CAP_PROP_ISO_SPEED; // 30


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

//!@endcond
}; // cedar::dev::sensors::visual::CameraSetting

#endif // CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_SETTING_H

