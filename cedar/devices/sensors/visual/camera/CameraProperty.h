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

    File:        CameraProperty.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description:  Header for CameraProperty enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTY_H
#define CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTY_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/visual/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <opencv2/highgui/highgui_c.h>



//(re)defines of our new introduced properties in OpenCv:
//comment this out, if the patched OpenCv is available
//#define CV_CAP_PROP_ZOOM 27
//#define CV_CAP_PROP_FOCUS 28
//#define CV_CAP_PROP_GUID 29


/*!@brief Enum class for camera properties direct mapped from opencv2/highgui/highui_c.h
 * (also in dc1394/control.h)
 *
 * \remarks
 *    Use this type for the CameraGrabber::setCameraProperty() and getCameraProperty() method
 * \par
 *    Until now, only properties supported by the opencv backend are implemented
 *
 */
class cedar::dev::sensors::visual::CameraProperty
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
  static const cedar::dev::sensors::visual::CameraProperty::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::visual::CameraProperty> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

  static const Id PROP_BRIGHTNESS    = CV_CAP_PROP_BRIGHTNESS; // = 10
  static const Id PROP_SATURATION    = CV_CAP_PROP_SATURATION; // = 12;
  static const Id PROP_HUE           = CV_CAP_PROP_HUE; //13;
  static const Id PROP_GAIN          = CV_CAP_PROP_GAIN; //14;
  static const Id PROP_EXPOSURE      = CV_CAP_PROP_EXPOSURE; //15;
  static const Id PROP_WHITE_BALANCE_BLUE_U = CV_CAP_PROP_WHITE_BALANCE_BLUE_U; //17;
  static const Id PROP_SHARPNESS     = CV_CAP_PROP_SHARPNESS; //20;
  static const Id PROP_AUTO_EXPOSURE = CV_CAP_PROP_AUTO_EXPOSURE; //21
  static const Id PROP_GAMMA         = CV_CAP_PROP_GAMMA; //22;
  static const Id PROP_TEMPERATURE   = CV_CAP_PROP_TEMPERATURE; // 23;
  static const Id PROP_TRIGGER       = CV_CAP_PROP_TRIGGER; // 24;
  static const Id PROP_TRIGGER_DELAY = CV_CAP_PROP_TRIGGER_DELAY; // 25;
  static const Id PROP_WHITE_BALANCE_RED_V = CV_CAP_PROP_WHITE_BALANCE_RED_V; // 26;
  static const Id PROP_ZOOM          = CV_CAP_PROP_ZOOM; // 27;
  static const Id PROP_FOCUS         = CV_CAP_PROP_FOCUS; // 28;

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
}; // cedar::dev::sensors::visual::CameraProperty

#endif // CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTY_H

