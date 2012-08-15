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

    File:        CameraProperties.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::visual::CameraProperties class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTIES_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTIES_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraSetting.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraPropertyMode.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraProperty.h"
#include "cedar/devices/sensors/visual/camera/CamProperty.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/Configurable.h"


// SYSTEM INCLUDES


/*!@brief This class manages all properties and their capabilities of one camerachannel
 *
 * A property is a specific property from the cv::VideoCapture class
 */
class cedar::dev::sensors::visual::CameraProperties
:
public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef cedar::aux::EnumId Id;
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CameraProperties();

  //!@brief Destructor
  ~CameraProperties();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  ///! set the value to the cv::VideoCapture object
  bool setPropertyRawValue(unsigned int property_id,double value);

  ///! get the value from the cv::VideoCapture object
  double getPropertyRawValue(unsigned int property_id);

  ///! set the value to the cv::VideoCapture object with respect to capabilities and settings
//  bool setProperty(unsigned int property_id,double value);

  ///! get the value from the cv::VideoCapture object with respect to capabilities and settings
//  double getProperty(unsigned int property);


//  cedar::dev::sensors::visual::CameraPropertiesSet& getProperties();


  bool setSetting(cedar::dev::sensors::visual::CameraSetting::Id settingId, double value);

  double getSetting(cedar::dev::sensors::visual::CameraSetting::Id settingId);


  /// Get the mode of operation of the used parameter
  cedar::dev::sensors::visual::CameraPropertyMode::Id getMode(cedar::dev::sensors::visual::CameraProperty::Id propId);

  /// Set the mode of operation
  void setMode
  (
    cedar::dev::sensors::visual::CameraProperty::Id propId,
    cedar::dev::sensors::visual::CameraPropertyMode::Id mode
  );

  //get the shared-pointer to the parameter
  //cedar::aux::DoubleParameterPtr getParameter();


  /*! \brief Get the minimum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  double getMinValue(CameraProperty::Id propId);

  /*! \brief Get the maximum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  double getMaxValue(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property is supported by the used camera
   *  \param propId The id of the  property
   */
  bool isSupported(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property is readable by the used camera
   *  \param propId The id of the  property
   */
  bool isReadable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property supports the OnePushAuto mode
   *
   *     OnePushAuto is a special mode. <br>
   *     It is used as follows: Set a value to a property and then to OnePushAuto mode.
   *     The camera now will try to hold this value automatically.
   *  \param propId The id of the  property
   */
  //bool isOnePushCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property could be turn off and on
   *  \param propId The id of the  property
   */
  bool isOnOffCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property can be set to auto-mode
   *  \param propId The id of the  property
   */
  bool isAutoCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property can be set manually
   *  \param propId The id of the  property
   */
  bool isManualCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property can be set to an absolute value
   *  \param propId The id of the  property
   */
  //bool isAbsoluteCapable(CameraProperty::Id propId);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //return CampProperty
  //@exception std::out_of_range if the searched value doesn't exist
  cedar::dev::sensors::visual::CamProperty& getPropertyPtr(CameraProperty::Id propId);


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //cedar::dev::sensors::visual::CameraPropertiesSet mProperties;
  cedar::dev::sensors::visual::CameraPropertyMap mPropertiesList;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::CameraProperties

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_PROPERTIES_H

