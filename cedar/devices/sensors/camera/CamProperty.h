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

#ifndef CEDAR_DEV_SENSORS_CAMERA_CAMPROPERTY_H
#define CEDAR_DEV_SENSORS_CAMERA_CAMPROPERTY_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/enums/Property.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/devices/sensors/camera/enums/PropertyMode.h"
#include "cedar/devices/sensors/camera/namespace.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/EnumType.h"

// SYSTEM INCLUDES
#include <QObject>


/*!@brief This class implements the capabilities of one property
 *
 * A property is a specific property from the cv::VideoCapture class
 */
class cedar::dev::sensors::camera::CamProperty
:
public QObject,
public cedar::aux::Configurable
{

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  Q_OBJECT

  protected slots:
  void propertyValueSlot();
  void propertyModeSlot();

signals:

  //!@brief This signal is emitted, when the value of the property has changed.
  void propertyValueChanged(cedar::dev::sensors::camera::Property::Id propertyId, double newValue);

  //!@brief This signal is emitted, when the mode of the property has changed.
  void propertyModeChanged
  (
    cedar::dev::sensors::camera::Property::Id propertyId,
    cedar::dev::sensors::camera::PropertyMode::Id newMode
  );

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

#ifdef CEDAR_USE_LIB_DC1394
  friend class cedar::dev::sensors::camera::DeviceDc1394;
#endif // CEDAR_USE_LIB_DC1394

  //!@brief The standard constructor.
  CamProperty
  (
   cedar::dev::sensors::camera::Property::Id id = 0,
   std::string name = "property",
   double minValue = (double) INT_MIN,
   double maxValue = (double) INT_MAX,
   double defaultValue = 128,
   bool supported = true,
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

  //enable the property, i.e. set constant
  void disable();

  //disable the property
  void enable();

  //! set the default value from backend initialization. This method is invoked from the createGrabber() method to set
  //! the default backend value;
  void setDefaultValue(double value);

  /// Get the id of the actual Property
  cedar::dev::sensors::camera::Property::Id getId();


  /// Get the mode of operation of the used parameter
  cedar::dev::sensors::camera::PropertyMode::Id getMode();

  /// Set the mode of operation
  void setMode(cedar::dev::sensors::camera::PropertyMode::Id modeId);

  // get the shared-pointer to the parameter
  // cedar::aux::DoubleParameterPtr getParameter();

  ///! Get the value of the property
  double getValue();

  ///! Set the value of the property
  // @param value The new value to set
  void setValue(double value);

  ///! Get the minimum possible value of the property
  double getMinValue();

  /*! \brief Get the maximum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  double getMaxValue();

  /*! \brief This method tells you, if the given property is supported by the used camera
   *  \param propId The id of the  property
   */
  bool isSupported();

  /*! \brief This method tells you, if the given property can be set to auto-mode
   */
  bool isAutoCapable();

  /*! \brief This method tells you, if the given property can be set manually
   */
  bool isManualCapable();

  // ACTUALLY NOT SUPPORTED BY CEDAR
  /*! \brief This method tells you, if the given property can be set to an absolute value
   */
  // bool isAbsoluteCapable();

  /*! \brief This method tells you, if the given property supports the OnePushAuto mode
   *
   *     OnePushAuto is a special mode. <br>
   *     It is used as follows: Set a value to a property and then to OnePushAuto mode.
   *     The camera now will try to hold this value automatically.
   *  \param propId The id of the  property
   */
  // bool isOnePushCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property could be turn off and on
   */
  // bool isOnOffCapable();

  /// Set the mode of operation internal to the backend
  // void setModeToCam(cedar::dev::sensors::camera::PropertyMode::Id modeId);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  void updateGuiElements();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::dev::sensors::camera::Property::Id mId;
  std::string mName;
  bool mSupported;
  bool mAutoCapable;
  bool mManualCapable;
  // bool mOnOffCapable;

  //! stores the default value from the camera initialization
  double mDefaultValue;

  //disable eventhandling for enum parameters during initialization and update
  bool doNotHandleEvents;

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

// typedef cedar::aux::SetParameter<cedar::dev::sensors::visual::Property> CameraPropertiesSet;

#endif // CEDAR_DEV_SENSORS_CAMERA_CAMPROPERTY_H

