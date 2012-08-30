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

#include "cedar/configuration.h"   // MAKE FIREWIRE OPTIONAL

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/visual/namespace.h"

// SYSTEM INCLUDES
#include <opencv2/highgui/highgui_c.h>



//--------------------------------------------------------------------------------------------------------------------
// (re)defines of our new introduced properties in OpenCV:
// for compatibility with OpenCV prior 2.4.0
//@todo: check OpenCV version with ifdef
#define CV_CAP_PROP_ZOOM 27
#define CV_CAP_PROP_FOCUS 28
#define CV_CAP_PROP_GUID 29

//mappings from OpenCV constants
//return value, if a property is not supported from the actual device
#define CAMERA_PROPERTY_NOT_SUPPORTED -1

//sets a feature to auto-mode. The propery have to be auto_capable.
//Assigning a value to a feature, sets its mode to manual // =-2
#define CAMERA_PROPERTY_MODE_AUTO CV_CAP_PROP_DC1394_MODE_AUTO

//if a feature is on/off capable, this value turns it off //=-4
#define CAMERA_PROPERTY_MODE_OFF CV_CAP_PROP_DC1394_OFF

//this sets a property to its one_push_auto mode. The feature have to be one_push_auto capable.
//one_push_auto: set the property to the wanted value and then to one_push_auto.
//the camera will try to automatically hold this value //-1
#define CAMERA_PROPERTY_MODE_ONE_PUSH_AUTO CV_CAP_PROP_DC1394_MODE_ONE_PUSH_AUTO


//--------------------------------------------------------------------------------------------------------------------
/*!@brief Enum class for camera properties direct mapped from opencv2/highgui/highui_c.h
 *
 * \remarks
 *    Use this type for the CameraGrabber::setCameraProperty() and getCameraProperty() method
 * \par
 *    Until now, only properties supported by the OpenCV backend are implemented
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
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Set the brightness through the CameraGrabber::setCameraProperty() method
   *
   *  @remarks Not all of this properties are supported on the different camera models
   */
  static const Id PROP_BRIGHTNESS = CV_CAP_PROP_BRIGHTNESS; // = 10
  /// @see PROP_BRIGHTNESS
  static const Id PROP_SATURATION = CV_CAP_PROP_SATURATION; // = 12;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_HUE = CV_CAP_PROP_HUE; //13;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_GAIN = CV_CAP_PROP_GAIN; //14;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_EXPOSURE = CV_CAP_PROP_EXPOSURE; //15;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_WHITE_BALANCE_BLUE_U = CV_CAP_PROP_WHITE_BALANCE_BLUE_U; //17;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_SHARPNESS = CV_CAP_PROP_SHARPNESS; //20;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_AUTO_EXPOSURE = CV_CAP_PROP_AUTO_EXPOSURE; //21
  /// @see PROP_BRIGHTNESS
  static const Id PROP_GAMMA = CV_CAP_PROP_GAMMA; //22;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_TEMPERATURE = CV_CAP_PROP_TEMPERATURE; // 23;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_TRIGGER = CV_CAP_PROP_TRIGGER; // 24;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_TRIGGER_DELAY = CV_CAP_PROP_TRIGGER_DELAY; // 25;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_WHITE_BALANCE_RED_V = CV_CAP_PROP_WHITE_BALANCE_RED_V; // 26;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_ZOOM = CV_CAP_PROP_ZOOM; // 27;
  /// @see PROP_BRIGHTNESS
  static const Id PROP_FOCUS = CV_CAP_PROP_FOCUS; // 28;

protected:
  // none yet
private:
  // none yet

}; // cedar::dev::sensors::visual::CameraProperty

#endif // CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTY_H

