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

    File:        Property.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::visual::Property class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMPROPERTY_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMPROPERTY_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/enums/CameraProperty.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/devices/sensors/visual/camera/enums/CameraPropertyMode.h"
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/EnumType.h"

// SYSTEM INCLUDES


/*!@brief This class implements the capabilities of one property
 *
 * A property is a specific property from the cv::VideoCapture class
 */
class cedar::dev::sensors::visual::CamProperty
:
public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:


  //!@brief The standard constructor.
  CamProperty
  (
   cedar::dev::sensors::visual::CameraProperty::Id id = 0,
   std::string name = "property",
   double minValue = (double) INT_MIN,
   double maxValue = (double) INT_MAX,
   double defaultValue = 128,
   bool supported = true,
   bool readable = true,
   bool autoCapable = false,
   bool manualCapable = true
//   bool onOffCapable = false
  );

  //!@brief Destructor
  ~CamProperty();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /// Get the id of the actual Property
  cedar::dev::sensors::visual::CameraProperty::Id getId();

  // Set the id
  //void setId(cedar::dev::sensors::visual::CameraProperty::Id id);

  /// Get the mode of operation of the used parameter
  cedar::dev::sensors::visual::CameraPropertyMode::Id getMode();

  /// Set the mode of operation
  void setMode(cedar::dev::sensors::visual::CameraPropertyMode::Id mode);

  //get the shared-pointer to the parameter
  //cedar::aux::DoubleParameterPtr getParameter();

  /*! \brief Get the minimum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  double getMinValue();

  /*! \brief Get the maximum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  double getMaxValue();

  /*! \brief This method tells you, if the given property is supported by the used camera
   *  \param propId The id of the  property
   */
  bool isSupported();

  /*! \brief This method tells you, if the given property is readable by the used camera
   */
  bool isReadable();

  /*! \brief This method tells you, if the given property can be set to auto-mode
   */
  bool isAutoCapable();

  /*! \brief This method tells you, if the given property can be set manually
   */
  bool isManualCapable();

  // ACTUALLY NOT SUPPORTED BY CEDAR
  /*! \brief This method tells you, if the given property can be set to an absolute value
   */
  //bool isAbsoluteCapable();

  /*! \brief This method tells you, if the given property supports the OnePushAuto mode
   *
   *     OnePushAuto is a special mode. <br>
   *     It is used as follows: Set a value to a property and then to OnePushAuto mode.
   *     The camera now will try to hold this value automatically.
   *  \param propId The id of the  property
   */
  //bool isOnePushCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property could be turn off and on
   */
  //bool isOnOffCapable();


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::dev::sensors::visual::CameraProperty::Id mId;
  std::string mName;
  bool mSupported;
  bool mReadable;
  bool mAutoCapable;
  bool mManualCapable;
  //bool mOnOffCapable;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // the real value of the parameter from the camera
  cedar::aux::DoubleParameterPtr mpPropertyValue;

  // operational mode
  cedar::aux::EnumParameterPtr mpPropertyMode;

private:
  // none yet

}; // class cedar::dev::sensors::visual::CamProperty

//typedef cedar::aux::SetParameter<cedar::dev::sensors::visual::Property> CameraPropertiesSet;

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMPROPERTY_H

