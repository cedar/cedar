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

    File:        camera/defines.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2012 01 27

    Description: Common structures and definitions used by the CameraGrabber class and his sub-classes

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_DEFINES_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_DEFINES_H

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/camera/CameraProperty.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <set>

//--------------------------------------------------------------------------------------------------------------------
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
/*! \struct CameraId
 *  \brief Store the unique Id of a camera
 */
struct CameraId
{
  ///! The bus id
  unsigned int busId;
  ///! The unique id of the device
  unsigned int guid;
};

//--------------------------------------------------------------------------------------------------------------------
///! std::set to store all supported properties in
//typedef std::set<cedar::dev::sensors::visual::CameraProperty::Id> SupportedPropertiesSet;

//--------------------------------------------------------------------------------------------------------------------
///! std::map to map property enum id to the value of the property
typedef std::map<cedar::dev::sensors::visual::CameraProperty::Id, double> CameraPropertyValues;
typedef std::pair<cedar::dev::sensors::visual::CameraProperty::Id, double> CameraPropertyValuesPair;

//--------------------------------------------------------------------------------------------------------------------
///! macro to create a pair of property-enum to property value
#define PROPERTY_VALUE_PAIR(A,B) std::pair<unsigned int,int>((A),(B))


//--------------------------------------------------------------------------------------------------------------------
/*! \struct CameraSettings
 *  \brief Structure to store the values of the settings (as an enum string) of a camera for the
 *    configuration interface
 */
struct CameraSettings
{
  //!@cond SKIPPED_DOCUMENTATION
  std::string fps;
  std::string mode;
  std::string iso_speed;
  std::string capability_config_file_name;
  //!@endcond
};

//--------------------------------------------------------------------------------------------------------------------
/*! \brief Stores the capability of a property
 *  \remarks Every property have several ways to be accessed, and a defined range
 *    where the values can be set.
 *    This struct stores flags for all possible ways and the range of the values
 */
struct CameraPropertyCapability
{
  //!@cond SKIPPED_DOCUMENTATION
   cedar::dev::sensors::visual::CameraProperty::Id propId;
   int max_value;
   int min_value;
   bool is_supported;
   bool is_readable;
   bool is_one_push_capable;
   bool is_on_off_capable;
   bool is_auto_capable;
   bool is_manual_capable;
   bool is_absolute_capable;
   //!@endcond
} ;

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_DEFINES_H
